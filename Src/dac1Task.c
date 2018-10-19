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
#define SIZE_SIN_WAVE_ARRAY (32)
#define USE_CONST_DATA (1)
#define USE_SRAM_DATA (2)
#define DMA_MEM_TO_USE (USE_SRAM_DATA)

const uint16_t sine_wave_array[SIZE_SIN_WAVE_ARRAY] = {2047, 1648, 1264, 910, 600,  345,
		156, 39,  0,  39,  156,  345,
		600, 910, 1264, 1648, 2048, 2447,
		2831, 3185, 3495, 3750, 3939, 4056,
		4095, 4056, 3939, 3750, 3495, 3185,
		2831, 2447};

const uint16_t gSineWaveArray[SIZE_SIN_WAVE_ARRAY]= {2047, 1648, 1264, 910, 600,  345,
		156, 39,  0,  39,  156,  345,
		600, 910, 1264, 1648, 2048, 2447,
		2831, 3185, 3495, 3750, 3939, 4056,
		4095, 4056, 3939, 3750, 3495, 3185,
		2831, 2447};
__attribute__((section(".sin_data.tl_SinWaveData"))) uint16_t tl_SinWaveData[SIZE_SIN_WAVE_ARRAY];


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
#define LIMIT_LEVEL (4095)
#define DEFAULT_DATA (1)
#define GENERATED_DATA (2)
#define GENERATE_SIN_DATA (GENERATED_DATA)
#define PI (3.141593)
	int idx;
	double value;

#if (GENERATE_SIN_DATA == GENERATED_DATA)
	for(idx=0;idx<SIZE_SIN_WAVE_ARRAY;idx++)
	{
		value = sin(PI/2 + PI*2*idx/SIZE_SIN_WAVE_ARRAY);
		value = (LIMIT_LEVEL/2) - (value*(LIMIT_LEVEL/2));
		tl_SinWaveData[idx] = (int16_t)value;
	}
	// generate data for ramp wave
//	for(idx=0;idx<SIZE_SIN_WAVE_ARRAY;idx++)
//	{
//		value = (double)4095*((double)idx/SIZE_SIN_WAVE_ARRAY);
//		tl_SinWaveData[idx] = (int16_t)value;
//	}
#endif
#if 1
#if (GENERATE_SIN_DATA == DEFAULT_DATA)
	tl_SinWaveData[0] = 2047;
	tl_SinWaveData[1] = 1648;
	tl_SinWaveData[2] = 1264;
	tl_SinWaveData[3] = 910;
	tl_SinWaveData[4] = 600;
	tl_SinWaveData[5] = 345;
	tl_SinWaveData[6] = 156;
	tl_SinWaveData[7] = 39;
	tl_SinWaveData[8] = 0;
	tl_SinWaveData[9] = 39;
	tl_SinWaveData[10] = 156;
	tl_SinWaveData[11] = 345;
	tl_SinWaveData[12] = 600;
	tl_SinWaveData[13] = 910;
	tl_SinWaveData[14] = 1264;
	tl_SinWaveData[15] = 1648;
	tl_SinWaveData[16] = 2048;
	tl_SinWaveData[17] = 2447;
	tl_SinWaveData[18] = 2831;
	tl_SinWaveData[19] = 3185;
	tl_SinWaveData[20] = 3495;
	tl_SinWaveData[21] = 3750;
	tl_SinWaveData[22] = 3939;
	tl_SinWaveData[23] = 4056;
	tl_SinWaveData[24] = 4095;
	tl_SinWaveData[25] = 4056;
	tl_SinWaveData[26] = 3939;
	tl_SinWaveData[27] = 3750;
	tl_SinWaveData[28] = 3495;
	tl_SinWaveData[29] = 3185;
	tl_SinWaveData[30] = 2831;
	tl_SinWaveData[31] = 2447;
#endif
#endif
}

void StartDac1Task(void const * argument)
{

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
		osDelay(1);
	}
	/* USER CODE END StartDac1Task */
}
