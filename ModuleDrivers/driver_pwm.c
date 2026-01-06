
#include "driver_pwm.h"

pwm pwm_para1 = {0};
pwm pwm_para2 = {0};
pwm pwm_para3 = {0};
pwm pwm_para4 = {0};


/********************
 * Freq = 168M / (PSC + 1) / (ARR + 1)
 *
 * Duty = CCR / (ARR + 1)
 *********************/
 /**********************************************************************
 * 函数名称： PWM_SetCCR
 * 功能描述： 设置指定PWM通道的比较寄存器值，从而改变占空比
 * 输入参数： 
 *     - Channel: PWM通道号（如TIM_CHANNEL_1, TIM_CHANNEL_2等）
 *     - Compare: 比较寄存器值，用于设置PWM占空比
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期 	  版本号 		修改人		  修改内容
 * -----------------------------------------------
 * 2024/7/9   V1.0	 	lxw		  创建
 ***********************************************************************/
void PWM_SetCCR(uint32_t Channel, uint32_t Compare)
{
    __HAL_TIM_SET_COMPARE(&htim1, Channel, Compare); // 更改CCR寄存器的值
}



/**********************************************************************
 * 函数名称： PWM_Process
 * 功能描述： 根据占空比计算比较寄存器值，并更新指定PWM通道的比较寄存器
 * 输入参数： 
 *     - pwm_para: 指向包含占空比和比较寄存器值的结构体的指针
 *     - Channel: PWM通道号（如TIM_CHANNEL_1, TIM_CHANNEL_2等）
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期 	  版本号 		修改人		  修改内容
 * -----------------------------------------------
 * 2024/7/9   V1.0	 	lxw		  创建
 ***********************************************************************/
void PWM_Process(pwm *pwm_para, uint32_t Channel)
{
	if (pwm_para->update) 
	{
	    pwm_para->ccr_value = (uint32_t)(pwm_para->duty / 100.0 * (ARR_VALUE + 1));
	    PWM_SetCCR(Channel, pwm_para->ccr_value);		
		pwm_para->update = 0; // 更新后清除标志位
	}
}


/**********************************************************************
 * 函数名称： Update_PWM
 * 功能描述： 更新所有PWM通道的占空比
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期 	  版本号 		修改人		  修改内容
 * -----------------------------------------------
 * 2024/7/9   V1.0	 	lxw		  创建
 ***********************************************************************/
void Update_PWM(void)
{
#if 0
    /* 互斥逻辑：PWM1和PWM2 */
    if (pwm_para1.duty > 0) 
	{
        // 使用PWM1_HEAT（加热），将PWM2_COOL（制冷）置0
        pwm_para2.duty = 0;
        pwm_para2.update = 1; // 标记需要更新
    } else if (pwm_para2.duty > 0) 
	{
        // 使用PWM2_COOL（制冷），将PWM1_HEAT（加热）置0
        pwm_para1.duty = 0;
        pwm_para1.update = 1; // 标记需要更新
    }


	/* PWM1_HEAT（加热） */
    PWM_Process(&pwm_para1, TIM_CHANNEL_1);
	
	/* PWM2_COOL（制冷） */	
    PWM_Process(&pwm_para2, TIM_CHANNEL_2);
	
	/* PWM3_HEAT（薄膜） */	
    PWM_Process(&pwm_para3, TIM_CHANNEL_3);
	
	/* PWM4_HEAT（热盖） */	
    PWM_Process(&pwm_para4, TIM_CHANNEL_4);
#else
//测试代码
	pwm_para1.duty = 50;
	pwm_para1.update = 1; // 标记需要更新

	pwm_para2.duty = 0;
	pwm_para2.update = 1; // 标记需要更新

	pwm_para3.duty = 50;
	pwm_para3.update = 1; // 标记需要更新

	pwm_para4.duty = 50;
	pwm_para4.update = 1; // 标记需要更新	



    /* 互斥逻辑：PWM1和PWM2 */
    if (pwm_para1.duty > 0) 
	{
        // 使用PWM1_HEAT（加热），将PWM2_COOL（制冷）置0
        pwm_para2.duty = 0;
        pwm_para2.update = 1; // 标记需要更新
    } else if (pwm_para2.duty > 0) 
	{
        // 使用PWM2_COOL（制冷），将PWM1_HEAT（加热）置0
        pwm_para1.duty = 0;
        pwm_para1.update = 1; // 标记需要更新
    }

	
	/* PWM1_HEAT（加热） */
    PWM_Process(&pwm_para1, TIM_CHANNEL_1);
	
	/* PWM2_COOL（制冷） */	
    PWM_Process(&pwm_para2, TIM_CHANNEL_2);
	
	/* PWM3_HEAT（薄膜） */	
    PWM_Process(&pwm_para3, TIM_CHANNEL_3);
	
	/* PWM4_HEAT（热盖） */	
    PWM_Process(&pwm_para4, TIM_CHANNEL_4);


#endif
}


