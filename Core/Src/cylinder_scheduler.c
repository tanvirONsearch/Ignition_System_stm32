/*
 * cylinder_scheduler.c
 *
 *  Created on: Jun 2, 2025
 *      Author: Shakil Tanvir
 * Description: Maps crankshaft angle to cylinder ignition event
 * --------------------------------------------------------------------------*/

#include "cylinder_scheduler.h"
#include<stdbool.h>

// Firing order (based on 4-stroke 12-cylinder typical V12 firing order)
// This is just an example: modify if your firing order differs
static const IgnitionEvent ignition_table[TOTAL_CYLINDERS] = IGNITION_TABLE;
//{
//    {  60,  1 },
//    { 120,  6 },
//    { 180,  8 },
//    { 240,  3 },
//    { 300, 10 },
//    { 360,  5 },
//    { 420,  7 },
//    { 480,  2 },
//    { 540, 11 },
//    { 600,  4 },
//    { 660,  9 },
//    { 720, 12 }
//};

// Returns current cylinder to be fired based on encoder position (crankshaft angle)
uint8_t CylinderScheduler_GetCurrent(uint32_t encoder_pos) {

    //uint16_t angle = (encoder_pos * CRANK_CYCLE_DEG) / PULSES_PER_CYCLE;
	uint16_t angle = (encoder_pos * CRANK_CYCLE_DEG) / ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES;

    for (int i = 0; i < TOTAL_CYLINDERS; ++i) {
        if (angle < ignition_table[i].angle_deg) {
            return ignition_table[i].cylinder;
        }
    }
    return ignition_table[0].cylinder; // fallback to first if not matched
}

// Returns ignition angle for given cylinder (in degrees)
int CylinderScheduler_GetIgnitionAngle(uint8_t cylinder) {
    for (int i = 0; i < TOTAL_CYLINDERS; ++i) {
        if (ignition_table[i].cylinder == cylinder) {
            return ignition_table[i].angle_deg;
        }
    }
    return ignition_table[0].angle_deg; // fallback
}

// Allow dynamic override of ignition angle (runtime tuning)
void CylinderScheduler_SetIgnitionAngle(uint8_t cylinder, int angle) {
    for (int i = 0; i < TOTAL_CYLINDERS; ++i) {
        if (ignition_table[i].cylinder == cylinder) {
            ((IgnitionEvent *)ignition_table)[i].angle_deg = angle; // cast to bypass const
            break;
        }
    }
}


