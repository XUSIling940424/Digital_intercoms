#include "main.h"

#define  HERAT_TIME    3

/*定义线程控制块指针*/
static rt_thread_t msg_thread = RT_NULL;
	
/*函数声明*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13级*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63级*/

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
#if 1		
    while (1)
    {
			  /*按键1检测*/
			  if(X1==0){
					if(x1_lin_time<MAX_Time_Out)
						x1_lin_time++;
					else
						x1_lin_time=MAX_Time_Out;			
				}else{
					if(x1_lin_time>=MAX_Time_Out)
					{ 					
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
						if(My_Attribute.state==Stand_State)//待机状态
						{
							open_rest();
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Com_Room;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=AUX_Call_Com;   //副机呼叫机房  
						}else if(My_Attribute.state==Tall_State)//通话状态
						{
							My_Attribute.state=Stand_State;
							stop_voice_play();
							My_Attribute.com_talk=0;  My_Attribute.car_talk=0; My_Attribute.dut_talk=0; My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Lift_Addr;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_Down;   //副机挂机信号 						  
						}						
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
					if(x2_lin_time>=MAX_Time_Out)
					{ 
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Aux_Call_DUT;   //副机呼叫值班室
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/						
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
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=HearBeat;   //心跳包上传
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
          time_cnt=0;					
				}
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
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 0, 0);

	while(1){				
#if 1		
		/*usart消息检测*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
		}	
#endif		
    /*检测can命令*/
		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
			remote_msg_handle(&my_remote_msg); 
      rt_memset(my_remote_msg.Can_Msg,0,sizeof(my_remote_msg.Can_Msg));			
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		rt_thread_delay(10);
	}
}

/*命令处理函数*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
//	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
//	
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://副机呼叫机房信号
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}				
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) //机房主动发起
			{				
				start_voice_play();
				My_Attribute.state=Tall_State;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Com_Call_Aux;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
				rt_kprintf("send Com_Call_Aux black\n");
			}else {
			  ;
			}				
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号	
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}				
			break;
		
		case Com_Call_Dut://机房呼叫值班室
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}			
			break;
		
		case Take_Down://挂机命令				
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room)  //机房挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.com_talk=0;  rt_kprintf("My_Attribute.dut_talk=%d\n",My_Attribute.dut_talk);
				if(My_Attribute.dut_talk==0){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)//值班室挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.dut_talk=0;  rt_kprintf("My_Attribute.com_talk=%d\n",My_Attribute.com_talk);
				if(My_Attribute.com_talk==0){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}				  
			}
			break;
		
		case Rst_Comm://报警终止
			stop_rest();
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Car_roof;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;		
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Rst_Comm;
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
			break;
		
		case Check_Com://手动检测
			break;
		
		case HearBeat://心跳包上传
			break;
		
		case ALA_FILT://机房报警过滤上传
			break;
		
		case Elec_DULER://电池欠压
			break;
		
		case VOILD_Chang://音量调节
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