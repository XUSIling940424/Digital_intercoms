#ifndef  __MY_CAN_H__
#define  __MY_CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "Can.h"
#include "My_Usart.h"

/*反馈宏*/
#define  Rest_Comm      0x40

/*地址宏*/
#define   Duty_Room      0x00     //值班室地址
#define   Com_Room       0x01     //机房地址
#define   Lift_Addr      0x02     //轿厢地址
#define   Car_roof       0x04     //轿顶地址
#define   Pit_Addr       0x08     //底坑地址

/*命令宏*/
#define   Call_Comm      0x01     //呼叫信号
#define   Take_down      0x02     //挂机信号
#define   Rst_Comm       0x03     //报警复位
#define   Check_Com      0x04     //手动检测


#define MAX_MSG_NUM     5
#define MAX_SEND_NUM    10

#define MAX_CAN_SEND_TIME    100

/*状态宏*/
#define   Stand_State    0x01     //待机状态
#define   Call_State     0x02     //呼叫状态
#define   Tall_State     0x03     //通话状态

struct Attribute_t{
  unsigned char state;   //状态
	unsigned char addr;    //地址
};

struct Can_Id{
	int head  : 2 ;    //头  0-3
	int cmd   : 7 ;    //命令
	int msg_from :  8 ;   //消息源地址
	int msg_to :    8 ;   //消息目的地址
	int data :   4 ;     //具体
	int zeros : 3 ;    //空，总为0	
};

union  Remote_frame{   //远程帧消息
	struct Can_Id msg;
	int id;
};

struct Remote_Msg{     //远程帧消息缓存
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

struct Can_Send_Buff{    //can发送缓存
	unsigned char send_buff_fifo[MAX_SEND_NUM][8];
	int send_input;
	int send_output;	
};

/*can获取函数指针*/
typedef int (*recv_can)(struct Can_Msg*,CAN_RxMessage_T*);

/*can回调函数管理器*/
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

