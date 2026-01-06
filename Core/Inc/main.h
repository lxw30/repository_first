/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "semphr.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SemaphoreHandle_t xMutex;
extern SemaphoreHandle_t xMutex2;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ID1_Pin GPIO_PIN_2
#define ID1_GPIO_Port GPIOE
#define ID2_Pin GPIO_PIN_3
#define ID2_GPIO_Port GPIOE
#define ID3_Pin GPIO_PIN_4
#define ID3_GPIO_Port GPIOE
#define ID4_Pin GPIO_PIN_5
#define ID4_GPIO_Port GPIOE
#define ID5_Pin GPIO_PIN_6
#define ID5_GPIO_Port GPIOE
#define ID6_Pin GPIO_PIN_13
#define ID6_GPIO_Port GPIOC
#define ID7_Pin GPIO_PIN_14
#define ID7_GPIO_Port GPIOC
#define ID8_Pin GPIO_PIN_15
#define ID8_GPIO_Port GPIOC
#define MOTOR_FG_Pin GPIO_PIN_0
#define MOTOR_FG_GPIO_Port GPIOC
#define MOTOR_LD_Pin GPIO_PIN_1
#define MOTOR_LD_GPIO_Port GPIOC
#define MOTOR_CCW_Pin GPIO_PIN_2
#define MOTOR_CCW_GPIO_Port GPIOC
#define MOTOR_ONOFF_Pin GPIO_PIN_3
#define MOTOR_ONOFF_GPIO_Port GPIOC
#define MOTOR_CK_Pin GPIO_PIN_0
#define MOTOR_CK_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define PWM1_HEAT_PIN_Pin GPIO_PIN_9
#define PWM1_HEAT_PIN_GPIO_Port GPIOE
#define PWM2_COOL_PIN_Pin GPIO_PIN_11
#define PWM2_COOL_PIN_GPIO_Port GPIOE
#define PWM3_HEAT_PIN_Pin GPIO_PIN_13
#define PWM3_HEAT_PIN_GPIO_Port GPIOE
#define PWM4_HEAT_PIN_Pin GPIO_PIN_14
#define PWM4_HEAT_PIN_GPIO_Port GPIOE
#define WIFI_POWER_Pin GPIO_PIN_14
#define WIFI_POWER_GPIO_Port GPIOB
#define WIFI_RST_Pin GPIO_PIN_9
#define WIFI_RST_GPIO_Port GPIOD
#define WIFI_LINK_Pin GPIO_PIN_10
#define WIFI_LINK_GPIO_Port GPIOD
#define WIFI_Ready_Pin GPIO_PIN_12
#define WIFI_Ready_GPIO_Port GPIOD
#define WIFI_ReLoad_Pin GPIO_PIN_14
#define WIFI_ReLoad_GPIO_Port GPIOD
#define TF_OK_Pin GPIO_PIN_3
#define TF_OK_GPIO_Port GPIOD
#define SPI3_SCK_Pin GPIO_PIN_3
#define SPI3_SCK_GPIO_Port GPIOB
#define SPI3_MISO_Pin GPIO_PIN_4
#define SPI3_MISO_GPIO_Port GPIOB
#define SPI3_MOSI_Pin GPIO_PIN_5
#define SPI3_MOSI_GPIO_Port GPIOB
#define SPI3_CS_Pin GPIO_PIN_6
#define SPI3_CS_GPIO_Port GPIOB
#define RUN_LED_Pin GPIO_PIN_8
#define RUN_LED_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_9
#define BEEP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
