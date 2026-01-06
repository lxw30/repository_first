#include "device_ADC_ad7124.h"


extern SemaphoreHandle_t xMutex;


static void AD_IO_Configuration(void) ;
static void AD7124_8_Reset(void);
static void AD7124_8_INIT(void);

static void Voltage1_Read(void);
static void T_Temp_Read(void);
static void Pt_Temp_Read(void);


ADCDevice AD7124 = {
	.Data_temp = 0,
	.Data_float = 0, 
	.rtd_value1 = 0,	
	.Vol1 = 0,
	.Vol2 = 0,   		//Vol1是真空计采集电压，Vol2是备用通道采集电压，单位：V
	.pt_temp = 0,    	//pt_temp是冷端补偿温度,单位℃
	.t_temp = 0,      	//t_temp是T型热电偶的温度，单位℃
	.t_voltage = 0,
	.t_voltage2 = 0,	
	.Configuration = AD_IO_Configuration,
	.Reset = AD7124_8_Reset,
	.Init = AD7124_8_INIT,
	.Voltage1_Read = Voltage1_Read,
	.T_Temp_Read = T_Temp_Read,
	.Pt_Temp_Read = Pt_Temp_Read,
};


static void AD_IO_Configuration(void) 
{ 
//  GPIO_InitTypeDef GPIO_InitStructure; 
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
//	
//	//GPIOA 初始化设置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
} 




static void AD7124_8_Write(uint8_t Data)
{
  uint8_t data;
	uint8_t i;
  data=Data;
  for(i=0;i<8;i++)
  {
    AD7124_8_SCLK_L;
    if(data&0x80)
    AD7124_8_DIN_H;
    else
    AD7124_8_DIN_L;
//    Delay(200);
	vTaskDelay(pdMS_TO_TICKS(200));
    AD7124_8_SCLK_H;
//    Delay(200);
	vTaskDelay(pdMS_TO_TICKS(200));
    data<<=1;
  }
}

static uint32_t AD7124_8_Read(void)
{
  uint32_t DATA;
	uint8_t i;
  DATA=0;
  for(i=0;i<24;i++)
  {
    DATA<<=1;
    AD7124_8_SCLK_L;
//    Delay(20);
	vTaskDelay(pdMS_TO_TICKS(20));
    if(AD7124_8_DOUT)
    DATA++;
    AD7124_8_SCLK_H;
//    Delay(20);
	vTaskDelay(pdMS_TO_TICKS(20));
  }
//  Delay(200);
  vTaskDelay(pdMS_TO_TICKS(200));
  return DATA;
}


#if 0
	static uint8_t AD7124_8_Read_Byte(void)
	{
	  uint8_t DATA;
		uint8_t i;
	  DATA=0;
	  for(i=0;i<8;i++)
	  {
		DATA<<=1;
		AD7124_8_SCLK_L;
		Delay(200);
		if(AD7124_8_DOUT)
		DATA++;
		AD7124_8_SCLK_H;
		Delay(200);
	  }
	  Delay(200);
	  return DATA;
	}
	
#endif	

static void AD7124_8_Reset(void)
{
  uint8_t a;
	AD7124_8_CS_L;
  AD7124_8_DIN_H;
  for(a=0;a<72;a++)  //至少64个，目前按72个，保证复位彻底
  {
    AD7124_8_SCLK_L;
//    Delay(100);
	vTaskDelay(pdMS_TO_TICKS(100));
    AD7124_8_SCLK_H;
//    Delay(100);
	vTaskDelay(pdMS_TO_TICKS(100));	
  }
  AD7124_8_CS_H;
}

