#include "wifi_uart.h"
#include "timer.h"

//之前
uint8_t Modbus_buff[LAN_MAX_LEN];   
uint8_t LAN_RX_INDEX = 0;  
uint8_t Rcv_Flag = 0;
uint8_t wifi_linked = 0;
uint8_t wifi_init_done = 0;


//新增
unsigned char Uart3_buff1[UART3_MAX_LEN];
uint16_t Uart3_Rx_Index1 = 0;

unsigned char U3_REC_Flag = 0;	
unsigned char U3_RX_buff[U3_BUFF_SIZE];
unsigned char U3_TX_buff[U3_BUFF_SIZE];
unsigned char U3_DATA_buff[U3_BUFF_SIZE];
unsigned int  U3_RX_INDEX = 0;	
unsigned char U3_RX_LEN = 0;

uint8_t at_state_flag = 1;

void UART3_Configuration(u32 bound)
{
  //GPIO端口设置
  	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	
	//串口3对应引脚复用映射
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE); //复用为USART3	
  
	/**USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
  */
	//USART3_TX   PD8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
  //USART3_RX	  PD9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);    
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); 	//初始化串口3
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART3, ENABLE);                    //使能串口3 

}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		if(at_state_flag == 1)	//如果wifi已经连接，且处于透传模式
		{	
			
//			TIM_Cmd(TIM5, ENABLE);	
			TIM_Cmd(TIM5, DISABLE);
			if(tim5_count == 0)
			{
				tim5_count = 20;
			}
			
			Modbus_buff[LAN_RX_INDEX] = USART3->DR;	//读取接收到的数据   
			LAN_RX_INDEX++;	
			if(LAN_RX_INDEX >= LAN_MAX_LEN)//超过接收缓冲区大小
			{
				LAN_RX_INDEX = 0;
			}		
			Rcv_Flag = 1;
			modbus_1ms_cnt = 0;
		} else 	//连接wifi
		{
			if(Uart3_Rx_Index1 <= 499)  //对于wifi信息，只读取前500字节的数据
			{
				Uart3_buff1[Uart3_Rx_Index1] = USART3->DR;	//读取接收到的数据   
				Uart3_Rx_Index1++;
			}
		}
		
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);  // 手动清除中断标志
	}
} 

void USART3_Send_Str(char * buf)
{  
	u8 i=0;
	while(1)
	{  
		if(buf[i]!=0)
		{  
			USART_SendData(USART3, buf[i]);  
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}; 
			i++;
		}
		else 
			return;
	}
}

void UartSend(uint8_t *pString,uint16_t DataLength)
{
 	uint8_t i = 0;
	for(i = 0;i < DataLength;i ++)
	{
		USART3->DR = pString[i];
		while((USART3->SR&0x40)==0);	
	}	
}


void Clr_Uart3_buff1(void)
{
	memset(Uart3_buff1, 0, sizeof(Uart3_buff1));
	Uart3_Rx_Index1 = 0;
}



void Clr_UART3_RX_buff(void)
{
	u16 i;
	for(i=0;i<LAN_MAX_LEN;i++)
	{
		Modbus_buff[i] = 0;
	}
	LAN_RX_INDEX = 0;
}

void Send_AT(const char *cmd)
{
    USART3_Send_Str((char *)cmd);
}

