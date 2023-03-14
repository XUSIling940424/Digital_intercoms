#ifndef  __MY_CAN_H__
#define  __MY_CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "Can.h"
#include "My_Usart.h"



/*��ַ��*/
#define   ALL_AUX__Room      0x00     //�ڲ����и�����ַ
#define   Car_roof           0x01     //�ζ���ַ
#define   Pit_Addr           0x02     //�׿ӵ�ַ
#define   Com_Room           0x03     //������ַ
#define   Lift_Addr          0x04     //�����ַ
#define   Dutt_Room          0x05     //ֵ���ҵ�ַ

/*�����*/
#define   AUX_Call_Com      0x01     //�������л����ź�
#define   Com_Call_Aux      0x02     //���������ڲ�����
#define   Aux_Call_DUT      0x03     //��������ֵ�����ź�
#define   Com_Call_Dut      0x04     //��������ֵ����
#define   Dut_Call_Aux      0x05     //ֵ���Һ��и���
#define   Take_Down         0X06     //�һ�����
#define   Rst_Comm          0x07     //������ֹ
#define   Check_Com         0x08     //�ֶ����
#define   HearBeat          0x09     //�������ϴ�
#define   ALA_FILT          0x0a     //�������������ϴ�
#define   Elec_DULER        0x0b     //���Ƿѹ
#define   VOILD_Chang       0x0c     //��������


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
	
	unsigned char com_talk;   //����ͨ����־
	unsigned char dut_talk;   //ֵ����ͨ����־	
	unsigned char car_talk;   //�ζ�ͨ����־
	unsigned char lif_talk;   //����ͨ����־
	unsigned char pit_talk;   //�׿�ͨ����־
	
	unsigned char com_line;   //�������߱�־
	unsigned char com_line_temp;   //�������߼���
	unsigned char dut_line;   //ֵ�������߱�־
	unsigned char dut_line_temp;   //ֵ�������߼���
	unsigned char car_line;   //�ζ����߱�־
	unsigned char car_line_temp;   //�ζ����߼���
	unsigned char lif_line;   //�������߱�־
	unsigned char lif_line_temp;   //�������߼���	
	unsigned char pit_line;   //�׿����߱�־
	unsigned char pit_line_temp;   //�׿����߼���		
};

struct Can_Id{
	int msg_from :  4 ;   //��ϢԴ��ַ
	int msg_to :    4 ;   //��ϢĿ�ĵ�ַ	
	int cmd   : 5 ;    //����
	int data :   16 ;     //����
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
extern struct Remote_Msg   my_can2_send_remote_msg;
extern struct Remote_Msg   my_can2_remote_msg;


int build_can_recv_thread(void);
int build_can_send_thread(void);

int my_can_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage);
int Can_Register(can_recv_data_t *data);
int can1_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage);


int my_can2_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage);
int can2_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage);
#endif

