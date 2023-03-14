#ifndef  __MY_USART_H__
#define  __MY_USART_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "Usart.h"
#include "My_Can.h"
#include "udp.h"

#define MAX_USART_MSG_NUM     5

#define USART3_BLACK          0x80

/*数字对讲值班室和屏幕协议*/

#define  EXTEN_CALL      			  		1    //分机呼叫
#define  HANDLE_TAKE_DOWN       		2    //手柄挂机
#define  HANDLE_TAKE_UP         		3    //手柄摘机
#define  EXTEN_ADD_TALK         		4    //分机加入通话
#define  COMPUTER_ROOM_REST     		5    //机房复位
#define  COMPUTER_ROOM_TAKE_DOWN    11   //机房挂机
#define  HEART_KEEP                 16   //心跳信息
#define  KEY0_KEEP                  17   //KEY0信息
#define  KEY1_KEEP                  18   //KEY1信息
#define  KEY2_KEEP                  19   //KEY2信息
#define  KEY3_KEEP                  20   //KEY3信息
#define  KEY4_KEEP                  21   //KEY4信息
#define  TALL_STAT                  24   //通话状态上报
#define  TAKE_DOWN_STAT             25   //挂机信息

#define  SCREE_CALL             		6    //屏幕呼叫
#define  SCREE_TAKE_DOWN        		7    //屏幕挂机
#define  SCREE_REST             		8    //屏幕复位
#define  SCREE_CHECK            		9    //屏幕检测
#define  SCREE_RES_CALL         		10   //屏幕呼叫应答
#define  RING                       12   //振铃开关信号
#define  CHANG_AUDIO                13   //切换音频信号
#define  AUDIO_CF                   14   //音频功放信号
#define  REST_4G                    15   //4G模块复位
#define  VOLUME                     22   //音量
#define  ON_SPEAK                   23   //免提

struct Usart_Msg{
	unsigned char msg_buff[MAX_USART_MSG_NUM][150];
	int msg_input;
	int msg_output;
	int temp;   //串口状态
	int len;  //数据有效长度
};

struct Msg{
  unsigned char msg_buff[50];
	int msg_len;
};

struct USART3_MSG{
	struct Msg msg_buff[MAX_USART_MSG_NUM];
	int msg_input;
	int msg_output;	
};

struct Usart_Comm{	
	int idx;
  int len;	
	/*数据格式*/
  unsigned char Head[3];
	unsigned char Comm;
	unsigned char DatLen;
	unsigned char Data[20];
};

extern struct Usart_Msg  my_usart_msg;
extern rt_sem_t  my_usart_sem;
extern rt_mutex_t usart2_send_mutex;
extern rt_sem_t  my_usart2_send_sem;

extern rt_sem_t  my_usart3_sem;
extern rt_mutex_t usart3_send_mutex;
extern struct USART3_MSG  my_usart3_msg;

int build_usart_recv_thread(void);
int build_usart3_recv_thread(void);

void usart1_send_Data(unsigned char*buf,unsigned char len);
void usart2_send_Data(unsigned char*buf,unsigned char len);
void usart3_send_Data(unsigned char*buf,unsigned char len );
void  usart2_dma_send(unsigned char*buf,unsigned char len);
void  usart3_dma_send(unsigned char*buf,unsigned char len);
void usart3_send_comm(struct Usart_Comm * comm);
#endif