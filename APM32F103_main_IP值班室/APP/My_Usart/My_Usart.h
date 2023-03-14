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

/*���ֶԽ�ֵ���Һ���ĻЭ��*/

#define  EXTEN_CALL      			  		1    //�ֻ�����
#define  HANDLE_TAKE_DOWN       		2    //�ֱ��һ�
#define  HANDLE_TAKE_UP         		3    //�ֱ�ժ��
#define  EXTEN_ADD_TALK         		4    //�ֻ�����ͨ��
#define  COMPUTER_ROOM_REST     		5    //������λ
#define  COMPUTER_ROOM_TAKE_DOWN    11   //�����һ�
#define  HEART_KEEP                 16   //������Ϣ
#define  KEY0_KEEP                  17   //KEY0��Ϣ
#define  KEY1_KEEP                  18   //KEY1��Ϣ
#define  KEY2_KEEP                  19   //KEY2��Ϣ
#define  KEY3_KEEP                  20   //KEY3��Ϣ
#define  KEY4_KEEP                  21   //KEY4��Ϣ
#define  TALL_STAT                  24   //ͨ��״̬�ϱ�
#define  TAKE_DOWN_STAT             25   //�һ���Ϣ

#define  SCREE_CALL             		6    //��Ļ����
#define  SCREE_TAKE_DOWN        		7    //��Ļ�һ�
#define  SCREE_REST             		8    //��Ļ��λ
#define  SCREE_CHECK            		9    //��Ļ���
#define  SCREE_RES_CALL         		10   //��Ļ����Ӧ��
#define  RING                       12   //���忪���ź�
#define  CHANG_AUDIO                13   //�л���Ƶ�ź�
#define  AUDIO_CF                   14   //��Ƶ�����ź�
#define  REST_4G                    15   //4Gģ�鸴λ
#define  VOLUME                     22   //����
#define  ON_SPEAK                   23   //����

struct Usart_Msg{
	unsigned char msg_buff[MAX_USART_MSG_NUM][150];
	int msg_input;
	int msg_output;
	int temp;   //����״̬
	int len;  //������Ч����
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
	/*���ݸ�ʽ*/
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