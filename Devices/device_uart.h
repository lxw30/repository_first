
#ifndef _UART_DEVICE_H
#define _UART_DEVICE_H

#include <stdint.h>

struct UART_Device {
    char *name;
    int (*Init)(struct UART_Device *pDev, int baud);
    int (*Send)(struct UART_Device *pDev, uint8_t *datas, int len, int timeout_ms);
    int (*Recv)(struct UART_Device *pDev, uint8_t *data, int timeout_ms);
//  void *priv_data;
 
    uint8_t *tx_buffer;
	uint8_t *rx_buffer;
    uint8_t rx_buffer_size;
    uint8_t tx_buffer_size; 
    volatile uint8_t rx_index; 
    volatile uint8_t tx_index; 
} UART_Dev;

struct UART_Device *GetUARTDevice(char *name);



#define USART1_TX_BUF_SIZE 100
#define USART1_RX_BUF_SIZE 100

#define USART2_TX_BUF_SIZE 100
#define USART2_RX_BUF_SIZE 100

#define UART4_TX_BUF_SIZE 100
#define UART4_RX_BUF_SIZE 100

#define UART5_TX_BUF_SIZE 100
#define UART5_RX_BUF_SIZE 100



#endif


