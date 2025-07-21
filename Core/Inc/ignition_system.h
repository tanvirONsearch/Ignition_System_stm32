/* --------------------------------------------------------------------------
 * File: ignition_system.h
 * Description: Header for multi-cylinder STM32 ignition control
 * --------------------------------------------------------------------------*/

#ifndef INC_IGNITION_SYSTEM_H_
#define INC_IGNITION_SYSTEM_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
//#include <ignition_cylinder.h>
#include "cylinder_scheduler.h"
#include "config.h"

//#define PULSE_PER_REVOLUTION 120
//#define MIN_DURATION_PER_PULSE_AT_200_RPM 4.5f // ms




// -------------------------- Status Codes ----------------------------------
typedef enum {
    IS_OK = 0,
    IS_ERROR_NULL_PTR,
    IS_ERROR_INVALID_STATE,
    IS_ERROR_UNKNOWN
} IS_Status;

// ------------------------- Engine States ----------------------------------
typedef enum {
    ENGINE_STOPPED = 0,
    ENGINE_RUNNING = 1
} EngineState;

typedef enum {
    COIL_OFF = 0,
    COIL_ON = 1
} CoilState;

// -------------------------- Configuration ----------------------------------
typedef struct {
    TIM_HandleTypeDef *encoder_timer;
    TIM_HandleTypeDef *time_base_timer;
    TIM_HandleTypeDef *input_capture_timer;
    UART_HandleTypeDef *uart;
    ADC_HandleTypeDef *battery_adc;
    GPIO_TypeDef *coil_ports[TOTAL_CYLINDERS];
    uint16_t coil_pins[TOTAL_CYLINDERS];
    int ignition_angle;
    const int coil_dwell;
} IS_Config;

// ----------------------------- State ---------------------------------------


typedef struct {
    int pulses;
    double totalTimeMs;
    unsigned int rpm;
    unsigned int frequency;
    unsigned int ticks;
    unsigned int pulse_time1;
    unsigned int pulse_time2;
    unsigned short count_overflow;
    unsigned short frequency_status;

    EngineState engine_state;
//    CoilState coil_state;
    CoilState coil_state[TOTAL_CYLINDERS]; // Track ON/OFF for each coil
    unsigned int encoder_last_position;
    bool isCrossed;
    unsigned int encoder_wrapped_position; //---------------------------------modified
    unsigned int timestamp_last_position;
    unsigned int charge_begin_position;
    int index_detected;
    unsigned int tim4_frequency;
    uint8_t cyl_no;
    int angle;

    uint32_t coil_on_start[TOTAL_CYLINDERS];
    float coil_durations[TOTAL_CYLINDERS];

} IS_State;

// ---------------------- Ignition System Handle -----------------------------
typedef struct {
    IS_Config cfg;
    IS_State state;
} IgnitionSystem;


////----------------------Coil Delay Handle-------------------------------------
//typedef struct {
//    uint8_t active;
//    uint8_t cylinder;
//    uint32_t start_time;
//    uint32_t dwell_ms;
//    IgnitionSystem *sys;
//} CoilDelayState;



// -------------------------- Public API -------------------------------------
IS_Status IS_Init(IgnitionSystem *sys, IS_Config *cfg);
IS_Status IS_HandleError(IgnitionSystem *sys);
IS_Status IS_UpdateRPM(IgnitionSystem *sys);
IS_Status IS_CalculatePulseTime(IgnitionSystem *sys);
IS_Status IS_ControlCoil(IgnitionSystem *sys);
IS_Status IS_UART_Debug(IgnitionSystem *sys);

// --------------------------- Callbacks -------------------------------------
void IS_TIM_IC_CaptureCallback(IgnitionSystem *sys);
void IS_TIM_PeriodElapsedCallback(IgnitionSystem *sys);
void IS_EXTI_Callback(IgnitionSystem *sys, uint16_t GPIO_Pin, uint16_t expected_pin);
void process_ignition_event(IgnitionSystem *sys, uint8_t cylinder, uint32_t dwell_ms);
//void process_ignition_event(IgnitionSystem *sys, uint8_t cylinder, uint32_t dwell_ms);
#endif /* INC_IGNITION_SYSTEM_H_ */
