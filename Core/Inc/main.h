/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENCODER_Z_IDX_Pin GPIO_PIN_2
#define ENCODER_Z_IDX_GPIO_Port GPIOA
#define ENCODER_Z_IDX_EXTI_IRQn EXTI2_IRQn
#define CYL11_Pin GPIO_PIN_10
#define CYL11_GPIO_Port GPIOB
#define CYL12_Pin GPIO_PIN_11
#define CYL12_GPIO_Port GPIOB
#define CYL_1_Pin GPIO_PIN_11
#define CYL_1_GPIO_Port GPIOA
#define CYL_2_Pin GPIO_PIN_12
#define CYL_2_GPIO_Port GPIOA
#define CYL_5_Pin GPIO_PIN_15
#define CYL_5_GPIO_Port GPIOA
#define CYL_3_Pin GPIO_PIN_10
#define CYL_3_GPIO_Port GPIOC
#define CYL_7_Pin GPIO_PIN_11
#define CYL_7_GPIO_Port GPIOC
#define CYL_6_Pin GPIO_PIN_12
#define CYL_6_GPIO_Port GPIOC
#define CYL_8_Pin GPIO_PIN_0
#define CYL_8_GPIO_Port GPIOD
#define CYL_9_Pin GPIO_PIN_1
#define CYL_9_GPIO_Port GPIOD
#define CYL_10_Pin GPIO_PIN_2
#define CYL_10_GPIO_Port GPIOD
#define CYL_4_Pin GPIO_PIN_3
#define CYL_4_GPIO_Port GPIOD
#define CYL_12_Pin GPIO_PIN_4
#define CYL_12_GPIO_Port GPIOD
#define CYL_11_Pin GPIO_PIN_5
#define CYL_11_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
