#include "lcd_device.h"

//显示页面
static void DisPage(uint16_t Screen_id);

//显示文本
static void PutString (uint16_t Screen_id, uint16_t Control_id, char* string);
static void PutString_uint16(uint16_t Screen_id, uint16_t Control_id, int data);
static void PutString_int16(uint16_t Screen_id, uint16_t Control_id, int data);
static void PutString_uint32(uint16_t Screen_id, uint16_t Control_id, int data);
static void PutString_float(uint16_t Screen_id, uint16_t Control_id, float data);
static void PutString_float2(uint16_t Screen_id, uint16_t Control_id, float data);
static void PutString_float_mbar(uint16_t Screen_id, uint16_t Control_id, float data);


//显示按键
static void DisButton (uint16_t screen_id,uint16_t control_id,uint8_t frame_id);

//设置文本颜色及背景色
void Set_Color_On(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back);
void Set_Color_Off(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back);


void Init_Lcd_Device(void);

LcdDeviceStatus lcdstatus = {0};
//PLcdDeviceStatus plcdstatus = &lcdstatus;

LcdDevice lcd;



DFS Disfuc = {
	DisPage,
	PutString,
	PutString_uint16,
	PutString_int16,
	PutString_uint32,	
	PutString_float,
	PutString_float2,
	PutString_float_mbar,
	DisButton,
	Set_Color_On,
	Set_Color_Off,
};

PDFS disfuc = &Disfuc;



/**********************************************************************
 * 函数名称： Display
 * 功能描述： 串口屏显示模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/5/8   V1.0	  lxw	      创建
 * 2024/5/13  V2.0	  lxw	      好多
 ***********************************************************************/
