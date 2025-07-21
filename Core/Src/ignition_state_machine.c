/*
 * ignition_state_machine.c
 *
 *  Created on: May 9, 2025
 *      Author: Shakil tanvir
 * Description: State machine module for STM32 Ignition System
 * --------------------------------------------------------------------------*/

#include "ignition_state_machine.h"
#include "cylinder_scheduler.h"
// Initialize the state machine
void ISM_Init(IgnitionStateMachine *sm) {
    sm->current_state = ISM_IDLE;
    sm->index_detected = 0;
    sm->encoder_position = 0;
    sm->ignition_angle = 237;
    sm->dwell_pulses = 0;
    sm->pulse_counter = 0;
    sm->fired_count = 0;
}

void ISM_Update(IgnitionStateMachine *sm,
                uint32_t            encoder_pos,
                uint32_t            dwell_pulses,
                int                 angle_deg,
                uint8_t             cylinder)
{
    // 1) Compute positions in encoder ticks

	sm->encoder_position = encoder_pos;
	sm->dwell_pulses = dwell_pulses;
	sm->ignition_angle = angle_deg;

    uint32_t fire_pos   = (angle_deg * ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES) / CRANK_CYCLE_DEG;
    uint32_t charge_pos = (fire_pos + ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES - dwell_pulses)
                            % ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES;

    // 2) Detect new cylinder event (scheduler moved us to a new cylinder)
    if (cylinder != sm->current_cylinder) {
        sm->current_cylinder = cylinder;
        sm->coil_on          = false;
        sm->pulse_counter    = 0;
        sm->current_state    = ISM_COIL_CHARGING;
    }

    // 3) FSM
    switch (sm->current_state)
    {
      case ISM_COIL_CHARGING:
      {
        // entered the charge window?
        bool in_window =
           ((charge_pos <= fire_pos)
             ? (encoder_pos >= charge_pos && encoder_pos < fire_pos)
             : (encoder_pos >= charge_pos || encoder_pos < fire_pos));

        // start charging once
        if (in_window && !sm->coil_on) {
            sm->coil_on       = true;
            sm->pulse_counter = 0;
        }

        // if charging, count ticks
        if (sm->coil_on) {
            sm->pulse_counter++;

            // either enough dwell ticks or we've passed the fire angle
            bool passed_fire =
                   sm->pulse_counter >= dwell_pulses
                || encoder_pos == fire_pos
                || (encoder_pos >  fire_pos && fire_pos < charge_pos);

            if (passed_fire) {
                sm->current_state = ISM_IGNITION;
            }
        }
        break;
      }

      case ISM_IGNITION:
      {
        // spark: clear coil_on and move to DONE
        sm->coil_on       = false;
        sm->current_state = ISM_IDLE;  // or define ISM_DONE if clearer
        break;
      }

      default:
        // stay idle until next cylinder change
        sm->coil_on       = false;
        sm->current_state = ISM_IDLE;
        break;
    }
}

void ISM_IndexDetected(IgnitionStateMachine *sm) {
    sm->index_detected = 1;
}

// Check coil status
bool ISM_IsCoilOn(IgnitionStateMachine *sm) {
    return sm->coil_on;
}


//uint32_t ignition_position = 0;
// Transition the state machine
//void ISM_Update(IgnitionStateMachine *sm,
//                uint32_t            encoder_pos,
//                uint32_t            dwell_pulses,
//                int                 angle_deg,
//                uint8_t             cylinder)
//{
//		sm->encoder_position = encoder_pos;
//		sm->dwell_pulses = dwell_pulses;
//		sm->ignition_angle = angle_deg;
//    // Precompute once per call
//    const uint32_t fire   = (angle_deg * ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES) / CRANK_CYCLE_DEG;
//    // charge = fire – dwell, wrapped into [0…FULL)
//    const uint32_t charge = (fire >= dwell_pulses)
//                            ? (fire - dwell_pulses)
//                            : (ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES - (dwell_pulses - fire));
//
//    // New cylinder?  Arm the charger
//    if (cylinder != sm->current_cylinder) {
//        sm->current_cylinder = cylinder;
//        sm->coil_on          = false;
//        sm->pulse_counter    = 0;
//        sm->current_state    = ISM_COIL_CHARGING;
//    }
//
//    switch (sm->current_state)
//    {
//      case ISM_COIL_CHARGING:
//        if (!sm->coil_on) {
//            // only one compare to start charging
//            if (encoder_pos == charge) {
//                sm->coil_on       = true;
//                sm->pulse_counter = 0;
//            }
//        }
//        else {
//            // count pulses until we reach dwell
//            if (++sm->pulse_counter >= dwell_pulses) {
//                sm->coil_on       = false;
//                sm->current_state = ISM_IGNITION;
//            }
//        }
//        break;
//
//      case ISM_IGNITION:
//        // done: drop coil, go idle
//        sm->coil_on       = false;
//        sm->current_state = ISM_IDLE;
//        break;
//
//      default:
//        // remain idle
//        sm->coil_on = false;
//        break;
//    }
//}

