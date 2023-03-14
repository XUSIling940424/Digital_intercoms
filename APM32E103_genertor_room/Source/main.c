#include "main.h"

#define  MAX_TIME_OUT    4

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t msg_thread = RT_NULL;
	
/*��������*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13��*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63��*/

int x1_lin_time=0;
int x2_lin_time=0;
int x3_lin_time=0;

int k1_lin_time=0;
char k1_lin_temp=0;

uint16_t   time_cnt=0;

int main(void)
{	
	 init_data();
	 build_can_recv_thread();
   build_usart_recv_thread();
	 build_can_send_thread();
		/*�����߳�*/
		msg_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "msg_send",                 /* �߳����� */
											msg_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											3,                      /* �̵߳����ȼ� */
											200);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(msg_thread != RT_NULL){
			rt_kprintf("rt_thread_create is success\n");
			rt_thread_startup(msg_thread);
		}
		else{ 
			rt_kprintf("rt_thread_create is faile\n");
			return -1;	
		}	
#if 1		
    while (1)
    {
			  /*����1���*/
			  if(X1==0){
					if(x1_lin_time<MAX_Time_Out)
						x1_lin_time++;
					else
						x1_lin_time=MAX_Time_Out;	
				}else{
					if(x1_lin_time==MAX_Time_Out)
					{ 
						/*����ֵ����*/
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd=Com_Call_Dut;
						my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/						
						rt_kprintf("key1 is end\n");  
						x1_lin_time=0;
					}else{
						x1_lin_time=0;
					}
				}
				/*����2���*/
			  if(X2==0){
					if(x2_lin_time<MAX_Time_Out)
						x2_lin_time++;
					else
						x2_lin_time=MAX_Time_Out;
					
				}else{
					if(x2_lin_time==MAX_Time_Out)
					{ 
						/*��λ�����ڲ�*/
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Rst_Comm;
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
						/*��λֵ����*/
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd=Rst_Comm;
						my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
            x2_lin_time=0;						
					}else{
						x2_lin_time=0;
					}
				}
				/*����3���*/
			  if(X3==0){
					if(x3_lin_time<MAX_Time_Out)
						x3_lin_time++;
					else
						x3_lin_time=MAX_Time_Out;
					
				}else{
					if(x3_lin_time==MAX_Time_Out)
					{ 
					 x3_lin_time=0;
					 if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0)  //�������˴�
					 {
						 wiz_ALM_C(1);
              /*���������ϴ�*/
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=ALA_FILT;
						  my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=0x01;
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						  rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/							 
					 
					 }else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1)  //�������˹ر�
					 {
						 /*���������ϴ�*/
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=ALA_FILT;
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=0x02;
						 my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						 rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/							 
					   wiz_ALM_C(0);
					 }
						
					}else{
						x3_lin_time=0;
					}
				}				
				
				/*�һ������*/
				if(K1==0){   /*�һ�״̬*/
					if(k1_lin_temp==1){   /*�һ�����*/
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_Down;							
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/										
						rt_kprintf("send Take_Down cmd\n");
						stop_voice_play();  My_Attribute.state=Stand_State;
						My_Attribute.com_talk=0;  My_Attribute.car_talk=0; My_Attribute.dut_talk=0; My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
					}
					k1_lin_time=0;							
					k1_lin_temp=0;						
				}else if(K1==1)  /*ժ��״̬*/
				{
					if(k1_lin_time<MAX_Time_Out)
						k1_lin_time++;
					else{
						if(k1_lin_temp==0){   /*ժ������*/
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Com_Call_Aux;							
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
							rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
							rt_kprintf("send Com_Call_Aux cmd\n");						
						}
					  k1_lin_temp=1;
					  k1_lin_time=MAX_Time_Out;
					}
				}
		/*״̬���*/
    if(My_Attribute.state==Call_State)
		{
		  GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,!GPIO_ReadOutputBit(GPIOB,GPIO_PIN_6));
			ring_time++;
			if(ring_time>=MAX_Rint_Rime){
				ring_time=0;
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);
				GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);
				My_Attribute.state=Stand_State;
			}			
		}
    /*���߼��*/
		time_cnt++;
		if(time_cnt>MAX_TIME_OUT){
			My_Attribute.car_line_temp++;
			My_Attribute.lif_line_temp++;
			My_Attribute.pit_line_temp++;
			if(My_Attribute.car_line_temp>=MAX_TIME_OUT/2){  //�ζ���ʱδ��������
				My_Attribute.car_line=0;
				My_Attribute.car_line_temp=0;
			}
			if(My_Attribute.lif_line_temp>=MAX_TIME_OUT/2){  //���ᳬʱδ��������
				My_Attribute.lif_line=0;
				My_Attribute.lif_line_temp=0;
			}
			if(My_Attribute.pit_line_temp>=MAX_TIME_OUT/2){  //�׿ӳ�ʱδ��������
				My_Attribute.pit_line=0;
				My_Attribute.pit_line_temp=0;
			}
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd=HearBeat;
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=0x00;
			
			rt_kprintf("My_Attribute.car_line=%d\n",My_Attribute.car_line);
			rt_kprintf("My_Attribute.lif_line=%d\n",My_Attribute.lif_line);
			rt_kprintf("My_Attribute.pit_line=%d\n",My_Attribute.pit_line);

			
			if(My_Attribute.car_line==1){   //�ζ�����
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x01;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfe;
			}
			if(My_Attribute.lif_line==1){   //��������
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x02;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfd;
			}
			if(My_Attribute.pit_line==1){   //�׿�����
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x04;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfb;
			}			
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x08;  //����
			my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
      time_cnt=0;			  
		}
		
    rt_thread_delay(100);
    }
	#endif	
}


