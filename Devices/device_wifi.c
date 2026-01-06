#include "wifi_uart.h"
#include "wifi.h"
#include "modbus.h"
#include "rfid.h"
#include "lcd.h"
#include "gpio_config.h"
#include "report.h"
#include "lcd_uart.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "SEGGER_RTT.h"
#include "i2c_ee.h"
//#include "delay.h"



//新增
uint8_t Uart3_Rcv_Flag = 0;

uint8_t wifi_number = 0;

uint8_t wifi_scan_flag = 0;

char *wifi_ssid = 0;
char connect_wifi_ssid[50] = {0};


char wifi1_RSSI[MAX_RSSI_LENGTH];
char wifi1_SSID[MAX_SSID_LENGTH];
char wifi2_RSSI[MAX_RSSI_LENGTH];
char wifi2_SSID[MAX_SSID_LENGTH];
char wifi3_RSSI[MAX_RSSI_LENGTH];
char wifi3_SSID[MAX_SSID_LENGTH];
char wifi4_RSSI[MAX_RSSI_LENGTH];
char wifi4_SSID[MAX_SSID_LENGTH];
char wifi5_RSSI[MAX_RSSI_LENGTH];
char wifi5_SSID[MAX_SSID_LENGTH];
//uint8_t group = 0;

uint8_t Comma_Count = 0;
uint16_t Comma_Position[100];

char buffer[BUFFER_MAX_LEN];
char wifi_head_buff[WIFI_HEAD_LENGTH];
char wifi_msg_buff[WIFI_MSG_LENGTH];

uint8_t connect_state = 0;
//volatile uint8_t at_state = 0;
uint8_t at_state = 0;

uint8_t x = 0;

uint8_t wifi_start_test_flag = 0;  //联网版中，只要test_control=1; 就开始检测

extern uint8  flag3;

uint8_t pre_state = 0;
uint8_t cur_state = 0;

uint8_t start_set_ip = 0;
static uint8_t ip_state = 0;



void Wifi_Set_Info_Process(void)
{
	u8 i;
	u8 j;
	u16 fram_addr = 0;

	if(wifi_play_test_result == 1)
	{
		wifi_play_test_result = 0;
		voice_command = Modbus_data[19];
	}
	



	//获取WIFI使能气密封按钮信息
	if(wifi_set_airseal_start_test == 1)
	{
		air_seal_button_control = Modbus_data[15];
		wifi_set_airseal_start_test = 0;

		if(air_seal_button_control == on)		//只有air_seal_button_control=1，气密封按钮才有效
		{
			wifi_airseal_test_flag = 1;
		} else if(air_seal_button_control == off)
		{
			wifi_airseal_test_flag = 0;
			MOTOR_IN1_L; //关电机
			flag3 = 1;
			airseal_start_flag = 0; 	//恢复rfid更新
		}	
		
	}

	//获取WIFI开始测试信息
	if(wifi_start_test == 1)
	{
//		work_flag = Modbus_data[23];
		test_control = Modbus_data[17];
		wifi_start_test = 0;
	
		if(test_control == on)		//只要test_control=1，就开始检测
		{
			cur_state = 1;
			wifi_start_test_flag = 1;
		} else if(test_control == off)
		{
			pre_state = 0;
			wifi_start_test_flag = 0;
		}

/*		
		if(work_flag == 1)
		{
			
			//只有当获取到ID号时才可以启动测试
			if(rfid_detected == 1)
			{
				test_status = on;
				wifi_test_flag = 1;
				work_page = work_system_run_page;
				C_DisScreen(system_run_page);	
				play_mp3(start_test_sound);
				//清空曲线
				GraphChannelDataClear(work_system_run_page,graph_table,0);
				//添加曲线
				//GraphChannelAdd(work_system_run_page,graph_table,0,80);
			}
			else
			{
				play_mp3(no_rfid_sound);
				C_DisScreen(no_rfid_warning_dialog_box);
			}
		}
		else
		{
			C_DisScreen(main_page);
			work_page = work_main_page;
			work_status = work_idle;
			test_status = off;
			//关闭抽气继电器
			MOTOR_IN2_L;
		}
*/		
	}

	//获取WIFI气密封目标压力信息
	if(wifi_set_airseal_expected_pressure_flag == 1)
	{
		wifi_set_airseal_expected_pressure_flag = 0;
		set_airseal_expected_pressure = Modbus_data[12] * 256 + Modbus_data[13];
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;

		/*每次设置新的气密封目标压力值时，存入数组中*/
		mem_airseal_expected_value[0][0] = (set_airseal_expected_pressure >> 8) & 0xff;
		mem_airseal_expected_value[0][1] = set_airseal_expected_pressure & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_airseal_expected_value_base_addr; //初始化FRAM存储地址，指向存储气密封目标压力值的区域
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_airseal_expected_value[0][j]);
			fram_addr++;
		}
		
	}
	
	//获取WIFI充气目标压差信息
	if(wifi_set_expected_pressure_flag == 1)
	{
		wifi_set_expected_pressure_flag = 0;
		set_expected_pressure = Modbus_data[0] * 256 + Modbus_data[1];		
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;

		mem_expected_value[0][0] = (set_expected_pressure >> 8) & 0xff;
		mem_expected_value[0][1] = set_expected_pressure & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_expected_value_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_expected_value[0][j]);
			fram_addr++;
		}		
	}
	//获取WIFI稳压阶段允许压差信息
	if(wifi_set_stabilized_pressure_flag == 1)
	{
		wifi_set_stabilized_pressure_flag = 0;
		set_stabilized_tolerance_pressure = Modbus_data[10] * 256 + Modbus_data[11];
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;


		mem_stabilized_tolerance_value[0][0] = (set_stabilized_tolerance_pressure >> 8) & 0xff;
		mem_stabilized_tolerance_value[0][1] = set_stabilized_tolerance_pressure & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_stabilized_tolerance_value_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_stabilized_tolerance_value[0][j]);
			fram_addr++;
		}		
	}
	//获取WIFI保压测试允许压差信息
	if(wifi_set_tolerance_pressure_flag == 1)
	{
		wifi_set_tolerance_pressure_flag = 0;
		set_tolerance_pressure = Modbus_data[2] * 256 + Modbus_data[3];	
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;

		mem_tolerance_value[0][0] = (set_tolerance_pressure >> 8) & 0xff;
		mem_tolerance_value[0][1] = set_tolerance_pressure & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_tolerance_value_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_tolerance_value[0][j]);
			fram_addr++;
		}
		
	}
	//获取WIFI充气时间信息
	if(wifi_set_air_bleed_time_flag == 1)
	{
		wifi_set_air_bleed_time_flag = 0;
		set_air_bleed_time = Modbus_data[4] * 256 + Modbus_data[5];	
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;


		mem_air_bleed_time[0][0] = (set_air_bleed_time >> 8) & 0xff;
		mem_air_bleed_time[0][1] = set_air_bleed_time & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_air_bleed_time_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_air_bleed_time[0][j]);
			fram_addr++;
		}
	}		
	//获取WIFI稳压时间信息
	if(wifi_set_stabilized_pressure_time_flag == 1)
	{
		wifi_set_stabilized_pressure_time_flag = 0;
		set_stabilized_pressure_time = Modbus_data[6] * 256 + Modbus_data[7];		
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;


		mem_stabilized_pressure_time[0][0] = (set_stabilized_pressure_time >> 8) & 0xff;
		mem_stabilized_pressure_time[0][1] = set_stabilized_pressure_time & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_stabilized_pressure_time_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_stabilized_pressure_time[0][j]);
			fram_addr++;
		}
	}		
	//获取WIFI设置保压时间信息
	if(wifi_set_hold_pressure_time_flag == 1)
	{
		wifi_set_hold_pressure_time_flag = 0;
		set_hold_pressure_time = Modbus_data[8] * 256 + Modbus_data[9];
		C_DisScreen(parameter_page);
		work_page = work_parameter_page;


		mem_hold_pressure_time[0][0] = (set_hold_pressure_time >> 8) & 0xff;
		mem_hold_pressure_time[0][1] = set_hold_pressure_time & 0xff;
/*写入非易失性存储器FRAM*/					
		fram_addr = mem_hold_pressure_time_base_addr; 
		for(j=0;j<2;j++)
		{
			TWI_WriteByte(fram_addr,mem_hold_pressure_time[0][j]);
			fram_addr++;
		}
	}


