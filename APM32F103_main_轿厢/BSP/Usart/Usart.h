#ifndef  __USART_H__
#define  __USART_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_dma.h"
#include "My_Usart.h"
#include "my_time.h"

#define MAX_USART_TXBUFLEN    150       //串口发送最大长度

#define MAX_USART_RXBUFLEN    150       //串口接收最大长度

#define MAX_USART_SENDLEN     10

struct  Usart_Msg_send{
   unsigned char msg[MAX_USART_SENDLEN][MAX_USART_TXBUFLEN];
	 char output;
   char input;	
	 char temp;
};


/*串口2发送缓冲区*/
extern u8 TxBuffer2[MAX_USART_TXBUFLEN];
/*串口2接收缓冲区*/
extern u8 RxBuffer2[MAX_USART_RXBUFLEN];
/*串口2接收缓冲区*/
extern u8 RX_USART2_BUFF[MAX_USART_RXBUFLEN];
/*串口2接收标志位*/
extern u8 Rx_USART2_Flags;
/*串口2接收长度*/
extern u8 Rx_USART2_Len;

extern struct  Usart_Msg_send  My_Usart_Send_Buff;

unsigned char Usart2_Dma_Init(unsigned int boom);
unsigned char Usart3_Dma_Init(unsigned int boom);

#endif

