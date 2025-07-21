/* --------------------------------------------------------------------------
 * File: ignition_system.c
 * Description: Source file for STM32 multi-cylinder ignition system
 * --------------------------------------------------------------------------*/

#include "ignition_system.h"
#include "ignition_state_machine.h"
#include "ignition_cylinder.h"
#include "voltage_map.h"
#include "cylinder_scheduler.h"
#include <string.h>
#include <stdio.h>

// Static instance of the ignition state machine
static IgnitionStateMachine fsm;

//#define RPM_SAMPLE_INTERVAL 5000 // 5 ms //50ms previos
//#define MIN_ENCODER_DELTA 2       // Minimum change in encoder to detect motion

//#define RPM_SAMPLE_INTERVAL 20000  // Lower interval
//#define MIN_ENCODER_DELTA 1        // Accept smaller movement


/**
 * @brief Initializes the ignition system and timers
 */
IS_Status IS_Init(IgnitionSystem *sys, IS_Config *cfg) {
    if (!sys || !cfg) return IS_ERROR_NULL_PTR;
    memcpy((void*)&sys->cfg, cfg, sizeof(IS_Config));

    memset(&sys->state, 0, sizeof(IS_State)); //zero init
    sys->state.engine_state = ENGINE_STOPPED;

    ISM_Init(&fsm);

    // Start encoder and timers
    HAL_TIM_Encoder_Start_IT(sys->cfg.encoder_timer, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(sys->cfg.input_capture_timer);
    HAL_TIM_IC_Start_IT(sys->cfg.input_capture_timer, TIM_CHANNEL_1);
    HAL_TIM_Base_Start(sys->cfg.time_base_timer);

    return IS_OK;
}

/**
 * @brief Handles error state: stops all coils and resets engine state
 */
IS_Status IS_HandleError(IgnitionSystem *sys) {
    if (!sys) return IS_ERROR_NULL_PTR;
    for (int i = 0; i < TOTAL_CYLINDERS; i++) {
        HAL_GPIO_WritePin(sys->cfg.coil_ports[i], sys->cfg.coil_pins[i], GPIO_PIN_RESET);
        sys->state.coil_state[i] = COIL_OFF;
    }
    sys->state.engine_state = ENGINE_STOPPED;
    return IS_OK;
}

/**
 * @brief Updates engine RPM based on encoder position change over time
 */

IS_Status IS_UpdateRPM(IgnitionSystem *sys) {
    if (!sys) return IS_ERROR_NULL_PTR;
    unsigned int current_time = __HAL_TIM_GET_COUNTER(sys->cfg.time_base_timer);
    unsigned int current_position = __HAL_TIM_GET_COUNTER(sys->cfg.encoder_timer);

    if ((current_time - sys->state.timestamp_last_position >= RPM_SAMPLE_INTERVAL) ||
        (current_time < sys->state.timestamp_last_position && (UINT32_MAX - sys->state.timestamp_last_position + current_time) >= RPM_SAMPLE_INTERVAL)) {

        unsigned int delta_pos = (current_position >= sys->state.encoder_last_position)
            ? current_position - sys->state.encoder_last_position
            : (0xFFFF - sys->state.encoder_last_position + current_position);

        if (delta_pos < ENCODER_MIN_DELTA) {
            sys->state.engine_state = ENGINE_STOPPED;
            sys->state.frequency = 0;
            sys->state.rpm = 0;
        } else {
            sys->state.engine_state = ENGINE_RUNNING;
            sys->state.frequency = sys->state.tim4_frequency;
            sys->state.rpm = sys->state.tim4_frequency * 2; // Half camshaft encoder
        }

        sys->state.encoder_last_position = current_position;
        sys->state.timestamp_last_position = current_time;
    }
    return IS_OK;
}

/** ... rest of the file unchanged ... */

/**
 * @brief Calculates how many encoder pulses are needed to charge the coil
 *        for a given RPM and battery voltage
 */
float batt_voltage = -1.f;
IS_Status IS_CalculatePulseTime(IgnitionSystem *sys) {
    if (!sys) return IS_ERROR_NULL_PTR;
    if (sys->state.rpm == 0) return IS_ERROR_INVALID_STATE;

    batt_voltage = Read_BatteryVoltage(sys->cfg.battery_adc)* (V_MAX - V_MIN) / VREF + V_MIN;

    //float batt_voltage = 15;
    if (batt_voltage < 0.0f) return IS_ERROR_INVALID_STATE;

    float dwell_time = Lookup_DwellTime(batt_voltage);

    double timePerRev = 60.0 / sys->state.rpm; // calcs in seconds and 60 seconds per minute
    double timePerPulse = timePerRev * 1000.0 / ENCODER_TIMER_PPR;//in ms //120;//ENCODER_TIMER_PPR;
    double pulsesNeeded = dwell_time / timePerPulse; // required pulses

    sys->state.pulses = (int)(pulsesNeeded + 0.5);
    sys->state.totalTimeMs = sys->state.pulses * timePerPulse;
    return IS_OK;
}

/**
 * @brief Controls coil ON/OFF based on state machine and current cylinder
 */


IS_Status IS_ControlCoil(IgnitionSystem *sys) {
    if (!sys)
        return IS_ERROR_NULL_PTR;

    if (sys->state.engine_state == ENGINE_STOPPED || sys->state.rpm < CONF_MIN_RPM) {
        for (int i = 0; i < TOTAL_CYLINDERS; ++i) {
            HAL_GPIO_WritePin(sys->cfg.coil_ports[i], sys->cfg.coil_pins[i], GPIO_PIN_RESET);
            sys->state.coil_state[i] = COIL_OFF;
        }
        return IS_OK; // Skip further processing
    }
    uint32_t encoder_pos = __HAL_TIM_GET_COUNTER(sys->cfg.encoder_timer);
    if (sys->state.isCrossed)
        sys->state.encoder_wrapped_position = ENCODER_TIMER_ENGINE_HALF_CYCLE_PULSES + encoder_pos;
    else
        sys->state.encoder_wrapped_position = encoder_pos;

    if (sys->state.encoder_wrapped_position >= ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES) {
        sys->state.encoder_wrapped_position = 0;
        //sys->state.isCrossed = false;
    }
    sys->state.cyl_no = CylinderScheduler_GetCurrent(sys->state.encoder_wrapped_position);
    //sys->state.cyl_no = CylinderScheduler_GetCurrent(encoder_pos);

    if (sys->state.cyl_no < 1 || sys->state.cyl_no > TOTAL_CYLINDERS)
        return IS_ERROR_INVALID_STATE;

    sys->state.angle = CylinderScheduler_GetIgnitionAngle(sys->state.cyl_no);

    // update FSM
    ISM_Update(&fsm, sys->state.encoder_wrapped_position, sys->state.pulses, sys->state.angle, sys->state.cyl_no);

    int idx = fsm.current_cylinder - 1;

    if (fsm.coil_on) { //fsm says to turn the coil on
        if (sys->state.coil_state[idx] == COIL_OFF) // if coil is off
            sys->state.coil_on_start[idx] = HAL_GetTick(); // tracks time just before turning it on

        HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx], GPIO_PIN_SET); // turns on the coil
        sys->state.coil_state[idx] = COIL_ON; //sets the state to on
    } else {
        if (sys->state.coil_state[idx] == COIL_ON)
            sys->state.coil_durations[idx] = HAL_GetTick() - sys->state.coil_on_start[idx]; // if it is on then checks how much time have passed

        HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx], GPIO_PIN_RESET);
        sys->state.coil_state[idx] = COIL_OFF;
    }

    return IS_OK;
}