/*
	//获取WIFI打印报告信息
	if(wifi_print_flag == 1)
	{
		C_DisScreen(report_page);	
		work_page = work_report_page;
		wifi_print_flag = 0;		
		if(Modbus_data[25] >= 1 && Modbus_data[25] < 6)
		{
			if(BLE_LINK == 0)
			{
				report_page_index = 1;
				//Print_Record_Report(Modbus_data[25]);				
			}	
		}
		else if(Modbus_data[25] >= 6 && Modbus_data[25] < 11)
		{
			if(BLE_LINK == 0)
			{
				report_page_index = 2;
				//Print_Record_Report(Modbus_data[25] - 5);				
			}	
		}
		else if(Modbus_data[25] >= 11 && Modbus_data[25] < 16)
		{
			if(BLE_LINK == 0)
			{
				report_page_index = 3;
				//Print_Record_Report(Modbus_data[25] - 10);				
			}	
		}
		else if(Modbus_data[25] >= 16 && Modbus_data[25] < 21)
		{
			if(BLE_LINK == 0)
			{
				report_page_index = 4;
				//Print_Record_Report(Modbus_data[25] - 15);				
			}	
		}
	}	
*/	
}

void wifi_set10_info_process(void)  //modbus 0x10参数写入
{
	u8 i;
	u8 j;
	u16 fram_addr = 0;

	voice_command = Modbus_data[18] * 256 + Modbus_data[19];


	////////////获取WIFI充气目标压差信息///////////////////////////////
		set_expected_pressure = Modbus_data[0] * 256 + Modbus_data[1];
//		mem_expected_value[0][0] = (set_expected_pressure >> 8) & 0xff;
//		mem_expected_value[0][1] = set_expected_pressure & 0xff;
//     /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_expected_value_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_expected_value[0][j]);
//			fram_addr++;
//		}		
  //////////////////////////////////////////////////////////////////
		
	////////////获取WIFI保压测试允许压差信息///////////////////////////
	set_tolerance_pressure = Modbus_data[2] * 256 + Modbus_data[3];	
//	mem_tolerance_value[0][0] = (set_tolerance_pressure >> 8) & 0xff;
//	mem_tolerance_value[0][1] = set_tolerance_pressure & 0xff;
//   /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_tolerance_value_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_tolerance_value[0][j]);
//			fram_addr++;
//		}
  //////////////////////////////////////////////////////////////////		

	////////////获取WIFI充气时间信息////////////////////////////////////
		set_air_bleed_time = Modbus_data[4] * 256 + Modbus_data[5];	
//		mem_air_bleed_time[0][0] = (set_air_bleed_time >> 8) & 0xff;
//		mem_air_bleed_time[0][1] = set_air_bleed_time & 0xff;
//     /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_air_bleed_time_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_air_bleed_time[0][j]);
//			fram_addr++;
//		}
  ////////////////////////////////////////////////////////////////////////	

  ////////////获取WIFI稳压时间信息////////////////////////////////////////
		set_stabilized_pressure_time = Modbus_data[6] * 256 + Modbus_data[7];		
//		mem_stabilized_pressure_time[0][0] = (set_stabilized_pressure_time >> 8) & 0xff;
//		mem_stabilized_pressure_time[0][1] = set_stabilized_pressure_time & 0xff;
//      /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_stabilized_pressure_time_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_stabilized_pressure_time[0][j]);
//			fram_addr++;
//		}
   ////////////////////////////////////////////////////////////////////////	

	////////////获取WIFI设置保压时间信息//////////////////////////////////////
		set_hold_pressure_time = Modbus_data[8] * 256 + Modbus_data[9];
//		mem_hold_pressure_time[0][0] = (set_hold_pressure_time >> 8) & 0xff;
//		mem_hold_pressure_time[0][1] = set_hold_pressure_time & 0xff;
//     /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_hold_pressure_time_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_hold_pressure_time[0][j]);
//			fram_addr++;
//		}
   ////////////////////////////////////////////////////////////////////////	
	
	////////////获取WIFI稳压阶段允许压差信息////////////////////////////////////
		set_stabilized_tolerance_pressure = Modbus_data[10] * 256 + Modbus_data[11];
//		mem_stabilized_tolerance_value[0][0] = (set_stabilized_tolerance_pressure >> 8) & 0xff;
//		mem_stabilized_tolerance_value[0][1] = set_stabilized_tolerance_pressure & 0xff;
//    /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_stabilized_tolerance_value_base_addr; 
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_stabilized_tolerance_value[0][j]);
//			fram_addr++;
//		}		
	//////////////////////////////////////////////////////////////////////////////////
		
	////////////获取WIFI气密封目标压力信息////////////////////////////////////////////
		set_airseal_expected_pressure = Modbus_data[12] * 256 + Modbus_data[13];
