/*
 * ignition_state_machine.h
 *
 *  Created on: May 9, 2025
 *      Author: Shakil Tanvir
 */

#ifndef INC_IGNITION_STATE_MACHINE_H_
#define INC_IGNITION_STATE_MACHINE_H_


#include <stdint.h>
#include <stdbool.h>
#include "config.h"
// Enum representing the state machine states
typedef enum {
    ISM_IDLE = 0,
    ISM_COIL_CHARGING,
    ISM_IGNITION
} ISM_State;

// Struct representing the state machine context
typedef struct {
    ISM_State current_state;     // Current state of the FSM
    uint8_t index_detected;      // Set externally upon index pulse
    uint32_t encoder_position;   // Current encoder position
    uint32_t ignition_angle;     // Angle at which ignition should occur
    uint32_t dwell_pulses;       // Pulse count for coil dwell
    uint8_t current_cylinder;    // track current cylinder
    bool coil_on;                // Coil output status
    uint16_t pulse_counter;
    uint16_t fired_count;
} IgnitionStateMachine;

// Public API
void ISM_Init(IgnitionStateMachine *sm);
void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses, int angle, uint8_t cylinder);
//void ISM_Update(IgnitionStateMachine *sm, uint32_t encoder_pos, uint32_t dwell_pulses,int angle);
void ISM_IndexDetected(IgnitionStateMachine *sm);
bool ISM_IsCoilOn(IgnitionStateMachine *sm);

#endif /* INC_IGNITION_STATE_MACHINE_H_ */
