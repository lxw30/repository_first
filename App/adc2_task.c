#include "adc2_task.h"

extern ADCDevice AD7124_2;

void ADC2_Task_Func(void *argument)
{


	while(1)
	{
	
	/* 采集样品温度1 */
	    //通道切换
		AD7124_2.Pt_Temp_Read();
		
	/* 采集样品温度2 */
		//通道切换
		AD7124_2.Pt_Temp_Read();

	/* 采集模组温度1 */
		//通道切换
		AD7124_2.Pt_Temp_Read();

	/* 采集模组温度2 */
		//通道切换
		AD7124_2.Pt_Temp_Read();
		
	}

}







