/*
 * submain.h
 *
 *  Created on: Jun 2, 2025
 *      Author: Shakil Tanvir
 */

#ifndef INC_SUBMAIN_H_
#define INC_SUBMAIN_H_

#include "main.h"


extern ADC_HandleTypeDef hadc1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim8;

extern UART_HandleTypeDef huart1;

void init(void);
void loop(void);
void update_coil_delay(void);

#endif /* INC_SUBMAIN_H_ */
