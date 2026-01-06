#ifndef __WIFI_UART_H
#define __WIFI_UART_H

#include "stm32f10x.h"

//之前
#define LAN_MAX_LEN 200

extern uint8_t Modbus_buff[LAN_MAX_LEN];   
extern uint8_t LAN_RX_INDEX; 
extern uint8_t Rcv_Flag;

extern uint8_t wifi_linked;
extern uint8_t wifi_init_done;

void UART3_Configuration(u32 bound);
void USART3_Send_Str(char * buf);
void UartSend(uint8_t *pString,uint16_t DataLength);
void Clr_UART3_RX_buff(void);
void Clr_Uart3_buff1(void);



//新增
#define UART3_MAX_LEN			500
#define U3_BUFF_SIZE			32	


extern unsigned char Uart3_buff1[UART3_MAX_LEN];
extern uint16_t Uart3_Rx_Index1;

extern unsigned char U3_REC_Flag;	
extern unsigned char U3_RX_buff[U3_BUFF_SIZE];
extern unsigned char U3_TX_buff[U3_BUFF_SIZE];
extern unsigned char U3_DATA_buff[U3_BUFF_SIZE];
extern unsigned int  U3_RX_INDEX;		
extern unsigned char U3_RX_LEN;

extern uint8_t at_state_flag;

void Send_AT(const char *cmd);


#endif //__TIMER_H__