static void AD7124_8_INIT(void)
{
  AD7124_8_CS_L;
//  Delay(100);
  vTaskDelay(pdMS_TO_TICKS(100));


  //********通道设置********//
  AD7124_8_Write(0x09);//CHANNEL_0  T电偶采集，禁止，配置方案0， P=AI2,N=AI3
  AD7124_8_Write(0x00);//禁止， 配置方案0
  AD7124_8_Write(0x43);//P=AI2, N=AI3
	
  AD7124_8_Write(0x0A);//CHANNEL_1  真空计采集，禁止，配置方案1，P为AIN4， N为AIN5
  AD7124_8_Write(0x10);//禁止， 配置方案1
  AD7124_8_Write(0x85);//P为AIN4， N为AIN5

  AD7124_8_Write(0x0B);//CHANNEL_2  PT100采集，禁止， 配置方案2，P=AI12, N=AI13
  AD7124_8_Write(0x21);//禁止， 配置方案2
  AD7124_8_Write(0x8D);//P=AI12, N=AI13

  //********配置设置********//
  AD7124_8_Write(0x19);//CONFIG_0
  AD7124_8_Write(0x08);// 双极性，开路检测关，REF缓冲关， AI缓冲开
  AD7124_8_Write(0x77);// 内部基准源 Pga=128

  AD7124_8_Write(0x1A);//CONFIG_1
  AD7124_8_Write(0x00);// 单极性，开路检测关，REF缓冲关， AI缓冲开
  AD7124_8_Write(0x70);// 内部基准源 Pga=1

  AD7124_8_Write(0x1B);//CONFIG_2
  AD7124_8_Write(0x01);// 单极性，开路检测关，REF缓冲开， AI缓冲开
  AD7124_8_Write(0xE5);// 基准选择REFIN1+-     Pga=32

  //********滤波器设置********//
  AD7124_8_Write(0x21);//FILTER_0 
  AD7124_8_Write(0x06);// sin4滤波器,25PSP,零延迟关
  AD7124_8_Write(0x01);
  AD7124_8_Write(0x80);// 转换速率384
  
  AD7124_8_Write(0x22);//FILTER_1 
  AD7124_8_Write(0x06);// sin4滤波器,25PSP,零延迟关
  AD7124_8_Write(0x01);
  AD7124_8_Write(0x80);// 转换速率384

  AD7124_8_Write(0x23);//FILTER_2 
//  AD7124_8_Write(0x06);// sin4滤波器,25PSP,零延迟关
//  AD7124_8_Write(0x01);
//  AD7124_8_Write(0x80);// 转换速率384	
  AD7124_8_Write(0x06);// sin4滤波器,25PSP,零延迟关
  AD7124_8_Write(0x00);
  AD7124_8_Write(0xA0);// 转换速率160	

  //********IO_CTRL2设置********//	
  AD7124_8_Write(0x04);//IO_CTRL2_REG
  AD7124_8_Write(0x00);
  AD7124_8_Write(0x05);//内部偏置电压AI0,AI2开启，热电偶P端使用

   //********ADC_CONTROL设置********//
  AD7124_8_Write(0x01);//ADC_CONTROL
  AD7124_8_Write(0x01);//禁止连续读，REF内部电压使能
  AD7124_8_Write(0xC0);//全功率，连续转换模式MODE0，内部时钟

  //CS使能始终是有效（低)
	
}


static void Voltage1_Read(void)
{
	  static uint32_t Data_temp;
		static float Data_float; 
		
	static TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(80); // 80ms


/* 获取互斥量（上锁） */
	if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
	{
			AD7124_8_CS_L;
			
			AD7124_8_Write(0x0A);//使能CHANNEL_1	真空计采集，使能，配置方案1，P为AIN4， N为AIN5
			AD7124_8_Write(0x90);//使能， 配置方案1
			AD7124_8_Write(0x85);//P为AIN4， N为AIN5    
			
		// 延时5ms
		vTaskDelay(pdMS_TO_TICKS(5)); 
		
		// 获取当前时间
		xLastWakeTime = xTaskGetTickCount();
		// 延时80ms
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
			while(AD7124_8_DOUT)  //等待RDY转换完成信号
			{
				vTaskDelay(pdMS_TO_TICKS(1)); // 避免忙等待;
			}
		
				AD7124_8_Write(0x42);		//发起读数据寄存器操作
			  Data_temp=AD7124_8_Read();  //连续读24位
				Data_float=Data_temp;
			  AD7124.Vol1=(Data_float/0x1000000)* 2.5f *4.1666667f;  //数据转换，4.16666是电阻分压比，10V降到2.4V
		
			  AD7124_8_Write(0x0A);//禁止CHANNEL_1  真空计采集，禁止，配置方案1，P为AIN4， N为AIN5
			  AD7124_8_Write(0x10);//禁止， 配置方案1
			  AD7124_8_Write(0x85);//P为AIN4， N为AIN5
		
		// 释放互斥量
		xSemaphoreGive(xMutex); 

	}

}



