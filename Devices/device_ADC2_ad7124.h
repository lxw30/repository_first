/***************************************************************************//**
*   @file    device_ADC2_ad7124.h
*   @brief   AD7124_2 header file.
*   	         Devices AD7124-4, AD7124-8
*
********************************************************************************
**************************/

#ifndef __DEVICE_ADC2_AD7124_H__
#define __DEVICE_ADC2_AD7124_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
//#include "delay.h"
//#include "sys.h"
#include "math.h"
//#include "timer.h"
#include "device_ADC_ad7124.h"
#include "main.h"





/******************************************************************************/
/******************* AD7124_2 Constants *****************************************/
/******************************************************************************/

#define AD7124_8_CS2_H          HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET)
#define AD7124_8_CS2_L          HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET)
#define AD7124_8_SCLK2_H        HAL_GPIO_WritePin(SPI3_SCK_GPIO_Port, SPI3_SCK_Pin, GPIO_PIN_SET)
#define AD7124_8_SCLK2_L        HAL_GPIO_WritePin(SPI3_SCK_GPIO_Port, SPI3_SCK_Pin, GPIO_PIN_RESET)
#define AD7124_8_DIN2_H         HAL_GPIO_WritePin(SPI3_MOSI_GPIO_Port, SPI3_MOSI_Pin, GPIO_PIN_SET)
#define AD7124_8_DIN2_L         HAL_GPIO_WritePin(SPI3_MOSI_GPIO_Port, SPI3_MOSI_Pin, GPIO_PIN_RESET)
#define AD7124_8_DOUT2          HAL_GPIO_ReadPin(SPI3_MISO_GPIO_Port, SPI3_MISO_Pin) 




/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/


#endif /* __DEVICE_ADC2_AD7124_H__ */

