#ifndef  __MY_CAN_H__
#define  __MY_CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "Can.h"
#include "My_Usart.h"

/*������*/
#define  Rest_Comm      0x40

/*��ַ��*/
#define   Duty_Room      0x00     //ֵ���ҵ�ַ
#define   Com_Room       0x01     //������ַ
#define   Lift_Addr      0x02     //�����ַ
#define   Car_roof       0x04     //�ζ���ַ
#define   Pit_Addr       0x08     //�׿ӵ�ַ

/*�����*/
#define   Call_Comm      0x01     //�����ź�
#define   Take_down      0x02     //�һ��ź�
#define   Rst_Comm       0x03     //������λ
#define   Check_Com      0x04     //�ֶ����


#define MAX_MSG_NUM     5
#define MAX_SEND_NUM    10

#define MAX_CAN_SEND_TIME    100

/*״̬��*/
#define   Stand_State    0x01     //����״̬
#define   Call_State     0x02     //����״̬
#define   Tall_State     0x03     //ͨ��״̬

struct Attribute_t{
  unsigned char state;   //״̬
	unsigned char addr;    //��ַ
};

struct Can_Id{
	int head  : 2 ;    //ͷ  0-3
	int cmd   : 7 ;    //����
	int msg_from :  8 ;   //��ϢԴ��ַ
	int msg_to :    8 ;   //��ϢĿ�ĵ�ַ
	int data :   4 ;     //����
	int zeros : 3 ;    //�գ���Ϊ0	
};

union  Remote_frame{   //Զ��֡��Ϣ
	struct Can_Id msg;
	int id;
};

struct Remote_Msg{     //Զ��֡��Ϣ����
		union Remote_frame Can_Msg[MAX_MSG_NUM];
		int Msg_In;
		int Msg_Out;
};

struct Can_Id_Buf{
	unsigned int  Can_Id_Num;
	unsigned char Can_Data[8];
};

struct Can_Msg{
	unsigned char msg_buff[MAX_MSG_NUM][80];
	int msg_input;
	int msg_output;
	int msg_id;	
};

struct Can_Send_Buff{    //can���ͻ���
	unsigned char send_buff_fifo[MAX_SEND_NUM][8];
	int send_input;
	int send_output;	
};

/*can��ȡ����ָ��*/
typedef int (*recv_can)(struct Can_Msg*,CAN_RxMessage_T*);

/*can�ص�����������*/
typedef struct can_recv_data{
	struct Can_Msg a;
	CAN_RxMessage_T b;
	recv_can   recv_can_back;
}can_recv_data_t;


extern struct Can_Msg  Recv_Can_buff[];
extern rt_sem_t  my_can_sem;
extern rt_sem_t  my_cansend_sem;
extern rt_sem_t  my_can_needsend_sem;
extern struct Remote_Msg   my_remote_msg;
extern struct Can_Send_Buff   my_send_buff_fifo;
extern struct Remote_Msg   my_send_remote_msg;
extern struct Attribute_t  My_Attribute;
extern rt_sem_t  my_can2_needsend_sem;
extern rt_sem_t  my_can2_sem;


int build_can_recv_thread(void);
int build_can_send_thread(void);

int my_can_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage);
int Can_Register(can_recv_data_t *data);
int can1_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage);


int my_can2_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage);
int can2_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage);
#endif

