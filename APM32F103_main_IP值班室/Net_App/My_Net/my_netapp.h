#ifndef __MY_NETAPP_H__
#define __MY_NETAPP_H__

#include "apm32f10x.h"
#include "stdio.h"
#include "udp.h"
#include "tcp_server.h"
#include "tcp_client.h"
#include "rtthread.h"
#include "My_Can.h"

#define MAX_NET_MSG_NUM   2

#define MAX_NET_PORT_NUM  5

struct Net_Msg{
	unsigned char msg_buff[MAX_NET_MSG_NUM][121];
	int msg_input;
	int msg_output;
	int temp;   
};

struct Net_Comm{
	unsigned char msg_from;
	unsigned char msg_to;	
	unsigned char cmd;
	unsigned char data;
	unsigned char zeros;
	unsigned char client_ip[4];
};

struct Net_Port_Msg{
  struct  Net_Comm  msg_buff[MAX_NET_MSG_NUM];
	int msg_input;
	int msg_output;	
};

extern struct Net_Port_Msg ser_6667_recv;
extern struct Net_Port_Msg ser_6666_recv;

int build_my_net_thread(void);
uint8 tcp_client_send(u8 Number,u16 client_poll,uint8 * server_ip,u16 server_poll,u8 *sendbuf,u16 send_len);
void tcp_server_list(u8 Number,u16 server_poll);

#endif
