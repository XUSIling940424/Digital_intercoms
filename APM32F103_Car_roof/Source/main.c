#include "main.h"

#define  HERAT_TIME    3

/*�����߳̿��ƿ�ָ��*/
static rt_thread_t msg_thread = RT_NULL;
	
/*��������*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13��*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63��*/

int main(void)
{
	int x1_lin_time=0;
	int x2_lin_time=0;
	int k1_lin_time=0;
	char k1_lin_temp=0;
	uint16_t   time_cnt=0;
	
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
					if(x1_lin_time>=MAX_Time_Out)
					{ 					
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
						if(My_Attribute.state==Stand_State)//����״̬
						{
							open_rest();
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Com_Room;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=AUX_Call_Com;   //�������л���  
						}else if(My_Attribute.state==Tall_State)//ͨ��״̬
						{
							My_Attribute.state=Stand_State;
							stop_voice_play();
							My_Attribute.com_talk=0;  My_Attribute.car_talk=0; My_Attribute.dut_talk=0; My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Lift_Addr;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_Down;   //�����һ��ź� 						  
						}						
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
					if(x2_lin_time>=MAX_Time_Out)
					{ 
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Aux_Call_DUT;   //��������ֵ����
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/						
						rt_kprintf("key2 is end\n");
						x2_lin_time=0;  open_rest();
					}else{
						x2_lin_time=0;
					}
				}
        
				if(My_Attribute.state==Call_State_COVER){
				  ring_time++;
					if(ring_time>=MAX_Rint_Rime){
						ring_time=0;
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,0);
						My_Attribute.state=Stand_State;
					}
				}else if(My_Attribute.state==Call_State){
				  ring_time++;
					if(ring_time>=MAX_Rint_Rime){
						ring_time=0;
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,0);
						My_Attribute.state=Stand_State;
					}
				}
				
				rt_thread_delay(100); time_cnt++;
				if(time_cnt>HERAT_TIME){
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Com_Room;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=HearBeat;   //�������ϴ�
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/	
          time_cnt=0;					
				}
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
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 0, 0);

	while(1){				
#if 1		
		/*usart��Ϣ���*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
		}	
#endif		
    /*���can����*/
		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
			remote_msg_handle(&my_remote_msg); 
      rt_memset(my_remote_msg.Can_Msg,0,sizeof(my_remote_msg.Can_Msg));			
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		rt_thread_delay(10);
	}
}

/*�������*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
//	
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://�������л����ź�
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}				
			break;
		
		case Com_Call_Aux://�������и����ź�
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) //������������
			{				
				start_voice_play();
				My_Attribute.state=Tall_State;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Com_Call_Aux;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //������ַ	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/
				rt_kprintf("send Com_Call_Aux black\n");
			}else {
			  ;
			}				
			break;
		
		case Aux_Call_DUT://��������ֵ�����ź�	
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}				
			break;
		
		case Com_Call_Dut://��������ֵ����
			break;
		
		case Dut_Call_Aux://ֵ���Һ��и���
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}			
			break;
		
		case Take_Down://�һ�����				
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room)  //�����һ�����
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //������ַ	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
				My_Attribute.com_talk=0;  rt_kprintf("My_Attribute.dut_talk=%d\n",My_Attribute.dut_talk);
				if(My_Attribute.dut_talk==0){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)//ֵ���ҹһ�����
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //������ַ	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
				My_Attribute.dut_talk=0;  rt_kprintf("My_Attribute.com_talk=%d\n",My_Attribute.com_talk);
				if(My_Attribute.com_talk==0){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}				  
			}
			break;
		
		case Rst_Comm://������ֹ
			stop_rest();
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;		
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Rst_Comm;
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
			rt_sem_release(my_can_needsend_sem);  /*�ͷ��ź���,����Ӳ������*/					
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
}

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}