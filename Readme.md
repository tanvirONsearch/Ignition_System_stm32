# STM32 Ignition System Firmware Documentation

## Overview

This embedded firmware controls an electronic ignition system using an STM32 microcontroller. It processes encoder signals to calculate engine RPM and dynamically adjusts ignition coil timing for precise spark control. The architecture is modular and follows STM32 HAL-style programming conventions.

## Features

* Real-time engine RPM calculation using a 60-PPR encoder
* Dwell time calculation based on RPM
* Precise coil-on and coil-off control using encoder timing
* Modular software design using configuration and state structs
* UART debugging support
* STM32 interrupt-driven design

## Module Architecture

### IgnitionSystem Structure

The core control logic is handled by the `IgnitionSystem` structure:

```c
typedef struct {
    IS_Config cfg;   // Hardware configuration
    IS_State state;  // Runtime state
} IgnitionSystem;
```

### Configuration (`IS_Config`)

Holds all hardware dependencies:

* Encoder Timer
* Input Capture Timer
* Time Base Timer
* UART for debugging
* GPIO port/pin for coil control
* Ignition angle (in pulses)

### State (`IS_State`)

Holds all live runtime parameters:

* RPM, frequency
* Encoder position & timestamps
* Coil state, index detected
* Pulse width timing and dwell tracking

### Status Enum (`IS_Status`)

Used for return values:

* `IS_OK`
* `IS_ERROR_NULL_PTR`
* `IS_ERROR_INVALID_STATE`

### Engine & Coil Enums

Used for runtime status:

```c
typedef enum { ENGINE_STOPPED, ENGINE_RUNNING } EngineState;
typedef enum { COIL_OFF, COIL_ON } CoilState;
```

## Functionality

### Initialization

`IS_Init()` sets up the ignition system and starts all timers:

* Encoder timer for position tracking
* Input capture timer for frequency measurement
* Time base timer for periodic RPM sampling

### RPM Calculation

`IS_UpdateRPM()` reads encoder positions and timestamps every 50ms to determine engine speed.

### Dwell Calculation

`IS_CalculatePulseTime()` determines how many pulses before ignition angle the coil should turn on based on RPM.

### Coil Control

`IS_ControlCoil()` uses the encoder position to turn the coil on/off precisely.

### UART Debug

`IS_UART_Debug()` prints RPM, dwell pulses, and dwell time in ms.

## Interrupt Support

* **`IS_TIM_IC_CaptureCallback()`** — Measures time between pulses for frequency
* **`IS_TIM_PeriodElapsedCallback()`** — Tracks overflows in capture timing
* **`IS_EXTI_Callback()`** — Resets encoder counter at index pulse (e.g., TDC)

## Assumptions

* Encoder is mounted on camshaft (2 pulses per engine rev)
* 60 PPR encoder => 120 pulses per revolution
* TIM4 is clocked at 84 MHz

## Integration Example

In `main.c`:

```c
IS_Config config = {
    .encoder_timer = &htim1,
    .input_capture_timer = &htim4,
    .time_base_timer = &htim5,
    .uart = &huart1,
    .coil_gpio_port = GPIOB,
    .coil_gpio_pin = GPIO_PIN_6,
    .ignition_angle = 237 // e.g. 3° BTDC
};
IS_Init(&ignition, &config);
```

## Dependencies

* STM32 HAL drivers
* Configured TIM1, TIM4, TIM5
* EXTI for index detection
* UART1 for debugging

## License

@creativefactory reserves the right.

---

For diagrams, flowcharts, or register-level mapping, contact the documentation owner or engineering lead.
