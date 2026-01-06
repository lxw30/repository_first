/***************************************************************************//**
*   @file    DEVICE_ADC_ad7124.h
*   @brief   AD7124 header file.
*   	         Devices AD7124-4, AD7124-8
*
********************************************************************************
**************************/

#ifndef __DEVICE_ADC_AD7124_H__
#define __DEVICE_ADC_AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stm32f4xx_hal.h"
#include <stdint.h>
//#include "delay.h"
//#include "sys.h"
#include "math.h"
//#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"



typedef struct ADC_AD7124{
	uint32_t Data_temp;
	float Data_float; 
	float rtd_value1;	
	float Vol1;
	float Vol2;   //Vol1是真空计采集电压，Vol2是备用通道采集电压，单位：V
	float pt_temp;     //pt_temp是冷端补偿温度,单位℃
	float t_temp;      //t_temp是T型热电偶的温度，单位℃
	double t_voltage;
	double t_voltage2;	
	void (*Configuration)(void);
	void (*Reset)(void);
	void (*Init)(void);

	void (*Voltage1_Read)(void);	//真空计
	void (*T_Temp_Read)(void);		//T型热电偶
	void (*Pt_Temp_Read)(void);		//冷端补偿完整
}ADCDevice, *PADCDevice;

extern ADCDevice AD7124;

/******************************************************************************/
/******************* AD7124 Constants *****************************************/
/******************************************************************************/


#define AD7124_8_CS_H          HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
#define AD7124_8_CS_L          HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define AD7124_8_SCLK_H        HAL_GPIO_WritePin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin, GPIO_PIN_SET)
#define AD7124_8_SCLK_L        HAL_GPIO_WritePin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin, GPIO_PIN_RESET)
#define AD7124_8_DIN_H         HAL_GPIO_WritePin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin, GPIO_PIN_SET)
#define AD7124_8_DIN_L         HAL_GPIO_WritePin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin, GPIO_PIN_RESET)
#define AD7124_8_DOUT          HAL_GPIO_ReadPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin)



/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
#if 0
void AD_IO_Configuration(void) ;
void AD7124_8_Reset(void);
void AD7124_8_INIT(void);

void Voltage1_Read(void);
void T_Temp_Read(void);
void Pt_Temp_Read(void);
#endif

#endif /* __DEVICE_ADC_AD7124_H__ */