//IS_Status IS_ControlCoil(IgnitionSystem *sys) {
//	if (!sys)
//		return IS_ERROR_NULL_PTR;
//
//	uint32_t encoder_pos = __HAL_TIM_GET_COUNTER(sys->cfg.encoder_timer);
//
//	sys->state.cyl_no = CylinderScheduler_GetCurrent(encoder_pos);
//	if (sys->state.cyl_no < 1 || sys->state.cyl_no > TOTAL_CYLINDERS)
//		return IS_ERROR_INVALID_STATE;
//
//	float voltage = 11;
//	uint32_t dwel_time = Lookup_DwellTime(voltage);
//
//
////	sys->state.angle = CylinderScheduler_GetIgnitionAngle(sys->state.cyl_no);
////
////	ISM_Update(&fsm, encoder_pos, sys->state.pulses, sys->state.angle);
////
//	//int idx = sys->state.cyl_no - 1;
////	if (ISM_IsCoilOn(&fsm)) {
////		if (sys->state.coil_state[idx] == COIL_OFF) {
////			sys->state.coil_on_start[idx] = HAL_GetTick();
////		}
////		HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx],
////				GPIO_PIN_SET);
////		sys->state.coil_state[idx] = COIL_ON;
////	} else {
////		if (sys->state.coil_state[idx] == COIL_ON) {
////			sys->state.coil_durations[idx] = (float) (HAL_GetTick()
////					- sys->state.coil_on_start[idx]);
////		}
////		HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx],
////				GPIO_PIN_RESET);
////		sys->state.coil_state[idx] = COIL_OFF;
////	}
//
//
//
//	process_ignition_event(sys,sys->state.cyl_no,dwel_time);
//
//	return IS_OK;
//}
//
//void turn_on_cylinder(IgnitionSystem *sys, uint8_t cylinder) {
//    if (cylinder >= 1 && cylinder <= TOTAL_CYLINDERS) {
//        uint8_t idx = cylinder - 1;
//        HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx], GPIO_PIN_SET);
//    }
//}
//
//
//void turn_off_cylinder(IgnitionSystem *sys, uint8_t cylinder) {
//    if (cylinder >= 1 && cylinder <= TOTAL_CYLINDERS) {
//        uint8_t idx = cylinder - 1;
//        HAL_GPIO_WritePin(sys->cfg.coil_ports[idx], sys->cfg.coil_pins[idx], GPIO_PIN_RESET);
//    }
//}
//
//void process_ignition_event(IgnitionSystem *sys, uint8_t cylinder, uint32_t dwell_ms)
//{
//    // If engine is stopped, forcefully deactivate any active coil
//    if (sys->state.engine_state == ENGINE_STOPPED) {
//        if (coil_delay.active) {
//            turn_off_cylinder(coil_delay.sys, coil_delay.cylinder);
//            coil_delay.active = 0;
//        }
//        return;  // Do not proceed
//    }
//
//    // Skip if already a coil is active (i.e., delay running)
//    if (coil_delay.active) return;
//
//    turn_on_cylinder(sys, cylinder);
//
//    coil_delay.active = 1;
//    coil_delay.sys = sys;
//    coil_delay.cylinder = cylinder;
//    coil_delay.dwell_ms = dwell_ms;
//    coil_delay.start_time = HAL_GetTick();
//}
//
//
//void update_coil_delay(void)
//{
//    if (!coil_delay.active) return;
//
//    if ((HAL_GetTick() - coil_delay.start_time) >= coil_delay.dwell_ms) {
//        turn_off_cylinder(coil_delay.sys, coil_delay.cylinder);
//        coil_delay.active = 0;
//    }
//
//
//}