static void Display(void)
{
	Init_Lcd_Device();
	PLcdDevice plcd = &lcd;
	
#if 0
/* 开机画面 */
	disfuc->DisPage(start_page);
#endif

/* 显示画面 */
	disfuc->DisPage(main_page);

/* 实验状态 */
	plcd->status->rt_temp_c = round(reg_value.AI2 / 10.0) / 10.0;
	plcd->status->rt_temp_k = plcd->status->rt_temp_c + 273.15f;

	
	plcd->status->rt_pressure_pa = reg_value.AI3;
#if 0
	plcd->status->rt_pressure_mbar = round(plcd->status->rt_pressure_pa / 100.0);
#else
	plcd->status->rt_pressure_mbar = plcd->status->rt_pressure_pa / 100.0f;
#endif
	
	//温度，压力，速率，单位
	if(reg_flag.temp_unit == 1)
	{
		reg_flag.temp_unit = 0;
		if(reg_value.temp_unit == 1)
		{
			plcd->status->temp_set_flag = 1;
			disfuc->PutString(main_page, rt_temp_unit_text, TEMP_UNIT_K);
			disfuc->PutString(main_page, exp_step_temp_unit_text, TEMP_UNIT_K);
		} else {
			plcd->status->temp_set_flag = 0;
			disfuc->PutString(main_page, rt_temp_unit_text, TEMP_UNIT_C);	
			disfuc->PutString(main_page, exp_step_temp_unit_text, TEMP_UNIT_C);
		}
	}

	if(reg_flag.pressure_unit == 1)
	{
		reg_flag.pressure_unit = 0;
		if(reg_value.pressure_unit == 1)
		{
			plcd->status->pressure_set_flag = 1;
			disfuc->PutString(main_page, rt_pressure_unit_text, PRESSURE_UNIT_MBAR);
			disfuc->PutString(main_page, exp_step_pressure_unit_text, PRESSURE_UNIT_MBAR);
		} else {
			plcd->status->pressure_set_flag = 0;
			disfuc->PutString(main_page, rt_pressure_unit_text, PRESSURE_UNIT_PA);
			disfuc->PutString(main_page, exp_step_pressure_unit_text, PRESSURE_UNIT_PA);
		} 
	}

	if(reg_flag.rate_unit == 1)
	{
		reg_flag.rate_unit = 0;
		if(reg_value.rate_unit == 1)
		{
			disfuc->PutString(main_page, exp_step_rate_unit_text, RATE_UNIT_K_PER_MIN);
		} else {
			disfuc->PutString(main_page, exp_step_rate_unit_text, RATE_UNIT_C_PER_MIN);
		} 
	}

	if(plcd->status->temp_set_flag)
	{
		disfuc->PutString_float(main_page, rt_temp_text, plcd->status->rt_temp_k);
	} else {
		disfuc->PutString_float(main_page, rt_temp_text, plcd->status->rt_temp_c);
	}
	if(plcd->status->pressure_set_flag)
	{
#if 0		
		disfuc->PutString_uint32(main_page, rt_pressure_text, plcd->status->rt_pressure_mbar);
#else
		disfuc->PutString_float_mbar(main_page, rt_pressure_text, plcd->status->rt_pressure_mbar/100.0f);
#endif
	} else {
#if 0
		disfuc->PutString_uint32(main_page, rt_pressure_text, plcd->status->rt_pressure_pa);
#else
		disfuc->PutString_float2(main_page, rt_pressure_text, plcd->status->rt_pressure_pa/100.0f);
#endif		
	}


	//通讯状态(已连接、未连接)
	disfuc->PutString(main_page, comm_status_text, COMM_STATUS_CONNECTED);
//	disfuc->PutString(main_page, comm_status_text, COMM_STATUS_DISCONNECTED);
	//实验状态(待机，进行中)
	if(reg_flag.exp_status == 1)
	{
		reg_flag.exp_status = 0;
		if(reg_value.exp_status == 1)
		{
			disfuc->Set_Color_On(main_page, exp_status_text, BLACK, GREEN);
			disfuc->PutString(main_page, exp_status_text, EXP_STATUS_RUNNING);
		} else {
			disfuc->Set_Color_Off(main_page, exp_status_text, WHITE, NULL);
			disfuc->PutString(main_page, exp_status_text, EXP_STATUS_STANDBY);
		}
	}
	
/* 实验步段：0x10一起设置 */
	if(reg_flag.exp_step == 1)
	{
		reg_flag.exp_step = 0;
		
		//上一步
		disfuc->DisButton(main_page, prev_step_button, reg_value.steps[0].button);	
		disfuc->PutString_uint16(main_page, prev_step_duration_text, reg_value.steps[0].duration);		
		
		//当前步
		disfuc->DisButton(main_page, cur_step_button, reg_value.steps[1].button);
		disfuc->PutString_uint16(main_page, cur_step_duration_text, reg_value.steps[1].duration);
		
		//下一步
		disfuc->DisButton(main_page, next_step_button, reg_value.steps[2].button);
		disfuc->PutString_uint16(main_page, next_step_duration_text, reg_value.steps[2].duration);
		
		if(reg_value.rate_unit == 1)
		{
			disfuc->PutString_float(main_page, prev_step_rate_text, reg_value.steps[0].rate_k/10.0);
			disfuc->PutString_float(main_page, cur_step_rate_text, reg_value.steps[1].rate_k/10.0);
			disfuc->PutString_float(main_page, next_step_rate_text, reg_value.steps[2].rate_k/10.0);			
		} else {
			disfuc->PutString_float(main_page, prev_step_rate_text, reg_value.steps[0].rate_c/10.0);
			disfuc->PutString_float(main_page, cur_step_rate_text, reg_value.steps[1].rate_c/10.0);
			disfuc->PutString_float(main_page, next_step_rate_text, reg_value.steps[2].rate_c/10.0);			
		} 		

		if(plcd->status->temp_set_flag)
		{
			disfuc->PutString_float(main_page, prev_step_target_temp_text, reg_value.steps[0].target_temp_k/10.0);
			disfuc->PutString_float(main_page, cur_step_target_temp_text, reg_value.steps[1].target_temp_k/10.0);
			disfuc->PutString_float(main_page, next_step_target_temp_text, reg_value.steps[2].target_temp_k/10.0);
		} else {
			disfuc->PutString_float(main_page, prev_step_target_temp_text, reg_value.steps[0].target_temp_c/10.0);
			disfuc->PutString_float(main_page, cur_step_target_temp_text, reg_value.steps[1].target_temp_c/10.0);
			disfuc->PutString_float(main_page, next_step_target_temp_text, reg_value.steps[2].target_temp_c/10.0);			
		}
		if(plcd->status->pressure_set_flag)
		{
			disfuc->PutString_float(main_page, prev_step_target_pressure_text, reg_value.steps[0].target_pressure_mbar/100.0);
			disfuc->PutString_float(main_page, cur_step_target_pressure_text, reg_value.steps[1].target_pressure_mbar/100.0);
			disfuc->PutString_float(main_page, next_step_target_pressure_text, reg_value.steps[2].target_pressure_mbar/100.0);
		} else {
			disfuc->PutString_uint16(main_page, prev_step_target_pressure_text, reg_value.steps[0].target_pressure_pa);
			disfuc->PutString_uint16(main_page, cur_step_target_pressure_text, reg_value.steps[1].target_pressure_pa);
			disfuc->PutString_uint16(main_page, next_step_target_pressure_text, reg_value.steps[2].target_pressure_pa);
		}	

	}


/* 设备状态 */
	if(reg_value.AO3)
	{
		reg_value.DI3 = 1;
	} else {
		reg_value.DI3 = 0;
	}
	if(pwm_para.duty)
	{
		reg_value.DI1 = 1;
	} else {
		reg_value.DI1 = 0;
	}
	if(reg_value.DO2)
	{
		reg_value.DI2 = 1;
	} else {
		reg_value.DI2 = 0;
	}
	
	disfuc->DisButton(main_page, cooling_status_button, reg_value.DI3);
	disfuc->DisButton(main_page, heating_status_button, reg_value.DI1);
	disfuc->DisButton(main_page, vacuum_pump_status_button, reg_value.DI2);

}