//		/*每次设置新的气密封目标压力值时，存入数组中*/
//		mem_airseal_expected_value[0][0] = (set_airseal_expected_pressure >> 8) & 0xff;
//		mem_airseal_expected_value[0][1] = set_airseal_expected_pressure & 0xff;
//    /*写入非易失性存储器FRAM*/					
//		fram_addr = mem_airseal_expected_value_base_addr; //初始化FRAM存储地址，指向存储气密封目标压力值的区域
//		for(j=0;j<2;j++)
//		{
//			TWI_WriteByte(fram_addr,mem_airseal_expected_value[0][j]);
//			fram_addr++;
//		}
		
	//////////////////////////////////////////////////////////////////////////////////
		
	////////////获取WIFI使能气密封按钮信息////////////////////////////////////////////////////////////
		air_seal_button_control = Modbus_data[15];
		wifi_set_airseal_start_test = 0;

		if(air_seal_button_control == on)		//只有air_seal_button_control=1，气密封按钮才有效
		{
			wifi_airseal_test_flag = 1;
		} else if(air_seal_button_control == off)
		{
			wifi_airseal_test_flag = 0;
			MOTOR_IN1_L; //关电机
			flag3 = 1;
			airseal_start_flag = 0; 	//恢复rfid更新	
		}		
	//////////////////////////////////////////////////////////////////////////////////	
		
	/////////获取WIFI开始测试信息///////////////////////////////////////////////////
		test_control = Modbus_data[17];
	
		if(test_control == on)		//只要test_control=1，就开始检测
		{
			wifi_start_test_flag = 1;
		} 
		else if(test_control == off)
		{
			wifi_start_test_flag = 0;
		}
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		
//		memset(Modbus_buff, 0, sizeof(Modbus_buff));
//		LAN_RX_INDEX = 0;
}