static void T_Temp_Read(void)
{	
	static double pt_voltage = 0;
	static double int_t_temp = 0;

	static uint32_t Data_temp;
	static float Data_float; 

	static TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(80); // 80ms

/* 获取互斥量（上锁） */	
	if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
	{
				AD7124_8_CS_L;
				AD7124_8_Write(0x09);//使能CHANNEL_0	T电偶采集，禁止，配置方案0， P=AI2,N=AI3
				AD7124_8_Write(0x80);//使能， 配置方案0
				AD7124_8_Write(0x43);//P=AI2, N=AI3
		
		// 延时5ms
			vTaskDelay(pdMS_TO_TICKS(5)); 
		
		// 获取当前时间
			xLastWakeTime = xTaskGetTickCount();
		// 延时80ms
			vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
		
			while(AD7124_8_DOUT)  //等待RDY转换完成信号
			{
				vTaskDelay(pdMS_TO_TICKS(1)); // 避免忙等待
			} 
		
		
				AD7124_8_Write(0x42);		//发起读数据寄存器操作
				Data_temp=AD7124_8_Read();	//连续读24位
				Data_float=Data_temp;
				AD7124.t_voltage=2.5*1000* (Data_float-0x800000)/(0x800000*128);  //数据转换，增益是128，双极性需要减去中点,单位是mV
				AD7124.t_voltage2 = AD7124.t_voltage;
				
				AD7124_8_Write(0x09);//禁止CHANNEL_0	T电偶采集，禁止，配置方案0， P=AI2,N=AI3
				AD7124_8_Write(0x00);//禁止， 配置方案0
				AD7124_8_Write(0x43);//P=AI2, N=AI3
		
				//**************以下为热电偶的压差解算**************//
				if(AD7124.pt_temp >= 0)
				{
					pt_voltage =  0 + 
												0.387481063640 * 1e-1 * AD7124.pt_temp + 
												0.332922278800 * 1e-4 * AD7124.pt_temp * AD7124.pt_temp +
												0.206182434040 * 1e-6 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												-0.218822568460 * 1e-8 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.109968809280 * 1e-10 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												-0.308157587720 * 1e-13 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.454791352900 * 1e-16 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												-0.275129016730 * 1e-29 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp;
				}
				else
				{
					pt_voltage =  0 + 
												0.387481063640 * 1e-1 * AD7124.pt_temp + 
												0.441944343470 * 1e-4 * AD7124.pt_temp * AD7124.pt_temp +
												0.118443231050 * 1e-6 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.200329735540 * 1e-7 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.901380195590 * 1e-9 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.226511565930 * 1e-10 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.360711542050 * 1e-12 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.384939398830 * 1e-14 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.282135219250 * 1e-16 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.142515947790 * 1e-18 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.487686622860 * 1e-21 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.107955392700 * 1e-23 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.139450270620 * 1e-26 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp +
												0.797951539270 * 1e-30 * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp * AD7124.pt_temp;
				}
				
				AD7124.t_voltage = AD7124.t_voltage + pt_voltage;
				
				if(AD7124.t_voltage < 0)
				{
					int_t_temp =	0 +
												2.5949192	 * 1e+1 * AD7124.t_voltage + 
												-2.1316967 * 1e-1 * AD7124.t_voltage * AD7124.t_voltage + 
												7.9018692 * 1e-1 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												4.2527777 * 1e-1 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												1.3304473 * 1e-1 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												2.0241446	* 1e-2 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												1.2668171 * 1e-3 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage;
				}
				else
				{
					int_t_temp =	0 +
												2.592800 * 1e+1 * AD7124.t_voltage + 
												-7.602961 * 1e-1 * AD7124.t_voltage * AD7124.t_voltage + 
												4.637791 * 1e-2 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												-2.165394 * 1e-3 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												6.048144 * 1e-5 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage +
												-7.293422 * 1e-7 * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage * AD7124.t_voltage;
				}
				AD7124.t_temp = (float)int_t_temp;
		
	// 释放互斥量
		xSemaphoreGive(xMutex); 
	
	}
	
}


