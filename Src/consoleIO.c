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

#define CLI_BUFFER_SIZE (30)

typedef struct ParseCli_s
{
	uint8_t cmd[CLI_BUFFER_SIZE];
	uint8_t idx;
}sParseCli_t;

static sParseCli_t tl_ParseCli;
static uint8_t tl_RxData[CLI_BUFFER_SIZE];
static uint8_t tl_TxData[CLI_BUFFER_SIZE];
static uint8_t tl_CliCmd[CLI_BUFFER_SIZE];

volatile static uint8_t tl_TxDataReady = 0;
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

//	HAL_UART_Transmit_IT(&huart2,(uint8_t *)tl_RxData,1);
}

/**
 * @details Start Console IO operations
 */
void StartConsoleIO()
{
	HAL_UART_Receive_IT(&huart2,(uint8_t*)tl_RxData,1);
	HAL_UART_Transmit_IT( &huart2,(uint8_t *)tl_TxData, strlen((char *)tl_TxData));
	tl_TxData[0] = '0';
	memset(&tl_ParseCli,0,sizeof(sParseCli_t));
}

/**
 * @details provide _write API to support printf calls
 */
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


/**
 * @details callback function called when the debug UART transmit port is idle
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	tl_TxDataReady = 1;
}

static uint8_t tl_CliRawData[30];
static uint8_t tl_CliParseStr[30];

/**
 * @details perform direction arrow input commands from user
 */
static uint8_t DirectionArrowPressed(uint8_t *Direction)
{
	const uint8_t arrowUp[]={27,91,65};
	const uint8_t arrowDown[]={27,91,66};
	const uint8_t arrowRight[]={27,91,67};
	const uint8_t arrowLeft[]={27,91,68};
	const uint8_t deleteArrow[]={0x08,0x08,0x20,0x20,0x08,0x08}; // backspace 2 chars
	const uint8_t deleteChar[]={0x08,0x20,0x08};

	if( (Direction[0] == arrowUp[0]) && (Direction[1] == arrowUp[1]) )
	{
		if(Direction[2] == arrowUp[2])
		{
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			Direction[2] = 0xff;
		}
		if(Direction[2] == arrowDown[2])
		{
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			Direction[2] = 0xff;
		}
		if(Direction[2] == arrowRight[2])
		{
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			Direction[2] = 0xff;
		}
		if(Direction[2] == arrowLeft[2])
		{
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			Direction[2] = 0x08;
		}
	}
	return Direction[2];
}

/**
 * entry for Console Task.
 */
void StartConsoleTask(void const * argument)
{
	vTaskDelay(100);
	const uint8_t deleteChar[]={0x08,0x20,0x08,0x0};
	uint8_t direction[3]={0,0,0};
	int32_t status;

	printf("start Console Task\n");

	/* Infinite loop */
	for(;;)
	{
		xQueueReceive(queueCLIHandle,tl_CliRawData,portMAX_DELAY );
		direction[0] = direction[1];
		direction[1] = direction[2];
		direction[2] = tl_CliRawData[0];

		tl_CliRawData[0] = DirectionArrowPressed(direction);

		switch(*tl_CliRawData)
		{
		case 0xff:
			break;
		case 0x0A:
			break;
		case 0x0D:
			vTaskDelay(25);
			printf("\n%s\n",tl_ParseCli.cmd);
			//			HAL_UART_Transmit_IT(&huart2,tl_ParseCli.cmd,tl_ParseCli.idx);
			//			vTaskDelay(200);
			//			HAL_UART_Transmit_IT(&huart2,crlf,2);
			tl_ParseCli.idx = 0;
			tl_ParseCli.cmd[0] = 0;
			break;
		case 0x08:
			tl_ParseCli.cmd[--tl_ParseCli.idx] = 0x0;
			printf("%s",deleteChar);
//			HAL_UART_Transmit_IT(&huart2,deleteChar,3);
			break;
		default:
			tl_ParseCli.cmd[tl_ParseCli.idx++] = *tl_CliRawData;
			tl_ParseCli.cmd[tl_ParseCli.idx] = 0;
//			printf("%s",tl_CliRawData[0]);
			HAL_UART_Transmit_IT(&huart2,tl_CliRawData,1);
//			vTaskDelay(25);
		}
//		HAL_UART_Transmit_IT(&huart2,(uint8_t *)tl_CliData,1);

		osDelay(1);
	}

}

