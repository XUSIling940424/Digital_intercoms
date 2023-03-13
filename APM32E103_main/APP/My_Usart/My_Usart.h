#ifndef  __MY_USART_H__
#define  __MY_USART_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "Usart.h"
#include "My_Can.h"

#define MAX_USART_MSG_NUM     5

struct Usart_Msg{
	unsigned char msg_buff[MAX_USART_MSG_NUM][121];
	int msg_input;
	int msg_output;
	int temp;   //´®¿Ú×´Ì¬
};

extern struct Usart_Msg  my_usart_msg;
extern rt_sem_t  my_usart_sem;
extern rt_mutex_t usart2_send_mutex;
extern rt_sem_t  my_usart2_send_sem;

int build_usart_recv_thread(void);
void usart1_send_Data(unsigned char*buf,unsigned char len);
void usart2_send_Data(unsigned char*buf,unsigned char len);
void  usart2_dma_send(unsigned char*buf,unsigned char len);
#endif

