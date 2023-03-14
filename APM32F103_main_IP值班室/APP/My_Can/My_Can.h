#ifndef  __MY_CAN_H__
#define  __MY_CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "Can.h"
#include "My_Usart.h"

/*反馈宏*/
#define  Rest_Comm      0x40

/*地址宏*/
#define   ALL_AUX__Room      0x00     //内部所有副机地址
#define   Car_roof           0x01     //轿顶地址
#define   Pit_Addr           0x02     //底坑地址
#define   Com_Room           0x03     //机房地址
#define   Lift_Addr          0x04     //轿厢地址
#define   Dutt_Room          0x05     //值班室地址

/*命令宏*/
#define   AUX_Call_Com      0x01     //副机呼叫机房信号
#define   Com_Call_Aux      0x02     //机房呼叫内部副机信号
#define   Aux_Call_DUT      0x03     //副机呼叫值班室信号
#define   Com_Call_Dut      0x04     //机房呼叫值班室
#define   Dut_Call_Aux      0x05     //值班室呼叫副机
#define   Take_Down         0X06     //挂机命令
#define   Rst_Comm          0x07     //报警终止
#define   Check_Com         0x08     //手动检测
#define   HearBeat          0x09     //心跳包上传
#define   ALA_FILT          0x0a     //机房报警过滤上传
#define   Elec_DULER        0x0b     //电池欠压
#define   VOILD_Chang       0x0c     //音量调节

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
	
	unsigned char com_line;   //机房在线标志
	unsigned char com_line_temp;   //机房在线计数
	unsigned char dut_line;   //值班室在线标志
	unsigned char dut_line_temp;   //值班室在线计数
	unsigned char car_line;   //轿顶在线标志
	unsigned char car_line_temp;   //轿顶在线计数
	unsigned char lif_line;   //轿厢在线标志
	unsigned char lif_line_temp;   //轿厢在线计数	
	unsigned char pit_line;   //底坑在线标志
	unsigned char pit_line_temp;   //底坑在线计数			
};

extern struct Attribute_t  My_Attribute;

#endif