/*
 * cylinder_scheduler.h
 *
 *  Created on: Jun 2, 2025
 *      Author: Shakil Tanvir
 * 		Description: Header for crank angle to cylinder ignition mapping
 */

#ifndef INC_CYLINDER_SCHEDULER_H_
#define INC_CYLINDER_SCHEDULER_H_



#include <stdint.h>
#include "config.h"

//#define TOTAL_CYLINDERS 12
//#define PULSES_PER_CYCLE 60//480//240//60//120   // Total encoder pulses per engine cycle (2 crank revolutions)
//#define CRANK_CYCLE_DEG 720    // 4-stroke engine crank cycle in degrees
//#define ENCODER_PULSE_PER_REV 4*PULSES_PER_CYCLE
//#define ENCODER_WRAP_THRESHOLD 239   // Half cycle in pulses (e.g., 360°)
//#define ENCODER_MAX_PULSES     478   // Full cycle (e.g., 720°)
//#define ENGINE_RPM_TO_MS(rpm) ((120000UL) / (rpm))

// Struct representing ignition event
typedef struct {
    uint16_t angle_deg;    // Crank angle (in degrees) where ignition occurs
    uint8_t cylinder;      // Cylinder number (1-based)
} IgnitionEvent;

/**
 * @brief Returns the current cylinder that should be fired based on encoder position
 * @param[in] encoder_pos Encoder pulse count (0 to PULSES_PER_CYCLE)
 * @return Cylinder number to ignite (1–12)
 */
uint8_t CylinderScheduler_GetCurrent(uint32_t encoder_pos);
int CylinderScheduler_GetIgnitionAngle(uint8_t cylinder);


#endif /* INC_CYLINDER_SCHEDULER_H_ */