void Scan_Wifi(void)
{
	uint8_t i = 0;
	char* start;
/*
	if(at_state == 0)
	{
		memset(Uart3_buff1, 0, sizeof(Uart3_buff1));
		Uart3_Rx_Index1 = 0;		
		Send_AT("AT+ENTM\r");
		timer_cnt1 = 1;
		at_state = 1;
	}
*/	
	if(at_state == 0)
	{
		Clr_Uart3_buff1();
		Send_AT("+++");
		timer_cnt4 = 6;
		//delay_ms(500);
		

		if(!(strncmp(Uart3_buff1, "a", 1) == 0)) 
		{
			//输出错误信息：没有收到a
			SEGGER_RTT_printf(0,"err1:do not receive a, please power it on again. \n");
		}			
		
		while (!(strncmp(Uart3_buff1, "a", 1) == 0) && (timer_cnt4 != 0)) 
		{
			Clr_Uart3_buff1();
			//没有收到a，一直发送+++
			Send_AT("+++");
			delay_ms(500);
		}

		Clr_Uart3_buff1();
		
		Send_AT("a");
		timer_flag1 = 0;
		timer_cnt1 = 1;
		at_state = 1;
	}	
	
	if(at_state == 1)
	{	
		//delay_ms(500);
		if(timer_flag1 == 1)
		{
//			timer_flag1 = 0;
			Clr_Uart3_buff1();
			at_state = 2;
		}
	}

/*		
	if(at_state == 2)
	{
		Send_AT("AT+E\r");
		timer_flag1 = 0;
		timer_cnt1 = 3;  //延时1.5s
		timer_cnt4 = 8;
		at_state = 3;
	}
	
	if(at_state == 3)
	{	
		//delay_ms(500);
		if(timer_flag1 == 1)
		{
//			timer_flag1 = 0;
			if(!(Uart3_buff1[0] == 0x41 && Uart3_buff1[1] == 0x54 && Uart3_buff1[8] == 0x6B && Uart3_buff1[11] == 0x0D && Uart3_buff1[12] == 0x0A)) 
			{
				//输出错误信息：关闭回显失败
				SEGGER_RTT_printf(0,"err2: Failed to disable the echo display, please power it on again. \n");
			}
//			memset(Uart3_buff1, 0, sizeof(Uart3_buff1));
//			Uart3_Rx_Index1 = 0;
//			at_state = 4;
			
			while ((strncmp(Uart3_buff1, "AT+E\n\r+ok", 9) != 0) && (timer_cnt4 != 0)) 
			{
				memset(Uart3_buff1, 0, sizeof(Uart3_buff1));
				Uart3_Rx_Index1 = 0;
				Send_AT("AT+E\r");
				delay_ms(500);
			}
			memset(Uart3_buff1, 0, sizeof(Uart3_buff1));
			Uart3_Rx_Index1 = 0;
			at_state = 4;						
		}
	}
*/
	
	if(at_state == 2)
	{
		Send_AT("AT+WMODE=STA\r");
		timer_flag1 = 0;
		timer_cnt1 = 1;
		timer_cnt4 = 6;
		at_state = 3;
	}
		
	if(at_state == 3)
	{
		//delay_ms(500);
		if(timer_flag1 == 1)
		{
			timer_flag1 = 0;
			if (Uart3_Rx_Index1>=6)
			{
				//继续执行
			}
			else
			{
				while (!(strncmp(Uart3_buff1, "AT+WMODE=STA\n\r+ok", 16) == 0) && (timer_cnt4!=0))
				{
					Clr_Uart3_buff1();
					//没有进入STA模式，重新发送AT+WMODE=STA\r
					Send_AT("AT+WMODE=STA\r");
					delay_ms(500);
				}		
			}
			Clr_Uart3_buff1();
			at_state = 4;
		}
	}	
				
	if(at_state == 4)
	{
		Send_AT("AT+WSCAN\r");
		timer_cnt3 = 8;
		at_state = 5;
	}
	
	if(at_state == 5)
	{
		//delay_ms(1000);
		//delay_ms(1000);
		//delay_ms(1000);
		if(timer_flag3 == 1)
		{
			timer_flag3 = 0;
			if(Uart3_Rx_Index1 >= 66)  //收到包头
			{
				//扫描wifi成功，继续执行
			}
			else
			{				
				//输出错误信息：wifi扫描失败
				SEGGER_RTT_printf(0,"err3: wifiscan is unsuccessful, please power it on again.\n");
			}

			
			start = strstr(Uart3_buff1, "AT+WSCAN");
			if (start != NULL)
			{
				strncpy(buffer, start, BUFFER_MAX_LEN - 1);
				buffer[BUFFER_MAX_LEN - 1] = '\0';		
			} else{
				buffer[0] = '\0';
			}	
			
			
			/*分割数组*/
			memcpy(wifi_head_buff, buffer, WIFI_HEAD_LENGTH);
			memcpy(wifi_msg_buff, buffer + WIFI_HEAD_LENGTH, WIFI_MSG_LENGTH);

			Clr_Uart3_buff1();
			memset(buffer, 0, sizeof(buffer));
			at_state = 6;
		}
	}
	
	if(at_state == 6)
	{	
		/*解析wifi信息*/
		Pre_Process();
		switch (Uart3_Rcv_Flag) 
		{
		case 0:
			ProcessData0();
			Uart3_Rcv_Flag = 0;
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));
			break;
		case 1:
			ProcessData1();
			Uart3_Rcv_Flag = 0;
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));			
			break;
		case 2:
			ProcessData2();
			Uart3_Rcv_Flag = 0;
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));			
			break;
		case 3:
			ProcessData3();
			Uart3_Rcv_Flag = 0;
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));			
			break;
		case 4:
			ProcessData4();
			Uart3_Rcv_Flag = 0;
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));			
			break;
		case 5:
			ProcessData5();
			Uart3_Rcv_Flag = 0;
			//输出信息：wifi扫描成功
			SEGGER_RTT_printf(0,"scan success!\r\n");
			//delay_ms(1000);
			memset(wifi_msg_buff, 0, sizeof(wifi_msg_buff));
			memset(wifi_head_buff, 0, sizeof(wifi_head_buff));			
			break;
		}
		
		//解析完数据，就要把Comma_Position[100]清零
		Comma_Count = 0;
		for(i=0; i<100; i++)
		{
			Comma_Position[i] = 0;		
		}
		
		at_state = 7;
		timer_cnt5 = 2;
		
		C_DisScreen(wifi_msg_page);
		
		if(wifi1_RSSI[0] == '\0' && wifi2_RSSI[0] == '\0' && wifi3_RSSI[0] == '\0') 	//如果扫描失败，就退出wifi热点列表
		{
			at_state = 0;
			C_DisScreen(parameter_page);
			
			timer_flag5 = 0;
			timer_cnt5 = 0;
			start_wifi_scan = 0;
			wifi_scan_flag = 0;
			
			memset(wifi1_SSID, 0, sizeof(wifi1_SSID));
			memset(wifi2_SSID, 0, sizeof(wifi2_SSID));
			memset(wifi3_SSID, 0, sizeof(wifi3_SSID));
			memset(wifi4_SSID, 0, sizeof(wifi4_SSID));
			memset(wifi5_SSID, 0, sizeof(wifi5_SSID));
			memset(wifi1_RSSI, 0, sizeof(wifi1_RSSI));
			memset(wifi2_RSSI, 0, sizeof(wifi2_RSSI));
			memset(wifi3_RSSI, 0, sizeof(wifi3_RSSI));
			memset(wifi4_RSSI, 0, sizeof(wifi4_RSSI));
			memset(wifi5_RSSI, 0, sizeof(wifi5_RSSI));
			
			work_page = work_parameter_page;
		}
		
	}
	
	if(at_state == 7)
	{
		if(timer_flag5 == 1)
		{
			timer_flag5 = 0;
//			at_state = 0;
		}
	}

/*		
	if(at_state == 10)
	{		
//		get_wifi_number();
//		Connect_Wifi(x);
	 
		Connect_Wifi2(1);
	}	
*/
}


