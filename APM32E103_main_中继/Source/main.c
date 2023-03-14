#include "main.h"

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t msg_thread = RT_NULL;
	
/*��������*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13��*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63��*/

int x1_lin_time=0;
int x2_lin_time=0;
int k1_lin_time=0;
char k1_lin_temp=0;

int main(void)
{	
//	 init_data();
//	 build_can_recv_thread();
//   build_usart_recv_thread();
//	 build_can_send_thread();
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
#if 0		
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
						while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}	
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Lift_Addr;
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Call_Comm;						
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						  rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
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
						while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
						  my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x00;
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Lift_Addr;
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Rst_Comm;
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						  rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/						
							rt_kprintf("key2 is end\n");
							x2_lin_time=0;
					}else{
						x2_lin_time=0;
					}
				}
				/*�һ������*/
				if(K1==0){   /*�һ�״̬*/
					if(k1_lin_time<MAX_Time_Out)
						k1_lin_time++;
					else{
						if(k1_lin_temp==1){   /*�һ�����*/
							if(My_Attribute.state==Tall_State)  //ͨ��״̬
							{
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x00;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Pit_Addr;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_down;							
								my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
								rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/								
								rt_kprintf("s1 is end\n");
								stop_voice_play();
							}								
						}
						k1_lin_temp=0;
						k1_lin_time=MAX_Time_Out;
					}						
				}else if(K1==1)  /*ժ��״̬*/
				{
					if(k1_lin_temp==0){   /*ժ������*/
						  if(My_Attribute.state==Call_State){
								rt_kprintf("take up\n");
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x00;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=My_Attribute.addr;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Call_Comm;							
								my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
								rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
							}
					}
						k1_lin_time=0;
					  k1_lin_temp=1;
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
#if 0		
		/*usart��Ϣ���*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}
#endif
#if 0		
    /*can2������Ϣ���*/
   	if(can2_send_buff.input != can2_send_buff.output){   
		  rt_sem_release(my_can2_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}
#endif		
    /*���can����*/
//		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
//			remote_msg_handle(&my_remote_msg);  
//			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
//		}
		rt_kprintf("OK\n");
		rt_thread_delay(1000);
	}
}

/*�������*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	switch(msg->Can_Msg[msg->Msg_Out].msg.head)
	{
		case 0x01:    //������Ϣ
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm|Rest_Comm))  //���з���
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room)
				{
					My_Attribute.state=Tall_State;
					GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);
					GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);					
					start_voice_play();
					rt_kprintf("recv Com_Room call back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down|Rest_Comm))  //�һ�����
			{
//				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
//				{
				  My_Attribute.state=Stand_State;   My_Attribute.addr=0;
					GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);
					GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);				
					stop_voice_play();
					rt_kprintf("recv Com_Room end tall back\n");
//				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm|Rest_Comm))  //������λ����
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room rest  back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com|Rest_Comm))  //�ֶ���ⷴ��
			{
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room check  back\n");
				}
			}
			break;
		
		case 0x00:    //������Ϣ
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm))  //����
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from | (Duty_Room | Lift_Addr | Car_roof | Pit_Addr)) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))
				{
					rt_kprintf("recv call from Com_Room\n");
          /*������������*/
					while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Call_Comm | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //������ַ									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
					if((k1_lin_temp==0) && (My_Attribute.state==Stand_State))   //�һ�״̬
					{
						My_Attribute.state=Call_State;
						My_Attribute.addr=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
						GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,0);
						GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,1);
					}else if((k1_lin_temp==1) && (My_Attribute.state==Stand_State)){
						My_Attribute.state=Call_State;
						My_Attribute.addr=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
						GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,0);
						GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,1);
					}else{
						GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);
					  GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);					
					}
          start_voice_play();					
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down))  //�һ�
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{				
          /*������������*/
					while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Take_down | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //������ַ									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
					rt_kprintf("recv end tall from Com_Room\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm))  //������λ
			{
				;
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com))  //�ֶ����
			{
				;
			}			
			break;
		
		default:
			break;
	}
}

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}