
#include "uart_device.h"

USART_InitTypeDef usart1;

static uint8_t usart1_tx_buffer[USART1_TX_BUF_SIZE];
static uint8_t usart1_rx_buffer[USART1_RX_BUF_SIZE];


static UART_Device g_usart1 = {
	"usart1",
	Uart_Init,
	Uart_Send,
	Uart_Recv,
//	&g_usart1_data,
	usart1_tx_buffer,
	usart1_rx_buffer,
	USART1_TX_BUF_SIZE,
	USART1_RX_BUF_SIZE,
	0,
	0,
};

struct UART_Data {
    USART_InitTypeDef *handle;
    SemaphoreHandle_t xTxSem;
    QueueHandle_t xRxQueue;
    uint8_t rxdata;
} UART_Data;

static UART_Data g_usart1_data = {
	&huart1,
};


static int Uart_Init(struct UART_Device *pDev, int baud)
{
    UART_Data *data = pDev->priv_data;

    data->xTxSem = xSemaphoreCreateBinary();
    data->xRxQueue = xQueueCreate(UART_RX_QUEUE_LEN, 1);

    /* 启动第1次数据的接收 */
    HAL_UART_Receive_IT(data->handle, &data->rxdata, 1);
    
    return 0;
}

static int Uart_Send(struct UART_Device *pDev, uint8_t *datas, int len, int timeout_ms)
{
    struct UART_Data *data = pDev->priv_data;
    
    /* 仅仅是触发中断而已 */
    HAL_UART_Transmit_IT(data->handle, datas, len);

    /* 等待发送完毕:等待信号量 */
    if (pdTRUE == xSemaphoreTake(data->xTxSem, timeout_ms))
        return 0;
    else
        return -1;

	/* 使用查询方式发送数据 */
	void USART_SendData(USART_TypeDef* USARTx, uint16_t Data)
}

static int Uart_Recv(struct UART_Device *pDev, uint8_t *data, int timeout_ms)
{
    struct UART_Data *uart_data = pDev->priv_data;
    
    /* 读取队列得到数据, 问题:谁写队列?中断:写队列 */
    if (pdPASS == xQueueReceive(uart_data->xRxQueue, data,timeout_ms))
        return 0;
    else
        return -1;


	uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
}








