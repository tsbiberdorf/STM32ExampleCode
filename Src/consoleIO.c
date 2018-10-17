/*
 * consoleIO.c
 *
 *  Created on: Oct 17, 2018
 *      Author: TBiberdorf
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

static uint8_t tl_RxData[30];
static uint8_t tl_TxData[30];
static uint8_t tl_DataReady = 0;
static uint8_t tl_TxDataReady = 0;
extern UART_HandleTypeDef huart2;
extern osMessageQId queueCLIHandle;

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
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	HAL_UART_Receive_IT(&huart2,(uint8_t*)tl_RxData,1);

    xQueueSendFromISR( queueCLIHandle, &tl_RxData, &xHigherPriorityTaskWoken );

//	tl_DataReady = 1;
//	HAL_UART_Transmit_IT(&huart2,(uint8_t *)tl_RxData,1);
}

void StartConsoleIO()
{
	HAL_UART_Receive_IT(&huart2,(uint8_t*)tl_RxData,1);
	HAL_UART_Transmit_IT( &huart2,(uint8_t *)tl_TxData, strlen((char *)tl_TxData));
	tl_TxData[0] = '0';

}

int _write(int file, char *data, int len)
{

	if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
	{
		errno = EBADF;
		return -1;
	}

	while( !tl_TxDataReady)
	{
		HAL_Delay(1);
	}

	if(tl_TxDataReady)
	{
		tl_TxDataReady = 0;
		HAL_UART_Transmit_IT( &huart2,(uint8_t *)data, len);
	}
	//    for (bytes_written = 0; bytes_written < len; bytes_written++)
	//    {
	//        UART_TxBlocking(*data);
	//        data++;
	//    }

	return len;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	tl_TxDataReady = 1;
}

static uint8_t tl_CliData[30];
void StartConsoleTask(void const * argument)
{
	/* USER CODE BEGIN StartConsoleTask */
	vTaskDelay(100);

	printf("start Console Task\n");
	/* Infinite loop */
	for(;;)
	{
		xQueueReceive(queueCLIHandle,tl_CliData,portMAX_DELAY );
		HAL_UART_Transmit_IT(&huart2,(uint8_t *)tl_CliData,1);

		osDelay(1);
	}
	/* USER CODE END StartConsoleTask */
}

