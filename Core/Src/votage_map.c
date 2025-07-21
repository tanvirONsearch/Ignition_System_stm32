/* --------------------------------------------------------------------------
 * File: voltage_map.c
 * Description: Lookup and interpolation for ignition timing and dwell time
 * --------------------------------------------------------------------------*/

#include "voltage_map.h"
#include <stddef.h>

// Example configuration — adjust according to your hardware
//#define ADC_RESOLUTION        4095.0f   // 12-bit ADC
//#define VREF                  3.3f      // Reference voltage in volts
//#define VOLTAGE_DIVIDER_GAIN  (11.0f)   // (R1 + R2) / R2 if R1=100k, R2=10k

// ---------------- Ignition Advance Map ----------------------
static const VoltageIgnitionMap voltage_map[TABLE_SIZE] = {
    {11.0f, 234},
    {11.5f, 235},
    {12.0f, 236},
    {12.5f, 237},
    {13.0f, 238},
    {13.5f, 239}
};

int Lookup_IgnitionAngle(float voltage) {
    if (voltage <= voltage_map[0].voltage)
        return voltage_map[0].ignition_advance_pulses;

    if (voltage >= voltage_map[TABLE_SIZE - 1].voltage)
        return voltage_map[TABLE_SIZE - 1].ignition_advance_pulses;

    for (int i = 0; i < TABLE_SIZE - 1; ++i) {
        float v1 = voltage_map[i].voltage;
        float v2 = voltage_map[i + 1].voltage;
        if (voltage >= v1 && voltage <= v2) {
            int a1 = voltage_map[i].ignition_advance_pulses;
            int a2 = voltage_map[i + 1].ignition_advance_pulses;
            float t = (voltage - v1) / (v2 - v1);
            return (int)(a1 + t * (a2 - a1));
        }
    }
    return voltage_map[0].ignition_advance_pulses; // fallback
}

// ---------------- Dwell Time Map --------------------------
static const VoltageDwellMap dwell_table[DWELL_TABLE_SIZE] = {
    {6.0f, 30.0},
    {8.0f, 24.0},
    {10.0f, 18.0},
    {12.0f, 10.0},
    {14.0f, 5.0},
    {16.0f, 2.0},
    {18.0f, 1.2},
    {20.0f, 1.0}
};

uint32_t Lookup_DwellTime(float voltage) {
    if (voltage <= dwell_table[0].voltage)
        return dwell_table[0].dwell_ms;

    if (voltage >= dwell_table[DWELL_TABLE_SIZE - 1].voltage)
        return dwell_table[DWELL_TABLE_SIZE - 1].dwell_ms;

    for (int i = 0; i < DWELL_TABLE_SIZE - 1; ++i) {
        float v1 = dwell_table[i].voltage;
        float v2 = dwell_table[i + 1].voltage;
        float d1 = dwell_table[i].dwell_ms;
        float d2 = dwell_table[i + 1].dwell_ms;

        if (voltage >= v1 && voltage <= v2) {
            float t = (voltage - v1) / (v2 - v1);
            return d1 + t * (d2 - d1);
        }
    }
    return dwell_table[0].dwell_ms; // fallback
}

// ---------------- ADC Voltage Reader ----------------------
float Read_BatteryVoltage(ADC_HandleTypeDef *hadc) {
    HAL_ADC_Start(hadc);
    if (HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY) == HAL_OK) {
        uint32_t raw = HAL_ADC_GetValue(hadc);
        float voltage = ((float)raw / ADC_RESOLUTION) * VREF * VOLTAGE_DIVIDER_GAIN;
        return voltage;
    }
    return -1.0f; // error fallback
}
