#include "pwm_task.h"



/**********************************************************************
 * 函数名称： PWM_Task_Func
 * 功能描述： PWM任务函数，用于定期更新PWM通道的占空比
 * 输入参数： 
 *     - argument: 任务函数的参数，一般在任务创建时传入（此处未使用）
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期 	  版本号 		修改人		  修改内容
 * -----------------------------------------------
 * 2024/7/9   V1.0	 	lxw		  创建
 ***********************************************************************/
void PWM_Task_Func(void *argument)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // 50ms
    TickType_t xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
        // 更新PWM
        Update_PWM();

        // 周期性延时
        vTaskDelayUntil(&xLastWakeTime, xFrequency);		

	}

}