uint8_t Pre_Process(void)
{	
	uint8_t j = 0;
	uint16_t i=0;
	
	for(i=0; i<WIFI_MSG_LENGTH; i++)   //统计comma的数量和位置
	{
		char Current_Char = wifi_msg_buff[i];
		if (Current_Char == 0x2c) //表示','
		{
			Comma_Position[j] = i;
			j++;
			Comma_Count++;
		}
	}
	
	if (Comma_Count < 6) //周围没有wifi信号
	{
		Uart3_Rcv_Flag = 0;
	}
	else if (Comma_Count == 6) //接收到1个wifi信号
	{
		Uart3_Rcv_Flag = 1;
	}
	else if (Comma_Count == 12) //接收到2个wifi信号
	{
		Uart3_Rcv_Flag = 2;
	}
	else if (Comma_Count == 18) //接收到3个wifi信号或者超出接收缓冲区大小
	{
		Uart3_Rcv_Flag = 3;
	}
	else if (Comma_Count == 24) //接收到4个wifi信号或者超出接收缓冲区大小
	{
		Uart3_Rcv_Flag = 4;
	}
	else if (Comma_Count >= 30 || Uart3_Rx_Index1 >= UART3_MAX_LEN - 1) //接收到5个wifi信号或者超出接收缓冲区大小
	{
		Uart3_Rcv_Flag = 5;
	}
	
	return Uart3_Rcv_Flag;
}


void ProcessData0(void)
{
	//输出错误信息：周围没有wifi信号
	SEGGER_RTT_printf(0,"err4:ProcessData0, no wifi signal around.\n");
}

void ProcessData1(void)
{
	uint16_t startIndex = 0;
	uint16_t endIndex = 0;
    uint8_t RSSI_Index = 0;
    uint8_t SSID_Index = 0;
	
	uint16_t i = 0;
	uint16_t k = 0;

	uint8_t group = 0;

	startIndex = Comma_Position[0];
	endIndex = Comma_Position[1];
	
	// 处理wifi1_RSSI	
	for (k = 0; k < startIndex; k++) 
	{
		char Current_Char = wifi_msg_buff[k];
		if (Current_Char == 0x2c) 
		{
			break; // 找到第一个逗号，停止处理
		}
		wifi1_RSSI[RSSI_Index++] = Current_Char;
	}
	wifi1_RSSI[RSSI_Index] = '\0';
	
	
	// 处理wifi1_SSID
	for (i = startIndex + 1; i < endIndex; i++) 
	{
		char Current_Char = wifi_msg_buff[i];
		if (Current_Char == 0x2c) 
		{
			break; // 找到第一个逗号，停止处理
		}
		wifi1_SSID[SSID_Index++] = Current_Char;
	}
	if (SSID_Index == 0) 
	{
    wifi1_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
	}
	wifi1_SSID[SSID_Index] = '\0';
}


