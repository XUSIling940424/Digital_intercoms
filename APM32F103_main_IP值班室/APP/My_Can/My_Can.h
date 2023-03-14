#ifndef  __MY_CAN_H__
#define  __MY_CAN_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "Can.h"
#include "My_Usart.h"

/*������*/
#define  Rest_Comm      0x40

/*��ַ��*/
#define   ALL_AUX__Room      0x00     //�ڲ����и�����ַ
#define   Car_roof           0x01     //�ζ���ַ
#define   Pit_Addr           0x02     //�׿ӵ�ַ
#define   Com_Room           0x03     //������ַ
#define   Lift_Addr          0x04     //�����ַ
#define   Dutt_Room          0x05     //ֵ���ҵ�ַ

/*�����*/
#define   AUX_Call_Com      0x01     //�������л����ź�
#define   Com_Call_Aux      0x02     //���������ڲ������ź�
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

extern struct Attribute_t  My_Attribute;

#endif