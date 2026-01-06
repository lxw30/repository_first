#ifndef __WIFI_H__
#define __WIFI_H__

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"



extern uint8_t work_flag;

void Wifi_Set_Info_Process(void);

//ÐÂÔö
#define BUFFER_MAX_LEN			500
#define MAX_RSSI_LENGTH 10
#define MAX_SSID_LENGTH 50
#define WIFI_HEAD_LENGTH 57+10
#define WIFI_MSG_LENGTH (sizeof(Uart3_buff1) - WIFI_HEAD_LENGTH)

extern uint8_t wifi_number;

extern uint8_t wifi_scan_flag;

extern uint8_t wifi_start_test_flag;

extern char *wifi_ssid;

extern char wifi1_RSSI[MAX_RSSI_LENGTH];
extern char wifi1_SSID[MAX_SSID_LENGTH];
extern char wifi2_RSSI[MAX_RSSI_LENGTH];
extern char wifi2_SSID[MAX_SSID_LENGTH];
extern char wifi3_RSSI[MAX_RSSI_LENGTH];
extern char wifi3_SSID[MAX_SSID_LENGTH];
extern char wifi4_RSSI[MAX_RSSI_LENGTH];
extern char wifi4_SSID[MAX_SSID_LENGTH];
extern char wifi5_RSSI[MAX_RSSI_LENGTH];
extern char wifi5_SSID[MAX_SSID_LENGTH];

extern uint8_t Uart3_Rcv_Flag;

extern uint8_t connect_state;
//extern volatile uint8_t at_state;
extern uint8_t at_state;

extern uint8_t pre_state;
extern uint8_t cur_state;

extern uint8_t start_set_ip;


uint8_t Pre_Process(void);
void ProcessData0(void);
void ProcessData1(void);
void ProcessData2(void);
void ProcessData3(void);
void ProcessData4(void);
void ProcessData5(void);

void Scan_Wifi(void);
uint8_t get_wifi_number(void);
void Connect_Wifi( uint8_t x );
void wifi_set10_info_process(void);

void Set_IP_Address(const char* ip_address, uint16_t port);
void ReadIPAndPortFromControls(void);

void ConfirmIPAddressChange(void);

void Get_IP_Address_On_Power_Up(char* ip_address, uint16_t* port);


#endif //__WIFI_H__

