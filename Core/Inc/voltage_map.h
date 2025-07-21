/*
 * voltage_map.h
 *
 *  Created on: May 15, 2025
 *      Author: Shakil Tanvir
 * Description: Header for voltage to ignition timing and dwell time lookup
 * --------------------------------------------------------------------------*/

#ifndef INC_VOLTAGE_MAP_H_
#define INC_VOLTAGE_MAP_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "config.h"

#define TABLE_SIZE 6
#define DWELL_TABLE_SIZE 8

// ------------------- Ignition Advance Map ----------------------
typedef struct {
    float voltage;                   // Input voltage (e.g., battery voltage)
    int ignition_advance_pulses;    // Corresponding ignition advance (in pulses)
} VoltageIgnitionMap;

// ------------------- Dwell Time Map ----------------------------
typedef struct {
    float voltage;                  // Input voltage
    float dwell_ms;                // Corresponding dwell time in milliseconds
} VoltageDwellMap;

// Lookup ignition timing based on current battery voltage
int Lookup_IgnitionAngle(float voltage);

// Lookup coil dwell time in ms based on battery voltage
uint32_t Lookup_DwellTime(float voltage);

// Reads the battery voltage using the specified ADC instance
// Returns voltage in volts, or -1.0f on error
float Read_BatteryVoltage(ADC_HandleTypeDef *hadc);

#endif /* INC_VOLTAGE_MAP_H_ */