#if 0
/* 显示温度和压力，放在定时器中 */
static void Display_FromISR(void)
{
	
}
#endif

static void SendChar(uint8_t c)
{
	USART1->SR;
	USART1->DR = c;
	while((USART1->SR & 0x40) == 0);
	USART_ClearITPendingBit(USART1,USART_IT_TC);
}
/**********************************************************************************************/
//发送开始结束符
/**********************************************************************************************/
static void SendBeginCMD(void)
{
	SendChar(0xee);
}
/**********************************************************************************************/
//发送命令结束符
/**********************************************************************************************/
static void SendEndCMD(void)
{
	SendChar(0xff);
	SendChar(0xfc);
	SendChar(0xff);
	SendChar(0xff);	
}


/*! 
*  \brief     设置控件背景色
*  \details  支持控件：进度条、文本
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 背景色
*/
void SetControlBackColor(uint16 screen_id,uint16 control_id,uint16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x18);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(color);
    END_CMD();
}

//设置控件背景透明
void SetControlBackColor_transparency(uint16 screen_id,uint16 control_id)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x17);
    TX_16(screen_id);
    TX_16(control_id);
    END_CMD();
}

/*! 
*  \brief     设置控件前景色
* \details  支持控件：进度条
*  \param  screen_id 画面ID
*  \param  control_id 控件ID
*  \param  color 前景色
*/
void SetControlForeColor(uint16 screen_id,uint16 control_id,uint16 color)
{
    BEGIN_CMD();
    TX_8(0xB1);
    TX_8(0x19);
    TX_16(screen_id);
    TX_16(control_id);
    TX_16(color);
    END_CMD();
}



#if 0
	/*! 
	*  \brief  串口发送送N个字节
	*  \param  个数
	*/
	static void SendNU8(uint8_t *pData, uint16_t nDataLen)
	{
		uint16_t i = 0;
		for (;i<nDataLen;++i)
		{
			TX_8(pData[i]);
		}
	}
	/*! 
	*  \brief  串口发送送N个16位的数据
	*  \param  个数
	*/
	static void SendNU16(uint16_t *pData, uint16_t nDataLen)
	{
		uint16_t i = 0;
		for (;i<nDataLen;++i)
		{
			TX_16(pData[i]);
		}
	}


	/*! 
	*  \brief     播放指定帧
	*  \param  screen_id 画面ID
	*  \param  control_id 控件ID
	*  \param  frame_id 帧ID
	*/
	static void AnimationPlayFrame(uint16_t screen_id,uint16_t control_id,uint8_t frame_id)
	{
		BEGIN_CMD();
		TX_8(0xB1);
		TX_8(0x23);
		TX_16(screen_id);
		TX_16(control_id);
		TX_8(frame_id);
		END_CMD();
	}