void ProcessData2(void)
{
    uint8_t RSSI_Index = 0;
    uint8_t SSID_Index = 0;

	uint16_t startIndex = 0;
	uint16_t endIndex = 0;
	uint16_t RSSI_start_index = 0;

	uint16_t i = 0;
	uint16_t k = 0;

	uint8_t group = 0;
	
	if(group == 0)
	{
		startIndex = Comma_Position[0];
		endIndex = Comma_Position[1];

		// 处理wifi1_RSSI	
		for (k = 0; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi1_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi1_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi1_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi1_SSID[SSID_Index] = '\0';

		group = 1;
		RSSI_Index = 0;
		SSID_Index = 0;		
	}
	else if(group == 1)
	{
		RSSI_start_index = Comma_Position[5];
		startIndex = Comma_Position[6];
		endIndex = Comma_Position[7];

		// 处理wifi2_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi2_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi2_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi2_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi2_SSID[SSID_Index] = '\0';
		group = 0;	
	}
}

void ProcessData3(void)
{
    uint8_t RSSI_Index = 0;
    uint8_t SSID_Index = 0;


	uint16_t startIndex = 0;
	uint16_t endIndex = 0;
	
	uint16_t RSSI_start_index = 0;

	uint16_t i = 0;
	uint16_t k = 0;

	uint8_t group = 0;
	
	if(group == 0)
	{
		startIndex = Comma_Position[0];
		endIndex = Comma_Position[1];

		// 处理wifi1_RSSI	
		for (k = 0; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi1_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi1_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi1_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi1_SSID[SSID_Index] = '\0';

		group = 1;
		RSSI_Index = 0;
		SSID_Index = 0;
	}
	if(group == 1)
	{
		RSSI_start_index = Comma_Position[5];
		startIndex = Comma_Position[6];
		endIndex = Comma_Position[7];

		// 处理wifi2_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi2_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi2_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi2_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi2_SSID[SSID_Index] = '\0';
		
		group = 2;
		RSSI_Index = 0;
		SSID_Index = 0;		
	}
	if(group == 2)
	{
		RSSI_start_index = Comma_Position[11];
		startIndex = Comma_Position[12];
		endIndex = Comma_Position[13];

		// 处理wifi3_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++)  
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi3_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi3_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi3_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi3_SSID[SSID_Index] = '\0';
		group = 0;	
	}
}

void ProcessData4(void)
{
    uint8_t RSSI_Index = 0;
    uint8_t SSID_Index = 0;


	uint16_t startIndex = 0;
	uint16_t endIndex = 0;
	
	uint16_t RSSI_start_index = 0;
	
	uint16_t i = 0;
	uint16_t k = 0;

	uint8_t group = 0;

	if(group == 0)
	{
		startIndex = Comma_Position[0];
		endIndex = Comma_Position[1];

		// 处理wifi1_RSSI	
		for (k = 0; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi1_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi1_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi1_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi1_SSID[SSID_Index] = '\0';

		group = 1;
		RSSI_Index = 0;
		SSID_Index = 0;
	}
	if(group == 1)
	{
		RSSI_start_index = Comma_Position[5];
		startIndex = Comma_Position[6];
		endIndex = Comma_Position[7];

		// 处理wifi2_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi2_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi2_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi2_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi2_SSID[SSID_Index] = '\0';
		
		group = 2;
		RSSI_Index = 0;
		SSID_Index = 0;		
	}
	if(group == 2)
	{
		RSSI_start_index = Comma_Position[11];
		startIndex = Comma_Position[12];
		endIndex = Comma_Position[13];

		// 处理wifi3_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++)
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi3_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi3_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi3_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi3_SSID[SSID_Index] = '\0';
		group = 3;
		RSSI_Index = 0;
		SSID_Index = 0;	
	}

	if(group == 3)
	{
		RSSI_start_index = Comma_Position[17];
		startIndex = Comma_Position[18];
		endIndex = Comma_Position[19];

		// 处理wifi4_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi4_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi4_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi4_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi4_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi4_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi4_SSID[SSID_Index] = '\0';
		group = 0;	
	}
}

void ProcessData5(void)
{
    uint8_t RSSI_Index = 0;
    uint8_t SSID_Index = 0;


	uint16_t startIndex = 0;
	uint16_t endIndex = 0;
	
	uint16_t RSSI_start_index = 0;

	uint16_t i = 0;
	uint16_t k = 0;

	uint8_t group = 0;
	
	if(group == 0)
	{
		startIndex = Comma_Position[0];
		endIndex = Comma_Position[1];

		// 处理wifi1_RSSI	
		for (k = 0; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi1_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi1_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi1_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi1_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi1_SSID[SSID_Index] = '\0';

		group = 1;
		RSSI_Index = 0;
		SSID_Index = 0;
	}
	if(group == 1)
	{
		RSSI_start_index = Comma_Position[5];
		startIndex = Comma_Position[6];
		endIndex = Comma_Position[7];

		// 处理wifi2_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi2_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi2_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi2_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi2_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi2_SSID[SSID_Index] = '\0';
		
		group = 2;
		RSSI_Index = 0;
		SSID_Index = 0;		
	}
	if(group == 2)
	{
		RSSI_start_index = Comma_Position[11];
		startIndex = Comma_Position[12];
		endIndex = Comma_Position[13];

		// 处理wifi3_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++)
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi3_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi3_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi3_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi3_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi3_SSID[SSID_Index] = '\0';
		group = 3;
		RSSI_Index = 0;
		SSID_Index = 0;			
	}

	if(group == 3)
	{
		RSSI_start_index = Comma_Position[17];
		startIndex = Comma_Position[18];
		endIndex = Comma_Position[19];

		// 处理wifi4_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++)
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi4_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi4_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi4_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi4_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi4_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi4_SSID[SSID_Index] = '\0';
		group = 4;
		RSSI_Index = 0;
		SSID_Index = 0;			
	}

	if(group == 4)
	{
		RSSI_start_index = Comma_Position[23];
		startIndex = Comma_Position[24];
		endIndex = Comma_Position[25];

		// 处理wifi5_RSSI	
		for (k = RSSI_start_index + 3; k < startIndex; k++) 
		{
			char Current_Char = wifi_msg_buff[k];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi5_RSSI[RSSI_Index++] = Current_Char;
		}
		wifi5_RSSI[RSSI_Index] = '\0';
		
		
		// 处理wifi5_SSID
		for (i = startIndex + 1; i < endIndex; i++) 
		{
			char Current_Char = wifi_msg_buff[i];
			if (Current_Char == 0x2c) 
			{
				break; // 找到第一个逗号，停止处理
			}
			wifi5_SSID[SSID_Index++] = Current_Char;
		}
		if (SSID_Index == 0) 
		{
			wifi5_SSID[SSID_Index++] = 0x20;  // 只在没有内容时插入空格
		}
		wifi5_SSID[SSID_Index] = '\0';
		group = 0;	
	}
}


/*
uint8_t get_wifi_number(void)
{
	uint8_t x = 0;
	//获取要连接wifi的序号
	
	return x;
}
*/

void Connect_Wifi( uint8_t x ) //连接wifi
{
	uint16_t fram_addr;
	uint8_t i = 0;
	uint8_t j = 0;

//	char *wifi_ssid = 0;
	char atCommand[70];
//	char r[5];
	static int8_t at_state2 = 0;
	char expectedResponse[100];
	
	C_DisScreen(wifi_connecting_page);
	
	switch (x) 
	{
		case 1:
			wifi_ssid = wifi1_SSID;
			break;
		case 2:
			wifi_ssid = wifi2_SSID;
			break;
		case 3:
			wifi_ssid = wifi3_SSID;
			break;
		case 4:
			wifi_ssid = wifi4_SSID;
			break;
		case 5:
			wifi_ssid = wifi5_SSID;
			break;
	}

	
	//将AT指令拼接到atCommand字符串中
//	snprintf(atCommand, sizeof(atCommand), "AT+WSSSID=%s\r", wifi_ssid);
//	SEGGER_RTT_printf(0,"%s\n", atCommand);

	
	switch (at_state2)
	{
		case 0:
			for(j=0; j<70; j++)
			{
				atCommand[j] = 0;
			}
		
			//将AT指令拼接到atCommand字符串中
			snprintf(atCommand, sizeof(atCommand), "AT+WSSSID=%s\r", wifi_ssid);
			snprintf(expectedResponse, sizeof(expectedResponse), "%s\n\r+ok", atCommand);
			SEGGER_RTT_printf(0,"%s\n", atCommand);
			Send_AT(atCommand);
			timer_cnt2 = 2;
			timer_cnt4 = 8;
			at_state2 = 1;
			//Send_AT("AT+WSSSID=lxw\r");
			//Send_AT("AT+WSSSID=ChinaNet-bD4ssF\r");
			break;

		case 1:
			//delay_ms(1000);
			if(timer_flag2 == 1)
			{
				timer_flag2 = 0;
				if (Uart3_Rx_Index1>=6)
				{
					//继续执行
				}
				else
				{
					while (!(strncmp(Uart3_buff1, expectedResponse, strlen(expectedResponse)) == 0) && (timer_cnt4!=0))				
					{
						Clr_Uart3_buff1();
						//没有回复，重新发送atCommand
						Send_AT(atCommand);
						delay_ms(500);
					}
				}
				Clr_Uart3_buff1();
				at_state2 = 2;
			}
			break;
		
		case 2:
			Send_AT("AT+WSKEY=WPA2PSK,AES,12345678\r");
			//Send_AT("AT+WSKEY=WPA2PSK,AES,usch4695\r");
			timer_cnt1 = 1;
			timer_cnt4 = 6;
			at_state2 = 3;
			break;

		case 3:
			//delay_ms(500);
			if(timer_flag1 ==1)
			{
				timer_flag1 = 0;
				if (Uart3_Rx_Index1>=6)
				{
					//继续执行
				}
				else
				{
					while (!(strncmp(Uart3_buff1, "AT+WSKEY=WPA2PSK,AES,12345678\r", 30) == 0) && (timer_cnt4!=0))
					{
						//没有回复，重新发送"AT+WSKEY=WPA2PSK,AES,12345678\r"
						Send_AT("AT+WSKEY=WPA2PSK,AES,12345678\r");
						//Send_AT("AT+WSKEY=WPA2PSK,AES,usch4695\r");
						delay_ms(500);
					}
				}
				Clr_Uart3_buff1();
				at_state2 = 4;
			}
			break;
	
		case 4:
			Send_AT("AT+Z\r");
			timer_cnt3 = 36;	//等18s
			at_state2 = 5;
			break;

		case 5:
			//delay_ms(1000);
			//delay_ms(1000);
			//delay_ms(1000);
			if(timer_flag3 == 1)
			{
//				timer_flag3 = 0;
				Clr_Uart3_buff1();

				
				if(WIFI_LINK == 0)
				{
					//连接成功
					connect_state = 1;
					Send_AT("AT+ENTM\r");	//进入透传模式
					at_state_flag = 1;
					timer_cnt1 = 2;
					at_state2 = 6;
				} else{
					//连接失败
					connect_state = 0;
					at_state2 = 0;
					at_state_flag = 1;

					//一次连接完成，无论成功还是失败，都清除wifi_SSID和wifi_RSSI数组			
					memset(wifi1_SSID, 0, sizeof(wifi1_SSID));
					memset(wifi2_SSID, 0, sizeof(wifi2_SSID));
					memset(wifi3_SSID, 0, sizeof(wifi3_SSID));
					memset(wifi4_SSID, 0, sizeof(wifi4_SSID));
					memset(wifi5_SSID, 0, sizeof(wifi5_SSID));
					memset(wifi1_RSSI, 0, sizeof(wifi1_RSSI));
					memset(wifi2_RSSI, 0, sizeof(wifi2_RSSI));
					memset(wifi3_RSSI, 0, sizeof(wifi3_RSSI));
					memset(wifi4_RSSI, 0, sizeof(wifi4_RSSI));
					memset(wifi5_RSSI, 0, sizeof(wifi5_RSSI));
					//同时把wifi扫描的标志位清零。wifi连接的wifi_number清零。wifi扫描的状态变量at_state清零。
					start_wifi_scan = 0;
					wifi_scan_flag = 0;
					wifi_number = 0;
					at_state = 0;
					timer_flag3 = 0;

					C_DisScreen(parameter_page);
					work_page = work_parameter_page;
				}
				//return connect_state;
			}
			break;
			
		case 6:
			if(timer_flag1 == 1)
			{
//					at_state_flag = 1;
//				at_state2 = 0;
//					at_state = 11;

				C_PutString2(system_set_page, connect_wifi_ssid_text, wifi_ssid);

				strcpy(connect_wifi_ssid,wifi_ssid);
				fram_addr = connected_wifi_ssid_addr; 
				for(i=0; i<50; i++)
				{
					TWI_WriteByte(fram_addr,connect_wifi_ssid[i]);
					fram_addr++;
				}	

				for(i=0;i<50;i++)
				{
					connect_wifi_ssid[i] = 0;
				}

				
				//一次连接完成，无论成功还是失败，都清除wifi_SSID和wifi_RSSI数组			
				memset(wifi1_SSID, 0, sizeof(wifi1_SSID));
				memset(wifi2_SSID, 0, sizeof(wifi2_SSID));
				memset(wifi3_SSID, 0, sizeof(wifi3_SSID));
				memset(wifi4_SSID, 0, sizeof(wifi4_SSID));
				memset(wifi5_SSID, 0, sizeof(wifi5_SSID));
				memset(wifi1_RSSI, 0, sizeof(wifi1_RSSI));
				memset(wifi2_RSSI, 0, sizeof(wifi2_RSSI));
				memset(wifi3_RSSI, 0, sizeof(wifi3_RSSI));
				memset(wifi4_RSSI, 0, sizeof(wifi4_RSSI));
				memset(wifi5_RSSI, 0, sizeof(wifi5_RSSI));


				//同时把wifi扫描的标志位清零。wifi连接的wifi_number清零。wifi扫描的状态变量at_state清零。
				start_wifi_scan = 0;
				wifi_scan_flag = 0;
				wifi_number = 0;
				at_state = 0;
				timer_flag3 = 0;
				
				timer_flag1 = 0;
//				timer_flag6 = 0;
				at_state2 = 0;
				C_DisScreen(parameter_page);
				work_page = work_parameter_page;

			}
			break;
			
		default:
			break;
	}
}


void Set_IP_Address(const char* ip_address, uint16_t port)
{
	static uint8_t ip_at_state = 0;
 	static char command[50] = {0};	

	switch (ip_at_state)
	{
/* 进入AT指令模式："+++"、"a"; */
		case 0:
		{
            Clr_Uart3_buff1();
            Send_AT("+++");
            ip_timer_cnt1 = 6;		

			while (!(strncmp(Uart3_buff1, "a", 1) == 0) && (ip_timer_cnt1 != 0))
			{
				Clr_Uart3_buff1();
				Send_AT("+++");
				delay_ms(500);
			}

            Clr_Uart3_buff1();
            Send_AT("a");			

			ip_timer_cnt1 = 6;
			ip_timer_flag2 = 0;
			ip_timer_cnt2 = 1;
			
			ip_at_state = 1;
			break;
		}
		case 1:
		{
			if (ip_timer_flag2 == 1)
			{
				Clr_Uart3_buff1();
				ip_at_state = 2;
			}
			break;
		}	

        /* 设置端口 */
        case 2:
        {
            snprintf(command, sizeof(command), "AT+NETP=TCP,Server,%d,10.10.100.100\r", port);
            Send_AT(command);
            ip_timer_flag2 = 0;
            ip_timer_cnt2 = 3; // 延时1.5秒

            ip_at_state = 3;
            break;
        }

		
/* 设置ip：AT+LANN=10.10.100.254,255.255.255.0 */		
		case 3:
		{
			if (ip_timer_flag2 == 1)
			{		
				snprintf(command, sizeof(command), "AT+LANN=%s,255.255.255.0\r", ip_address);
				Send_AT(command);
				ip_timer_flag2 = 0;
				ip_timer_cnt2 = 3; // 延时1.5秒

				ip_at_state = 4;
			}	
			break;
		}
		
/* 重启wifi模块：AT+Z */		
		case 4:
		{
			if (ip_timer_flag2 == 1)
			{
				snprintf(command, sizeof(command), "AT+LANN=%s,255.255.255.0\n\r+ok", ip_address);
//				 if (strncmp(Uart3_buff1, command, strlen(command)) == 0)
				if (strstr(Uart3_buff1, command) != NULL)
				{
					Clr_Uart3_buff1();
					Send_AT("AT+Z\r");
					ip_timer_flag2 = 0;
					ip_timer_cnt2 = 16;	//等待8s
					ip_at_state = 5;


                    // 保存 IP 和端口到 FRAM
                    uint16_t fram_addr = IP_FRAM_ADDR;
                    for (uint8_t i = 0; i < 50; i++)
                    {
                        if (i < strlen(ip_address))
                            TWI_WriteByte(fram_addr, ip_address[i]);
                        else
                            TWI_WriteByte(fram_addr, 0); // 如果 IP 地址不足 50 字节，用 0 填充
                        fram_addr++;
                    }

                    fram_addr = PORT_FRAM_ADDR;
                    TWI_WriteByte(fram_addr, (port >> 8) & 0xFF); // 高字节
                    fram_addr++;
                    TWI_WriteByte(fram_addr, port & 0xFF); // 低字节
					
				} else
                {
                    // 错误处理
                    SEGGER_RTT_printf(0, "Error: IP address setting failed.\n");
                    ip_at_state = 0; // 重置状态机
                    start_set_ip = 0;
					ip_state = 0;
					C_DisScreen(system_set_page);
					work_page = work_system_set_page;	

					at_state_flag = 1;		//wifi模块重新进入透传模式
                }				
			}
			break;
		}
		case 5:
		{
			if (ip_timer_flag2 == 1)
			{
				ip_timer_flag2 = 0;
				
				C_DisScreen(system_set_page);
				work_page = work_system_set_page;
				ip_at_state = 0;

				start_set_ip = 0;	
				ip_state = 0;

				at_state_flag = 1;		//wifi模块重新进入透传模式
			}		
			break;
		}		
		default:
			break;
	}
}




void ReadIPAndPortFromControls(void)
{
	//先读一次，确保ip_part1_text能够读到完整准确的值
	GetControlValue(system_set_page, ip_part1_text);
	delay_ms(100);
	
    // 读取IP地址的每个部分
    GetControlValue(system_set_page, ip_part1_text);
    delay_ms(800);
    ip_part1 = atoi(LCD_DATA_buff + 8);

    GetControlValue(system_set_page, ip_part2_text);
    delay_ms(100);
    ip_part2 = atoi(LCD_DATA_buff + 8);

    GetControlValue(system_set_page, ip_part3_text);
    delay_ms(100);
    ip_part3 = atoi(LCD_DATA_buff + 8);

    GetControlValue(system_set_page, ip_part4_text);
    delay_ms(100);
    ip_part4 = atoi(LCD_DATA_buff + 8);

    // 读取端口
    GetControlValue(system_set_page, net_port_text);
    delay_ms(100);
    port = atoi(LCD_DATA_buff + 8);
}



void ConfirmIPAddressChange(void)
{
	static char ip_address[16];
	if(ip_state == 0)
	{
	    // 读取当前文本控件中的数值
	    ReadIPAndPortFromControls();
	    
	    // 生成完整的IP地址字符串
	    snprintf(ip_address, sizeof(ip_address), "%d.%d.%d.%d", ip_part1, ip_part2, ip_part3, ip_part4);		
		ip_state = 1;
	}
	
	if(ip_state == 1)
	{
	    // 设置IP地址
	    Set_IP_Address(ip_address, port);		
	}

    // 显示成功消息或其他操作
//    printf("IP地址修改成功: %s:%d\n", ip_address, port);	
}



// 在上电时读取 FRAM 中的数据
void Get_IP_Address_On_Power_Up(char* ip_address, uint16_t* port)
{
    uint16_t fram_addr = IP_FRAM_ADDR;

    for (uint8_t i = 0; i < 50; i++)
    {
        ip_address[i] = TWI_ReadByte(fram_addr);
        fram_addr++;
    }

    fram_addr = PORT_FRAM_ADDR;
    *port = (TWI_ReadByte(fram_addr) << 8) | TWI_ReadByte(fram_addr + 1);
}



