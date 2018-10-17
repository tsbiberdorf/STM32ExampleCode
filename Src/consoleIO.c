/*
 * consoleIO.c
 *
 *  Created on: Oct 17, 2018
 *      Author: TBiberdorf
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>

static uint8_t tl_RxData[30];
static uint8_t tl_TxData[30];
static uint8_t tl_DataReady = 0;

extern UART_HandleTypeDef huart2;

void consoleInit()
{
	  __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE); // RX interrupt enable
	  __HAL_UART_ENABLE_IT(&huart2,UART_IT_TC); // TX interrupt enable
		strcpy(tl_TxData,"UserInterface\r\n");
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */
//  HAL_UART_Transmit(&huart2,(uint8_t *)tl_RxData,strlen((char *)tl_RxData),10);
  HAL_UART_Receive_IT(&huart2,(uint8_t*)tl_RxData,1);

  if( tl_TxData[0] == '9')
  {
	  tl_TxData[0] = '0';
  }
  else
  {
	  tl_TxData[0]++;
  }
  tl_TxData[1] = 0;
  tl_DataReady = 1;
  HAL_UART_Transmit_IT(&huart2,(uint8_t *)tl_RxData,1);
}

void StartConsoleIO()
{
	  HAL_UART_Receive_IT(&huart2,(uint8_t*)tl_RxData,1);
	  HAL_UART_Transmit_IT( &huart2,(uint8_t *)tl_TxData, strlen((char *)tl_TxData));
	//  HAL_UART_Transmit(&huart2,(uint8_t *)tl_TxData,strlen((char *)tl_TxData),10);
	  tl_TxData[0] = '0';
	//  HAL_UART_Receive_DMA(&huart2,(uint8_t *)tl_RxData,2);

}