/*�̶߳���*/
static void msg_thread_entry(void *paramter)
{
	int   i=0;
	int 	ret=0;
  
  /*��������*/
//  usart2_send_Data(voice_chang1,5);	
//	usart2_send_Data(voice_chang2,5);	
	
//	start_voice_play();				
	CAN_EnableInterrupt(CAN1, CAN_INT_F0MP );  		
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 0, 1);			
	CAN_EnableInterrupt(CAN2, CAN_INT_F1MP );  		
	NVIC_EnableIRQRequest(CAN2_RX1_IRQn, 1, 0);	
	
	while(1){				
#if 1		
		/*usart��Ϣ���*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}
#endif
#if 1		
    /*can2������Ϣ���*/
   	if(can2_send_buff.input != can2_send_buff.output){   
		  rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}
#endif		
    /*���can����*/
		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
			remote_msg_handle(&my_remote_msg);  
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		if(my_can2_remote_msg.Msg_Out!=my_can2_remote_msg.Msg_In){
			remote_msg_handle(&my_can2_remote_msg);  
			my_can2_remote_msg.Msg_Out=((my_can2_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		rt_thread_delay(10);
	}
}

/*�������*/
static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
 
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://�������л����ź�
			rt_kprintf("recv AUX_Call_Com\n");
			/*������Ϣ*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =AUX_Call_Com; 
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
			if(My_Attribute.state==Stand_State){//����״̬
				 My_Attribute.state=Call_State;
				 open_call();
			}else{
				 ;
			}
      open_rest();			
			break;
		
		case Com_Call_Aux://���������ڲ������ź�
			stop_call();
			start_voice_play();
			My_Attribute.state=Tall_State;
			break;
		
		case Aux_Call_DUT://��������ֵ�����ź�
			/*ת��ֵ����*/
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr)){  //�ζ��͵׿ӷ��ͺ����ź�
				if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1){  //�رձ�������
					rt_kprintf("recv Aux_Call_DUT from Car_roof | Pit_Addr\n");
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
				}else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0){  //�򿪱�������
					open_rest();
					/*������Ϣ*/
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Aux_Call_DUT; 
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
					if(My_Attribute.state==Stand_State){//����״̬
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}				
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)  //����������������ź�
			{
				open_rest();
				if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1){  //�رձ�������
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
					/*�ı�״̬*/
					if(My_Attribute.state==Stand_State){//����״̬
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}
				}else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0){  //�򿪱�������
					/*������Ϣ*/
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Aux_Call_DUT; 
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
					if(My_Attribute.state==Stand_State){//����״̬
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}				
				}				
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) {  //ֵ������������
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
        rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
        rt_kprintf("recv Aux_Call_DUT from Dutt_Room");			  
			}		
			break;
		
		case Com_Call_Dut://��������ֵ����
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //ֵ���ҷ�����Ϣ
			{
				if(My_Attribute.state==Stand_State){//����״̬
					start_voice_play();
					My_Attribute.state=Tall_State;
				}else{
					 ;
				}				
			}				
			break;
		
		case Dut_Call_Aux://ֵ���Һ��и���
			open_rest();
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //ֵ������������
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
				if(My_Attribute.state==Stand_State){//����״̬
					 My_Attribute.state=Call_State;
					 open_call();
				}else{
					 ;
				}					
			}else {  //��������
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/			  
			}				
			break;
		
		case Take_Down://�һ�����
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //ֵ������������
			{rt_kprintf("recv Take_Down from Dutt_Room\n");
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //�ζ��Ĺһ��ź�
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr){  //�׿ӵĹһ��ź�
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
			}			
			break;
		
		case Rst_Comm://������ֹ
			stop_rest();
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==ALL_AUX__Room))  //ֵ������������,��λ����
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
			}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))  //�������ͺ�ֵ���ҵķ����ź�
			{
			
			}else if(((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)) \
			  &&(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Dutt_Room))	//��������ֵ�����ź�
			{
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/			  
			}else if(((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)) \
			  &&(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))	//�������������ź�
			{	
				;
			}			
			break;
		
		case Check_Com://�ֶ����
			break;
		
		case HearBeat://�������ϴ�
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof)  //�ζ�������
			{
			  My_Attribute.car_line_temp=0;   My_Attribute.car_line=1;
			}
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr)  //�׿�������
			{
			  My_Attribute.pit_line_temp=0;  My_Attribute.pit_line=1;
			}
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)  //����������
			{
			  My_Attribute.lif_line_temp=0;  My_Attribute.lif_line=1;
			}					
			break;
		
		case ALA_FILT://�������������ϴ�
			break;
		
		case Elec_DULER://���Ƿѹ
			break;
		
		case VOILD_Chang://��������
			break;
		
		default:
			break;
	}
}

#if 0
		switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
		{
		  case AUX_Call_Com://�������л����ź�
				break;
			case Com_Call_Aux://�һ��ź�
				break;
			case Aux_Call_DUT://��������ֵ�����ź�
				break;
			case Com_Call_Dut://��������ֵ����
				break;	
			case Dut_Call_Aux://ֵ���Һ��и���
				break;
			case Take_Down://�һ�����
				break;
			case Rst_Comm://������ֹ
				break;
			case Check_Com://�ֶ����
				break;	
			case HearBeat://�������ϴ�
				break;
			case ALA_FILT://�������������ϴ�
				break;	
			case Elec_DULER://���Ƿѹ
				break;	
			case VOILD_Chang://��������
				break;
			default:
				break;
		}

#endif

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}