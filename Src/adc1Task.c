/*
 * adc3Task.c
 *
 *  Created on: Oct 18, 2018
 *      Author: TBiberdorf
 */
#include "main.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"

extern ADC_HandleTypeDef hadc1;

void StartAdc1Task(void const * argument)
{
	uint32_t adcValue;
	vTaskDelay(200);
	printf("StartAdc3Task\n");


	/* Infinite loop */
	for(;;)
	{
		HAL_ADC_Start(&hadc1);
		if( HAL_ADC_PollForConversion(&hadc1,5) == HAL_OK )
		{
			adcValue = HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Stop(&hadc1);

		printf("adc %d\n",adcValue);
		vTaskDelay(500);
	}
	/* USER CODE END StartAdc3Task */
}
