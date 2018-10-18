/*
 * adc3Task.c
 *
 *  Created on: Oct 18, 2018
 *      Author: TBiberdorf
 */
#include "main.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"

void StartAdc3Task(void const * argument)
{
	vTaskDelay(200);
	printf("StartAdc3Task\n");
  /* USER CODE BEGIN StartAdc3Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAdc3Task */
}