//void process_ignition_event(IgnitionSystem *sys, uint8_t cylinder, uint32_t dwell_ms) {
//    turn_on_cylinder(sys, cylinder);
//    HAL_Delay(dwell_ms);  // Not real-time safe
//    turn_off_cylinder(sys, cylinder);
//}



/**
 * @brief Sends RPM and coil timing debug data over UART
 */
IS_Status IS_UART_Debug(IgnitionSystem *sys) {
    if (!sys || !sys->cfg.uart) return IS_ERROR_NULL_PTR;
    char buf[256];
    char coil_str[TOTAL_CYLINDERS * 4] = {0};
    for (int i = 0; i < TOTAL_CYLINDERS; ++i) {
        strcat(coil_str, sys->state.coil_state[i] ? "ON " : "OFF ");
    }
    snprintf(buf, sizeof(buf), "RPM: %u, Pulses: %d, Time: %.2f ms, Coils: %s\r\n",
             sys->state.rpm, sys->state.pulses, sys->state.totalTimeMs, coil_str);
    HAL_UART_Transmit(sys->cfg.uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    return IS_OK;
}

/**
 * @brief Input Capture callback to measure frequency between encoder pulses
 */
void IS_TIM_IC_CaptureCallback(IgnitionSystem *sys) {
    if (!sys) return;

    if (sys->state.frequency_status == 0) {
        sys->state.pulse_time1 = sys->cfg.input_capture_timer->Instance->CCR1;
        sys->state.count_overflow = 0;
        sys->state.frequency_status = 1;
    } else if (sys->state.frequency_status == 1) {
        sys->state.pulse_time2 = sys->cfg.input_capture_timer->Instance->CCR1;
        sys->state.ticks = (sys->state.pulse_time2 + (sys->state.count_overflow * 65536)) - sys->state.pulse_time1;
        sys->state.tim4_frequency = (uint32_t)(84000000UL / sys->state.ticks);
        sys->state.frequency_status = 0;
    }
}

/**
 * @brief Period elapsed callback to count overflows between input captures
 */
void IS_TIM_PeriodElapsedCallback(IgnitionSystem *sys) { //overflow to detect errors
    if (!sys) return;
    sys->state.count_overflow++;
}

/**
 * @brief External interrupt callback (e.g. encoder index pulse)
 */
void IS_EXTI_Callback(IgnitionSystem *sys, uint16_t GPIO_Pin, uint16_t expected_pin) {
    if (!sys) return;
    static uint8_t index_counter = 0;  // counts number of rising edges (index pulses)

    if (GPIO_Pin == expected_pin) {
    	sys->state.isCrossed ^= 1;
        index_counter++;
        HAL_TIM_Encoder_Stop_IT(sys->cfg.encoder_timer, TIM_CHANNEL_ALL);
        __HAL_TIM_SET_COUNTER(sys->cfg.encoder_timer, 0);
        HAL_TIM_Encoder_Start_IT(sys->cfg.encoder_timer, TIM_CHANNEL_ALL);

        if (index_counter >= (CRANK_CYCLE_DEG/360)) {  // 2 pulses = 720° (full crank cycle)
        	index_counter = 0;
               sys->state.index_detected = 1;
               ISM_IndexDetected(&fsm);
           }
    }
}
//void IS_EXTI_Callback(IgnitionSystem *sys, uint16_t GPIO_Pin, uint16_t expected_pin) {
//    if (!sys) return;
//    if (GPIO_Pin == expected_pin) {
//        HAL_TIM_Encoder_Stop_IT(sys->cfg.encoder_timer, TIM_CHANNEL_ALL);
//        __HAL_TIM_SET_COUNTER(sys->cfg.encoder_timer, 0);
//        HAL_TIM_Encoder_Start_IT(sys->cfg.encoder_timer, TIM_CHANNEL_ALL);
//        sys->state.index_detected = 1;
//        ISM_IndexDetected(&fsm);
//    }
//}



