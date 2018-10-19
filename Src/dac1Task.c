/*
 * dac1Task.c
 *
 *  Created on: Oct 18, 2018
 *      Author: TBiberdorf
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <math.h>

extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;

#define TRIANGLE_WAVE_GENERATION (0)
#define SINE_WAVE_GENERATION (1)

/**
 * @note select which signal wave generation to create
 */
#define SIGNAL_GENERATION (SINE_WAVE_GENERATION)

static DAC_ChannelConfTypeDef sConfig;
#define DACx_CHANNEL (DAC_CHANNEL_1)
#define SIZE_SIN_WAVE_ARRAY (64-1)
#define USE_CONST_DATA (1)
#define USE_SRAM_DATA (2)
#define DMA_MEM_TO_USE (USE_SRAM_DATA)

__attribute__((section(".sin_data.tl_SinWaveData"))) uint16_t tl_SinWaveData[SIZE_SIN_WAVE_ARRAY+1];


/**
 * @brief  DAC Channel1 Triangle Configuration
 * @param  None
 * @retval None
 */
static void DAC_Ch1_TriangleConfig(void)
{
	/*##-1- Initialize the DAC peripheral ######################################*/
	if (HAL_DAC_Init(&hdac1) != HAL_OK)
	{
		/* DAC initialization Error */
		Error_Handler();
	}

	/*##-2- DAC channel2 Configuration #########################################*/
	sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DACx_CHANNEL) != HAL_OK)
	{
		/* Channel configuration Error */
		Error_Handler();
	}

	/*##-3- DAC channel2 Triangle Wave generation configuration ################*/
	if (HAL_DACEx_TriangleWaveGenerate(&hdac1, DACx_CHANNEL, DAC_TRIANGLEAMPLITUDE_4095) != HAL_OK)
	{
		/* Triangle wave generation Error */
		Error_Handler();
	}

	/*##-4- Enable DAC Channel1 ################################################*/
	if (HAL_DAC_Start(&hdac1, DACx_CHANNEL) != HAL_OK)
	{
		/* Start Error */
		Error_Handler();
	}

	/*##-5- Set DAC channel1 DHR12RD register ################################################*/
	if (HAL_DAC_SetValue(&hdac1, DACx_CHANNEL, DAC_ALIGN_12B_R, 0x00) != HAL_OK)
	{
		/* Setting value Error */
		Error_Handler();
	}
}

static void DAC_Ch1_SinWaveConfig(void)
{
#if (DMA_MEM_TO_USE == USE_CONST_DATA )
	if (HAL_DAC_Start_DMA(&hdac1, DACx_CHANNEL, (uint32_t*)gSineWaveArray,SIZE_SIN_WAVE_ARRAY,DAC_ALIGN_12B_R) != HAL_OK)
#elif (DMA_MEM_TO_USE == USE_SRAM_DATA )
	if (HAL_DAC_Start_DMA(&hdac1, DACx_CHANNEL, (uint32_t*)tl_SinWaveData,SIZE_SIN_WAVE_ARRAY,DAC_ALIGN_12B_R) != HAL_OK)
#else
#error "choose memory to DMA from"
#endif
//	if (HAL_DAC_Start_DMA(&hdac1, DACx_CHANNEL, (uint32_t*)sine_wave_array,32,DAC_ALIGN_12B_R) != HAL_OK)
//	if (HAL_DAC_Start_DMA(&hdac1, DACx_CHANNEL, (uint32_t*)sine_wave_array,SIZE_SIN_WAVE_ARRAY,DAC_ALIGN_12B_R) != HAL_OK)
	{
		/* Channel configuration Error */
		Error_Handler();
	}
}


static void SinWaveData()
{
#define OFFSET (50)
#define LIMIT_LEVEL (4095.0)/2
#define DEFAULT_DATA (1)
#define GENERATED_DATA (2)
#define GENERATE_SIN_DATA (GENERATED_DATA)
#define PI (3.141593)
	int idx;
	double value;

	for(idx=0;idx<SIZE_SIN_WAVE_ARRAY;idx++)
	{
		value = sin(PI/2 + PI*2*idx/SIZE_SIN_WAVE_ARRAY);
		value = (LIMIT_LEVEL/2) - (value*(LIMIT_LEVEL/2));// + OFFSET;
		tl_SinWaveData[idx] = (uint16_t)(value);
	}
}

void StartDac1Task(void const * argument)
{
//#define ADD_OFFSET (1)
	int idx;
	int AddOffset = 0;
	vTaskDelay(200);
	printf("StartDac1Task\n");

#if (SIGNAL_GENERATION == TRIANGLE_WAVE_GENERATION)
	uint8_t selectWaveform = 2;
#else
	uint8_t selectWaveform = 1;
#endif

	SinWaveData();

	/* Triangle Wave generator -------------------------------------------*/
	switch(selectWaveform)
	{
	case 1:
		DAC_Ch1_SinWaveConfig();
		break;
	case 2:
		DAC_Ch1_TriangleConfig();
		break;
	}

	HAL_TIM_Base_Start(&htim6);


	for(;;)
	{
		if( AddOffset )
		{
			for(idx=0;idx<SIZE_SIN_WAVE_ARRAY;idx++)
			{
#if (ADD_OFFSET)
				tl_SinWaveData[idx] += AddOffset;
#endif
			}
			AddOffset = 0;
		}

		osDelay(1);
	}
	/* USER CODE END StartDac1Task */
}
