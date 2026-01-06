#ifndef __DRIVER_PWM_H
#define __DRIVER_PWM_H


#include "stm32f4xx_hal.h"
#include "tim.h"



#define TIMER_CLOCK_FREQUENCY 		168000000
#define ARR_VALUE  					8400-1 
#define PSC_VALUE  					4000-1

#define DATA_SIZE 200
#define MIN_FREQ 10
#define STEP 10


// 定义pwm结构体
typedef struct{
//  uint16_t psc_value;
//	uint16_t arr_value;
    uint32_t ccr_value;
	uint8_t duty;
	uint8_t update; // 标志位，指示是否需要更新
} pwm;

extern pwm pwm_para;


void Update_PWM(void);



#endif	//__DRIVER_PWM_H


