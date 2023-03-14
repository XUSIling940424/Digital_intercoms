#ifndef  __CAN_H__
#define  __CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_can.h"
#include "My_Can.h"

#define   MAX_FILE_NUM      10

struct can_send_file_buff{
   CAN_TxMessage_T   msg[MAX_FILE_NUM];
	 char input;
	 char output; 
};


struct can_recv_buff{
   CAN_RxMessage_T   msg[MAX_FILE_NUM];
	 char input;
	 char output; 
};

extern CAN_TxMessage_T   TxMessage;
extern CAN_RxMessage_T   RxMessage;
extern struct can_send_file_buff    can_file_buff;
extern struct can_recv_buff    my_recv_can;

int Can1_Init(void);
int Can2_Init(void);

#endif
