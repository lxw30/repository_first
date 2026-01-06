#ifndef __LCD_DEVICE_H
#define __LCD_DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "stm32f4xx.h"
#include "modbus_tcp.h"



#define SEND_DATA(P) SendChar(P)          //发送一个字节

#define BEGIN_CMD() TX_8(0XEE)            //帧头
#define END_CMD() TX_32(0XFFFCFFFF)       //帧尾
#define TX_8(P1) SEND_DATA((P1)&0xFF)                    //发送单个字节
//#define TX_8N(P,N) SendNU8((uint8 *)P,N)                 //发送N个字节
#define TX_16(P1) TX_8((P1)>>8);TX_8(P1)                 //发送16位整数
//#define TX_16N(P,N) SendNU16((uint16 *)P,N)              //发送N个16位整数
#define TX_32(P1) TX_16((P1)>>16);TX_16((P1)&0xFFFF)     //发送32位整数

// 温度单位
#define TEMP_UNIT_C 				"℃"
#define TEMP_UNIT_K 				"K"

// 压力单位
#define PRESSURE_UNIT_PA 			"Pa"
#define PRESSURE_UNIT_MBAR 			"mbar"

// 速率单位
#define RATE_UNIT_C_PER_MIN 		"℃/min"
#define RATE_UNIT_K_PER_MIN 		"K/min"

// 通讯状态
#define COMM_STATUS_CONNECTED 		"已连接"
#define COMM_STATUS_DISCONNECTED 	"断开"

// 实验状态
#define EXP_STATUS_RUNNING 			"进行中"
#define EXP_STATUS_STANDBY 			"待机"
#define WHITE 						0xffff
#define BLACK 						0x0000
#define GREEN 						0x07E0



typedef enum {
    start_page = 0,
    main_page = 1,
} PageID;


typedef enum {
    rt_temp_text = 1,						//温度实时值
    rt_pressure_text = 2,					//压力实时值
	rt_temp_unit_text = 3,					//温度实时值 单位
	rt_pressure_unit_text = 4,				//压力实时值 单位
 	prev_step_rate_text = 11, 				//上一步速率
	prev_step_target_temp_text = 12,		//上一步目标温度
	prev_step_target_pressure_text =13,		//上一步目标压力
	prev_step_duration_text = 14,			//上一步时长
	cur_step_rate_text = 21, 			//当前步速率
	cur_step_target_temp_text = 22,		//当前步目标温度
	cur_step_target_pressure_text = 23,	//当前步目标压力
	cur_step_duration_text = 24,		//当前步时长
	next_step_rate_text = 31,				//下一步速率
	next_step_target_temp_text = 32,		//下一步目标温度
	next_step_target_pressure_text = 33,	//下一步目标压力
	next_step_duration_text = 34,			//下一步时长
	exp_step_temp_unit_text = 40, 			//实验步段温度 单位
	exp_step_pressure_unit_text = 41,		//实验步段压力 单位
	exp_step_rate_unit_text = 42,			//实验步段速率 单位
	comm_status_text = 50,					//通讯状态
	exp_status_text = 51,					//实验状态
} TextID;


typedef enum {
	cooling_status_button = 100,				//制冷状态
	heating_status_button = 101,				//制热状态
	vacuum_pump_status_button = 102,			//真空泵状态
	prev_step_button = 110,				//上一步识别按键
	cur_step_button = 111,				//当前步识别按键
	next_step_button = 112,				//下一步识别按键   
} ButtonID;


/* 显示相关的函数集 */
typedef struct DisplayFunctionSets{
void (*DisPage)(uint16 Screen_id);										//切换页面
	
void (*PutString)(uint16 Screen_id, uint16 Control_id, char* string);	//显示文本
void (*PutString_uint16)(uint16 Screen_id, uint16 Control_id, int data);	//显示无符号16位整数
void (*PutString_int16)(uint16_t Screen_id, uint16_t Control_id, int data); 	//显示有符号16位整数
void (*PutString_uint32)(uint16 Screen_id, uint16 Control_id, int data);	//显示无符号32位整数
void (*PutString_float)(uint16 Screen_id, uint16 Control_id, float data);	//显示浮点数（小数点后1位）
void (*PutString_float2)(uint16_t Screen_id, uint16_t Control_id, float data);	//显示浮点数（小数点后2位）
void (*PutString_float_mbar)(uint16_t Screen_id, uint16_t Control_id, float data);	//显示浮点数（小数点后4位）

void (*DisButton)(uint16 screen_id,uint16 control_id,uint8 Status);	//显示按键

void (*Set_Color_On)(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back);
void (*Set_Color_Off)(uint16 screen_id, uint16 control_id, uint16 color_fore, uint16 color_back);

}DFS, *PDFS;



typedef struct LcdDeviceStatus {               	
    float rt_temp_c;			// 温度实时值
	float rt_temp_k;			// 压力实时值
	float rt_pressure_pa;
	float rt_pressure_mbar;
	uint8_t temp_set_flag;		//0：℃，1：K
	uint8_t pressure_set_flag;	//0：pa，1：mbar
	
#if 0 
    uint8_t temp_unit;                 		// 温度 单位
    uint8_t pressure_unit;             		// 压力 单位
    uint8_t rate_unit;						//速率 单位
    uint16_t prev_step_rate;              // 上一步速率
    uint16_t prev_step_target_temp;       // 上一步目标温度
    uint16_t prev_step_target_pressure;   // 上一步目标压力
    uint16_t prev_step_duration;          // 上一步时长
    uint16_t cur_step_rate;           // 当前步速率
    uint16_t cur_step_target_temp;    // 当前步目标温度
    uint16_t cur_step_target_pressure;// 当前步目标压力
    uint16_t cur_step_duration;       // 当前步时长
    uint16_t next_step_rate;              // 下一步速率
    uint16_t next_step_target_temp;       // 下一步目标温度
    uint16_t next_step_target_pressure;   // 下一步目标压力
    uint16_t next_step_duration;          // 下一步时长
    uint8_t comm_status;                  // 通讯状态
    uint8_t exp_status;                   // 实验状态
    bool cooling_status;				//制冷状态
	bool heating_status;				//制热状态
	bool vacuum_pump_status;			//真空泵状态
	bool prev_step_icon;				//上一步识别图标
	bool current_step_icon;				//当前步识别图标
	bool next_step_icon;				//下一步识别图标
#endif	
} LcdDeviceStatus, *PLcdDeviceStatus;


typedef struct LcdDevice{
	PLcdDeviceStatus status;		
	void (*Display)(void);
//	void (*Display_FromISR)(void);
}LcdDevice, *PLcdDevice;



extern LcdDevice lcd;


void Display(void);

void Display_FromISR(void);

void Init_Lcd_Device(void);


#endif	//__LCD_DEVICE_H