//void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses, int angle_deg, uint8_t cylinder)
//{
//    sm->encoder_position = encoder_pos;
//    sm->dwell_pulses = 10;//dwell_pulses;
//    sm->ignition_angle = angle_deg;
//    sm->current_cylinder = cylinder;
//
//     ignition_position = (angle_deg * ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES) / CRANK_CYCLE_DEG;
//
//    switch (sm->current_state) {
//        case ISM_IDLE:
//            if (sm->index_detected) {
//                sm->index_detected = false;
//                sm->current_state = ISM_COIL_CHARGING;
//                sm->coil_on = false;
//            }
//            break;
//
//        case ISM_COIL_CHARGING:
//            if (encoder_pos >= (ignition_position - dwell_pulses)) {
//                sm->coil_on = true;
//                sm->current_state = ISM_IGNITION;
//            }
//            break;
//
//        case ISM_IGNITION:
//            if (encoder_pos >= ignition_position-1) {
//                sm->coil_on = false;
//                sm->fired_count++;
//
//                if (sm->fired_count >= TOTAL_CYLINDERS) {
//                	sm->fired_count = 0;  // start new cycle
//                    sm->current_state = ISM_IDLE;  // wait for next index
//
//                } else {
//                    sm->current_state = ISM_COIL_CHARGING;  // fire next cylinder
//                }
//            }
//            break;
//
//        default:
//            sm->coil_on = false;
//            sm->current_state = ISM_IDLE;
//            break;
//    }
//}

//void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses, int angle_deg, uint8_t cylinder) {
//    sm->encoder_position = encoder_pos;
////    sm->dwell_pulses = dwell_pulses;
//    sm->dwell_pulses = 3;
//    sm->ignition_angle = angle_deg;
//    sm->current_cylinder = cylinder;
//
//    uint32_t ignition_position = (angle_deg * ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES) / CRANK_CYCLE_DEG;
//
//    switch (sm->current_state) {
//        case ISM_IDLE:
//            if (sm->index_detected) {
//            	if(sm->fired_count>=TOTAL_CYLINDERS){
//            		sm->index_detected = 0;
//            		sm->fired_count = 0;
//            	}
//                sm->coil_on = false;
//                sm->current_state = ISM_COIL_CHARGING;
//            }
//            break;
//
//        case ISM_COIL_CHARGING:
//            if (encoder_pos >= (ignition_position - dwell_pulses)) {
//                sm->coil_on = true;
//                sm->current_state = ISM_IGNITION;
//            }
//            break;
//
//        case ISM_IGNITION:
//            if (encoder_pos >= ignition_position) {
//                sm->coil_on = false;
//                sm->fired_count++;
//                sm->current_state = ISM_IDLE;
//            }
//            break;
//
//        default:
//            sm->coil_on = false;
//            sm->current_state = ISM_IDLE;
//            break;
//    }
//}

//void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses, int angle, uint8_t cylinder) {
//    sm->encoder_position = encoder_pos;
//    sm->dwell_pulses = dwell_pulses;
//    sm->ignition_angle = angle;
//    sm->current_cylinder = cylinder;
//    uint32_t ignition_position = (angle * PULSES_PER_CYCLE) / CRANK_CYCLE_DEG; // from angle to pulse
//
//    switch (sm->current_state) {
//        case ISM_IDLE:
//            if (sm->index_detected) {
//                sm->current_state = ISM_COIL_CHARGING;
//                sm->index_detected = 0;
//            }
//            break;
//
//        case ISM_COIL_CHARGING:
//            if (sm->encoder_position >= (ignition_position - sm->dwell_pulses)) {
//                sm->coil_on = true;
//                sm->current_state = ISM_IGNITION;
//            }
//            break;
//
//        case ISM_IGNITION:
//            if (encoder_pos >= ignition_position) {
//                sm->coil_on = false;
//                sm->current_state = ISM_COIL_CHARGING;//ISM_COIL_CHARGING;  // continue cycle
//            }
//            break;
//
//        default:
//            sm->coil_on = false;
//            sm->current_state = ISM_IDLE;
//            break;
//    }
//}


//void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses,int angle) {
//    sm->encoder_position = encoder_pos;
//    sm->dwell_pulses = dwell_pulses;
//    sm->ignition_angle = angle;
//
//    switch (sm->current_state) {
//        case ISM_IDLE:
//            if (sm->index_detected) {
//                sm->current_state = ISM_COIL_CHARGING;
//                sm->index_detected = 0;
//            }
//            break;
//
//        case ISM_COIL_CHARGING:
//            if (sm->encoder_position >= (sm->ignition_angle - sm->dwell_pulses)) {
//                sm->current_state = ISM_IGNITION;
//                sm->coil_on = true;
//            }
//            break;
//
//        case ISM_IGNITION:
//            if (sm->encoder_position >= sm->ignition_angle) {
//                sm->current_state = ISM_COIL_CHARGING;//ISM_IDLE;
//                sm->coil_on = false;
//            }
//            break;
//
//        default:
//            sm->current_state = ISM_IDLE;
//            sm->coil_on = false;
//            break;
//    }
//}

// Handle index detection externally



