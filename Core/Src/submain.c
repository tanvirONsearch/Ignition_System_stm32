/*
 * submain.c
 *
 *  Created on: Jun 2, 2025
 *      Author: Shakil Tanvir
 */

#include "submain.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "ignition_system.h"
#include "print_active.h"
#include "config.h"

// Ignition system instance
IgnitionSystem ignition;

void init(void) {

	//   Initialize ignition system config
	IS_Config config = { .encoder_timer = &htim1, .input_capture_timer = &htim4,
			.time_base_timer = &htim5, .uart = &huart1, .battery_adc = &hadc1,

			.coil_ports = COIL_PORT_LIST,
//	{
//				GPIOA, GPIOA, GPIOC, GPIOD, GPIOA, GPIOC,
//			    GPIOC, GPIOD, GPIOD,
//			    GPIOD, GPIOD, GPIOD
//			},
			.coil_pins = COIL_PIN_LIST,

//			{
//			    GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_15, GPIO_PIN_12,
//			    GPIO_PIN_11, GPIO_PIN_0, GPIO_PIN_1,
//			    GPIO_PIN_2, GPIO_PIN_5, GPIO_PIN_4
//			},

			.ignition_angle = 237,//717,//237, // 240 - 3 advance// Ignition timing of cylinder 1(9 degree top death corner, 3 degree per pulse)

			.coil_dwell = 5 };
	// Initialize ignition system and start timers
	if (IS_Init(&ignition, &config) != IS_OK) {
		Error_Handler();
	}

}
//void init(void) {
//
//	//   Initialize ignition system config
//	IS_Config config = { .encoder_timer = &htim1, .input_capture_timer = &htim4,
//			.time_base_timer = &htim5, .uart = &huart1, .battery_adc = &hadc1,
//
//			.coil_ports = {
//			    GPIOD, GPIOD, GPIOD                         // PA11, PA12, PA15
//			},
//			.coil_pins = {
//			    GPIO_PIN_2, GPIO_PIN_5, GPIO_PIN_4                                    // A11, A12, A15
//			},
//
//			.ignition_angle = 237,//717,//237, // 240 - 3 advance// Ignition timing of cylinder 1(9 degree top death corner, 3 degree per pulse)
//			.coil_dwell = 5 };
//	// Initialize ignition system and start timers
//	if (IS_Init(&ignition, &config) != IS_OK) {
//		Error_Handler();
//	}
//
//}
IS_Status status1;
IS_Status status2;
IS_Status status3;
void loop(void) {

	status1 = IS_UpdateRPM(&ignition);
	status2 = IS_CalculatePulseTime(&ignition);
	status3 = IS_ControlCoil(&ignition);
//	update_coil_delay();
//	IS_UART_Debug(&ignition); // Optional debug output
//	printf("hellooo \n");
//	HAL_Delay(100);

}











//uint8_t intcount=0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//intcount++;
    IS_EXTI_Callback(&ignition, GPIO_Pin, GPIO_PIN_2); // Assuming EXTI on pin 2

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        IS_TIM_IC_CaptureCallback(&ignition);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        IS_TIM_PeriodElapsedCallback(&ignition);
    }
}



