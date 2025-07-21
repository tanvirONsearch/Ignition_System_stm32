///*
// * ignition_cylinder.h
// *
// *  Created on: Jun 1, 2025
// *      Author: Shakil Tanvir
// * File: ignition_cylinder.h
// * Description: Header for multi-cylinder engine cylinder tracking (modular)
// * --------------------------------------------------------------------------*/
//
//#ifndef INC_IGNITION_CYLINDER_H_
//#define INC_IGNITION_CYLINDER_H_
//
//#include <stdint.h>
////#include "cylinder_scheduler.h"
//
//#ifndef CYLINDER_COUNT
//#define CYLINDER_COUNT 12
//#endif
//
//#define PULSES_PER_CYCLE (PULSES_PER_REVOLUTION / CYLINDER_COUNT) // 40
//
//
///**
// * @brief Reset the current cylinder to 0 (called on index pulse)
// */
//void CylinderTracker_Reset(void);
//
///**
// * @brief Get the current cylinder index (0 to CYLINDER_COUNT - 1)
// */
//uint8_t CylinderTracker_GetCurrent(void);
//
///**
// * @brief Advance to the next cylinder in sequence
// */
//void CylinderTracker_Advance(void);
//
///**
// * @brief Called when ignition occurs to advance firing order
// */
//void CylinderTracker_OnIgnition(void);
//
//#endif /* INC_IGNITION_CYLINDER_H_ */
//
//