#endif

void Set_Button_Status(uint16 Screen_id,uint16 Control_id,uint8 Status)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar((Screen_id>>8)&0xff);
	SendChar(Screen_id&0xff);
	SendChar((Control_id>>8)&0xff);
	SendChar(Control_id&0xff);
	SendChar(Status);
	SendEndCMD();
}






//切换页面
static void DisPage(uint16_t Screen_id)
{
	SendBeginCMD();
	SendChar(0xB1);
	SendChar(0x00);
	SendChar((Screen_id>>8)&0xff);
	SendChar(Screen_id&0xff);
	SendEndCMD();	
}


//显示文本
static void PutString (uint16_t Screen_id, uint16_t Control_id, char* string)
{
	SendBeginCMD();
	SendChar(0xb1);
	SendChar(0x10);
	SendChar(Screen_id>>8);
	SendChar(Screen_id);
	SendChar(Control_id>>8);
	SendChar(Control_id);
	do
	{
		SendChar(*string);
	}
	while(*string++);
	SendEndCMD();	
}
//显示无符号16位整数
static void PutString_uint16(uint16_t Screen_id, uint16_t Control_id, int data)
{
	char str[16]; 
	unsigned char i; 	
	int8_t tmp_data = 0;
	for(i=0;i<16;i++)
	{
		str[i] = 0;
	}

    if(data >= 10000 && data < 65536)
    {
        str[0] = data / 10000 + 0x30;
        str[1] = (data % 10000) / 1000 + 0x30;
        str[2] = (data % 1000) / 100 + 0x30;
        str[3] = (data % 100) / 10 + 0x30;
        str[4] = data % 10 + 0x30;            
    } else if(data >= 1000 && data < 10000)
	{
		str[0] = data / 1000 + 0x30;
		str[1] = (data % 1000) / 100 + 0x30;			
		str[2] = (data % 100) / 10 + 0x30;
		str[3] = data % 10 + 0x30;			
	} else if(data >= 100 && data < 1000)
	{
		str[0] = data / 100 + 0x30;			
		str[1] = (data % 100) / 10 + 0x30;
		str[2] = data % 10 + 0x30;			
	} else if(data >= 10 && data < 100)
	{
		str[0] = (data % 100) / 10 + 0x30;
		str[1] = data % 10 + 0x30;			
	} else if(data >= 0 && data < 10)
	{
		str[0] = data + 0x30;
	} else {
		tmp_data = -data;
		str[0] = 0x2d;
		str[1] = tmp_data + 0x30;
	}
	PutString(Screen_id,Control_id,str);
}

//显示有符号16位整数
static void PutString_int16(uint16_t Screen_id, uint16_t Control_id, int data)
{
	char str[16]; 
//	unsigned char i; 
	
    if(data < 0) {
        snprintf(str, sizeof(str), "%d", data);  // 使用 snprintf 安全格式化负数
    } else {
        if(data >= 10000 && data < 65536) {
            snprintf(str, sizeof(str), "%05d", data);  // 保证包括前导零的五位数
        } else if(data >= 1000 && data < 10000) {
            snprintf(str, sizeof(str), "%04d", data);  // 四位数
        } else if(data >= 100 && data < 1000) {
            snprintf(str, sizeof(str), "%03d", data);  // 三位数
        } else if(data >= 10 && data < 100) {
            snprintf(str, sizeof(str), "%02d", data);  // 两位数
        } else {
            snprintf(str, sizeof(str), "%d", data);    // 一位数
        }
    }

	PutString(Screen_id,Control_id,str);
}