//AI14输出激励电流250uA
//AI15输出激励电流250uA
//AI12模拟输入正端
//AI13模拟输入负端
static void Pt_Temp_Read(void)
{	
//	uint32_t read_data;
//	uint8_t ad_channel;
//	float rtd_value1;
	float rtd_value2;
	float rtd_value_average;
//	uint32_t Data_temp;
//	float Data_float; 


	static TickType_t xLastWakeTime1;
	static TickType_t xLastWakeTime2;
	const TickType_t xFrequency = pdMS_TO_TICKS(80); // 80ms


// 获取互斥量（上锁）
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
    {
		AD7124_8_CS_L;	
		AD7124_8_Write(0x03);//IO_CTRL1设置
		AD7124_8_Write(0x00);// GPIO不使能
		AD7124_8_Write(0x1B);// 电桥关，IOUT1输出250uA, IOUT0输出250uA
		AD7124_8_Write(0xEF);// IOUT1通过AIN14输出，IOUT0通过AIN15输出，交换
	
		AD7124_8_Write(0x0B);//使能CHANNEL_2	PT100采集，使能， 配置方案2，P=AI12, N=AI13
		AD7124_8_Write(0xA1);//使能， 配置方案2
		AD7124_8_Write(0x8D);//P=AI12, N=AI13
	
	// 延时5ms
		vTaskDelay(pdMS_TO_TICKS(5)); 


// 获取当前时间
		xLastWakeTime1 = xTaskGetTickCount();
// 延时80ms
		vTaskDelayUntil(&xLastWakeTime1, xFrequency);
	
		while(AD7124_8_DOUT)  //等待RDY转换完成信号
		{
			vTaskDelay(pdMS_TO_TICKS(1)); // 避免忙等待
		} 
	
		
		AD7124_8_Write(0x42);		//发起读数据寄存器操作
		AD7124.Data_temp=AD7124_8_Read();  //连续读24位
		AD7124.Data_float=AD7124.Data_temp;
		AD7124.rtd_value1=(AD7124.Data_float / 32) * 10220 / 0x1000000;  //数据转换成PT100阻值(是阻值！)，增益是32,基准 5.11K*2*250uA,  转换成电阻还要/250uA, 这样就相当于5110*2= 10220
	
// 释放互斥量
        xSemaphoreGive(xMutex);		
	}    

// 等待一段时间，再次获取互斥量
    vTaskDelay(pdMS_TO_TICKS(5)); 


// 获取互斥量（上锁）
    if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
    {
		AD7124_8_Write(0x03);//IO_CTRL1设置
		AD7124_8_Write(0x00);// GPIO不使能
		AD7124_8_Write(0x1B);// 电桥关，IOUT1输出250uA, IOUT0输出250uA
		AD7124_8_Write(0xFE);// IOUT1通过AIN15输出，IOUT0通过AIN14输出，交换
		
		AD7124_8_Write(0x0B);//使能CHANNEL_2  PT100采集，使能， 配置方案2，P=AI12, N=AI13
		AD7124_8_Write(0xA1);//使能， 配置方案2
		AD7124_8_Write(0x8D);//P=AI12, N=AI13

// 延时5ms
        vTaskDelay(pdMS_TO_TICKS(5));


// 获取当前时间
		xLastWakeTime2 = xTaskGetTickCount();
// 延时80ms
		vTaskDelayUntil(&xLastWakeTime2, xFrequency);

		while(AD7124_8_DOUT)  //等待RDY转换完成信号
		{
			 vTaskDelay(pdMS_TO_TICKS(1)); // 避免忙等待
		} 


		AD7124_8_Write(0x42);		//发起读数据寄存器操作
	    AD7124.Data_temp=AD7124_8_Read();  //连续读24位
		AD7124.Data_float=AD7124.Data_temp;
		rtd_value2=(AD7124.Data_float / 32) * 10220 / 0x1000000;  //数据转换成PT100阻值(是阻值！)，增益是32,基准 5.11K*2*250uA,  转换成电阻还要/250uA, 这样就相当于5110*2= 10220

		rtd_value_average = (AD7124.rtd_value1 + rtd_value2) / 2;
		AD7124.pt_temp = (-0.0039083f + sqrt(0.0039083f * 0.0039083f - 4 * (-0.0000005775f)*(1 - rtd_value_average/100.0f))) / (2 * (-0.0000005775f));
				
		AD7124_8_Write(0x03);//IO_CTRL1设置
		AD7124_8_Write(0x00);// GPIO不使能
		AD7124_8_Write(0x00);// 电桥关，IOUT1 IOUT0关闭
		AD7124_8_Write(0x00);// IOUT1 IOUT0关闭

		AD7124_8_Write(0x0B);//禁止CHANNEL_2  PT100采集，禁止， 配置方案2，P=AI12, N=AI13
		AD7124_8_Write(0x21);//禁止， 配置方案2
		AD7124_8_Write(0x8D);//P=AI12, N=AI13		

// 释放互斥量
        xSemaphoreGive(xMutex);		
	}   
}




