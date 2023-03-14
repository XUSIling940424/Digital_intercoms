#include "main.h"

/*定义线程控制块指针*/
static rt_thread_t msg_thread = RT_NULL;
	
/*函数声明*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13级*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63级*/

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
		/*创建线程*/
		msg_thread=                              /* 线程控制块指针 */
		rt_thread_create( "msg_send",                 /* 线程名字 */
											msg_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											3,                      /* 线程的优先级 */
											200);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
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
			  /*按键1检测*/
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
						  rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
							rt_kprintf("key1 is end\n");  
							x1_lin_time=0;
					}else{
						x1_lin_time=0;
					}
				}
				/*按键2检测*/
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
						  rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/						
							rt_kprintf("key2 is end\n");
							x2_lin_time=0;
					}else{
						x2_lin_time=0;
					}
				}
				/*挂机键检测*/
				if(K1==0){   /*挂机状态*/
					if(k1_lin_time<MAX_Time_Out)
						k1_lin_time++;
					else{
						if(k1_lin_temp==1){   /*挂机动作*/
							if(My_Attribute.state==Tall_State)  //通话状态
							{
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x00;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Pit_Addr;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_down;							
								my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
								rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/								
								rt_kprintf("s1 is end\n");
								stop_voice_play();
							}								
						}
						k1_lin_temp=0;
						k1_lin_time=MAX_Time_Out;
					}						
				}else if(K1==1)  /*摘机状态*/
				{
					if(k1_lin_temp==0){   /*摘机动作*/
						  if(My_Attribute.state==Call_State){
								rt_kprintf("take up\n");
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x00;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=My_Attribute.addr;
								my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Call_Comm;							
								my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
								rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
							}
					}
						k1_lin_time=0;
					  k1_lin_temp=1;
				}
        rt_thread_delay(100);
    }
	#endif	
}


/*线程定义*/
static void msg_thread_entry(void *paramter)
{
	int   i=0;
	int 	ret=0;
  
  /*音量调节*/
//  usart2_send_Data(voice_chang1,5);	
//	usart2_send_Data(voice_chang2,5);	
	
//	start_voice_play();				
	CAN_EnableInterrupt(CAN1, CAN_INT_F0MP );  		
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 0, 1);			
	CAN_EnableInterrupt(CAN2, CAN_INT_F1MP );  		
	NVIC_EnableIRQRequest(CAN2_RX1_IRQn, 1, 0);	
	
	while(1){				
#if 0		
		/*usart消息检测*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
		}
#endif
#if 0		
    /*can2语音消息检测*/
   	if(can2_send_buff.input != can2_send_buff.output){   
		  rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/
		}
#endif		
    /*检测can命令*/
//		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
//			remote_msg_handle(&my_remote_msg);  
//			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
//		}
		rt_kprintf("OK\n");
		rt_thread_delay(1000);
	}
}

/*命令处理函数*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	switch(msg->Can_Msg[msg->Msg_Out].msg.head)
	{
		case 0x01:    //反馈信息
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm|Rest_Comm))  //呼叫反馈
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room)
				{
					My_Attribute.state=Tall_State;
					GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);
					GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);					
					start_voice_play();
					rt_kprintf("recv Com_Room call back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down|Rest_Comm))  //挂机反馈
			{
//				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
//				{
				  My_Attribute.state=Stand_State;   My_Attribute.addr=0;
					GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,0);
					GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);				
					stop_voice_play();
					rt_kprintf("recv Com_Room end tall back\n");
//				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm|Rest_Comm))  //报警复位反馈
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room rest  back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com|Rest_Comm))  //手动检测反馈
			{
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room check  back\n");
				}
			}
			break;
		
		case 0x00:    //主发信息
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm))  //呼叫
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from | (Duty_Room | Lift_Addr | Car_roof | Pit_Addr)) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))
				{
					rt_kprintf("recv call from Com_Room\n");
          /*反馈机房主机*/
					while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Call_Comm | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
					if((k1_lin_temp==0) && (My_Attribute.state==Stand_State))   //挂机状态
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
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down))  //挂机
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{				
          /*反馈机房主机*/
					while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Take_down | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
					rt_kprintf("recv end tall from Com_Room\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm))  //报警复位
			{
				;
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com))  //手动检测
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