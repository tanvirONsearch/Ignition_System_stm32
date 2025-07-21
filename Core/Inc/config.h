/*
 * config.h
 *
 *  Created on: Jun 29, 2025
 *      Author: Tanvir Rahman Sahed @ CFL
 * 		Description: Macros and defines configurations
 */
#pragma once
#define ONE_REV_DEG 360
/* from cylinder_scheduler.h ENGINE SPECS----------------- */
#define TOTAL_CYLINDERS 8
#define CRANK_CYCLE_DEG 720    // 4-stroke engine crank cycle in degrees

/* ——— Coil port/pin setup by cylinder count ——— */
#if   TOTAL_CYLINDERS == 12

  #define COIL_PORT_LIST  { \
    GPIOA, GPIOA, GPIOC, GPIOD, GPIOA, GPIOC, \
    GPIOC, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD  \
  }
  #define COIL_PIN_LIST   { \
    GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_3,  GPIO_PIN_15, GPIO_PIN_12, \
    GPIO_PIN_11, GPIO_PIN_0,  GPIO_PIN_1,  GPIO_PIN_2,  GPIO_PIN_5,  GPIO_PIN_4  \
  }

#define IGNITION_TABLE { \
    {  60,  1 }, { 120,  6 }, { 180,  8 }, { 240,  3 },\
    { 300,  10 }, { 360,  5 }, { 420,  7 }, { 480,  2 },\
    { 540,  11 }, { 600, 4 }, { 660, 9 }, { 720, 12 }\
	}

#elif TOTAL_CYLINDERS == 8

  /* First eight coils of the 12-cyl mapping */
  #define COIL_PORT_LIST  { \
    GPIOA, GPIOA, GPIOC, GPIOD, GPIOA, GPIOC, \
    GPIOC, GPIOD \
  }
  #define COIL_PIN_LIST   { \
    GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_3, \
    GPIO_PIN_15, GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_0 \
  }
 #define IGNITION_TABLE  { \
    {  90, 1 }, { 180, 2 }, { 270, 3 }, { 360, 4 },\
    { 450, 5 }, { 540, 6 }, { 630, 7 }, { 720, 8 }\
	}

#elif TOTAL_CYLINDERS == 6

  #define COIL_PORT_LIST  {GPIOA, GPIOA, GPIOC, GPIOD, GPIOA, GPIOC}
  #define COIL_PIN_LIST   {GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_3,  GPIO_PIN_15, GPIO_PIN_12}

  #define IGNITION_TABLE   {{ 120, 1 }, { 240, 2 }, { 360, 3 },{ 480, 4 }, { 600, 5 }, { 720, 6 }}

#elif TOTAL_CYLINDERS == 4

  #define COIL_PORT_LIST  { GPIOA, GPIOA, GPIOC, GPIOD }
  #define COIL_PIN_LIST   { GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_3 }
  #define IGNITION_TABLE   {{ 180, 1 }, { 360, 2 },{ 540, 3 }, { 720, 4 }}

#elif TOTAL_CYLINDERS == 3

  #define COIL_PORT_LIST  { GPIOA, GPIOA, GPIOC }
  #define COIL_PIN_LIST   { GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10 }
  #define IGNITION_TABLE  {{ 240, 1 }, { 480, 2 }, { 720, 3 }}
#else
  #error "Unsupported TOTAL_CYLINDERS – must be 3, 4, 6, 8 or 12"
#endif





/*ENCODER SPECS---------------------------------------------*/


#define ENC_MODE_ONE_CH_ONE_EDGE  1  ///< 1 channel, single edge (×1)
#define ENC_MODE_ONE_CH_TWO_EDGE  2  ///< 1 channel, both edges  (×2)
#define ENC_MODE_TWO_CH_ONE_EDGE  2  ///< 2 channels, single edge (×2)
#define ENC_MODE_TWO_CH_TWO_EDGE  4   ///< 2 channels, both edges  (×4)

#define ENCODER_SPEC_PPR 60//


//#define PULSES_PER_CYCLE 60//480//240//60//120   // Total encoder pulses per engine cycle (2 crank revolutions)
//#define ENCODER_PULSE_PER_REV 4*PULSES_PER_CYCLE
//#define ENCODER_WRAP_THRESHOLD 239   // Half cycle in pulses (e.g., 360°)
//#define ENCODER_MAX_PULSES     478   // Full cycle (e.g., 720°)

#define ENCODER_TIMER_PPR ((ENC_MODE_TWO_CH_TWO_EDGE*ENCODER_SPEC_PPR)-1)//239//((ENC_MODE_TWO_CH_TWO_EDGE * ENCODER_SPEC_PPR)-1)
#define ENCODER_TIMER_ENGINE_HALF_CYCLE_PULSES  239//(ENCODER_TIMER_PPR*(CRANK_CYCLE_DEG/2)/ONE_REV_DEG) // Half cycle in pulses (e.g., 360°)
#define ENCODER_TIMER_ENGINE_FULL_CYCLE_PULSES  478//ENCODER_TIMER_ENGINE_HALF_CYCLE_PULSES*2   // Full cycle (e.g., 720°)
#define ENCODER_MIN_DELTA 1       // Minimum change in encoder to detect motion



/*from ignition_system.c ----------------------*/
#define RPM_SAMPLE_INTERVAL 5000 // 5 ms //50ms previos
#define CONF_MIN_RPM 200


/*ignition_system.h---------------------------*/
//#define PULSE_PER_REVOLUTION 120 //used @ ignition_system.c ->110
#define MIN_DURATION_PER_PULSE_AT_200_RPM 4.5f // ms

/*voltage_map.c---------------------------*/
// Example configuration — adjust according to your hardware
#define V_MIN 6.0
#define V_MAX 20.0f
#define ADC_RESOLUTION        4095.0f   // 12-bit ADC //used at line ->82
#define VREF                  3.3f      // Reference voltage in volts //used at line ->82
#define VOLTAGE_DIVIDER_GAIN  1.f//(11.0f)   // (R1 + R2) / R2 if R1=100k, R2=10k //used at line ->82