//显示无符号32位整数
static void PutString_uint32(uint16_t Screen_id, uint16_t Control_id, int data)
{
	char str[16]; 
	unsigned char i; 	
	int8_t tmp_data = 0;
	for(i=0;i<16;i++)
	{
		str[i] = 0;
	}

    if(data >= 100000 && data < 1000000)
    {
        str[0] = data / 100000 + 0x30;
        str[1] = (data % 100000) / 10000 + 0x30;
        str[2] = (data % 10000) / 1000 + 0x30;
        str[3] = (data % 1000) / 100 + 0x30;
        str[4] = (data % 100) / 10 + 0x30;
        str[5] = data % 10 + 0x30;            
    } else if(data >= 10000 && data < 100000)
    {
        str[0] = data / 10000 + 0x30;
        str[1] = (data % 10000) / 1000 + 0x30;
        str[2] = (data % 1000) / 100 + 0x30;
        str[3] = (data % 100) / 10 + 0x30;
        str[4] = data % 10 + 0x30;            
    } else if(data >= 1000 && data < 10000)
	{
		str[0] = data / 1000 + 0x30;
		str[1] = (data % 1000) / 100 + 0x30;			
		str[2] = (data % 100) / 10 + 0x30;
		str[3] = data % 10 + 0x30;			
	} else if(data >= 100 && data < 1000)
	{
		str[0] = data / 100 + 0x30;			
		str[1] = (data % 100) / 10 + 0x30;
		str[2] = data % 10 + 0x30;			
	} else if(data >= 10 && data < 100)
	{
		str[0] = (data % 100) / 10 + 0x30;
		str[1] = data % 10 + 0x30;			
	} else if(data >= 0 && data < 10)
	{
		str[0] = data + 0x30;
	} else {
		tmp_data = -data;
		str[0] = 0x2d;
		str[1] = tmp_data + 0x30;
	}		
	PutString(Screen_id,Control_id,str);
}


//显示浮点数（小数点后1位）
static void PutString_float(uint16_t Screen_id, uint16_t Control_id, float data)
{
	char str[64]; 
	unsigned char i; 	

	for(i=0;i<64;i++)
	{
		str[i] = 0;
	}

	sprintf(str, "%.1f", data);
	
	PutString(Screen_id, Control_id, str);
}
//显示浮点数（小数点后2位）
static void PutString_float2(uint16_t Screen_id, uint16_t Control_id, float data)
{
	char str[64]; 
	unsigned char i; 	

	for(i=0;i<64;i++)
	{
		str[i] = 0;
	}

	sprintf(str, "%.2f", data);
	
	PutString(Screen_id, Control_id, str);
}

//显示浮点数（小数点后4位）
static void PutString_float_mbar(uint16_t Screen_id, uint16_t Control_id, float data)
{
	char str[64]; 
	unsigned char i; 	

	for(i=0;i<64;i++)
	{
		str[i] = 0;
	}

	sprintf(str, "%.4f", data);
	
	PutString(Screen_id, Control_id, str);
}



//显示按键
static void DisButton(uint16 Screen_id,uint16 Control_id,uint8 Status)
{
	Set_Button_Status(Screen_id, Control_id, Status);
}


//设置文本颜色及背景色(on: 字体黑色(0x00)，背景绿色(0x07E0)；off：字体白色(0xFFFF)，背景透明)
void Set_Color_On(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back)
{
	SetControlForeColor(screen_id, control_id, color_fore);	//文本颜色
	SetControlBackColor(screen_id, control_id, color_back);	//背景色
}


void Set_Color_Off(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back)
{
	SetControlForeColor(screen_id, control_id, color_fore);
	SetControlBackColor_transparency(screen_id, control_id);
}



/**********************************************************************
 * 函数名称： Init_Lcd_Device
 * 功能描述： 初始化lcd device
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/5/11   V1.0	  lxw	      创建
 ***********************************************************************/
void Init_Lcd_Device(void)
{
    PLcdDeviceStatus plcdstatus = &lcdstatus;
    lcd.status = plcdstatus;  // 在函数内部赋值
    lcd.Display = Display;    // 直接赋值函数指针
}






