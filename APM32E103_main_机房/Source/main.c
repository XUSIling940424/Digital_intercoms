#include "main.h"

#define  MAX_TIME_OUT    4

/*定义线程控制块指针*/
static rt_thread_t msg_thread = RT_NULL;
	
/*函数声明*/
static void msg_thread_entry(void *parameter);
static  void  remote_msg_handle(struct Remote_Msg * msg);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13级*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63级*/

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
					if(x1_lin_time==MAX_Time_Out)
					{ 
						/*呼叫值班室*/
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd=Com_Call_Dut;
						my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/						
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
						/*复位电梯内部*/
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Rst_Comm;
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
						/*复位值班室*/
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd=Rst_Comm;
						my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/
            x2_lin_time=0;						
					}else{
						x2_lin_time=0;
					}
				}
				/*按键3检测*/
			  if(X3==0){
					if(x3_lin_time<MAX_Time_Out)
						x3_lin_time++;
					else
						x3_lin_time=MAX_Time_Out;
					
				}else{
					if(x3_lin_time==MAX_Time_Out)
					{ 
					 x3_lin_time=0;
					 if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0)  //报警过滤打开
					 {
						 wiz_ALM_C(1);
              /*报警过滤上传*/
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
              my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=ALA_FILT;
						  my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=0x01;
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						  rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/							 
					 
					 }else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1)  //报警过滤关闭
					 {
						 /*报警过滤上传*/
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=ALA_FILT;
						 my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=0x02;
						 my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						 rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/							 
					   wiz_ALM_C(0);
					 }
						
					}else{
						x3_lin_time=0;
					}
				}				
				
				/*挂机键检测*/
				if(K1==0){   /*挂机状态*/
					if(k1_lin_temp==1){   /*挂机动作*/
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Take_Down;							
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/										
						rt_kprintf("send Take_Down cmd\n");
						stop_voice_play();  My_Attribute.state=Stand_State;
						My_Attribute.com_talk=0;  My_Attribute.car_talk=0; My_Attribute.dut_talk=0; My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
					}
					k1_lin_time=0;							
					k1_lin_temp=0;						
				}else if(K1==1)  /*摘机状态*/
				{
					if(k1_lin_time<MAX_Time_Out)
						k1_lin_time++;
					else{
						if(k1_lin_temp==0){   /*摘机动作*/
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Com_Room;	
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=ALL_AUX__Room;
							my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Com_Call_Aux;							
							my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
							rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
							rt_kprintf("send Com_Call_Aux cmd\n");						
						}
					  k1_lin_temp=1;
					  k1_lin_time=MAX_Time_Out;
					}
				}
		/*状态检测*/
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
    /*在线检测*/
		time_cnt++;
		if(time_cnt>MAX_TIME_OUT){
			My_Attribute.car_line_temp++;
			My_Attribute.lif_line_temp++;
			My_Attribute.pit_line_temp++;
			if(My_Attribute.car_line_temp>=MAX_TIME_OUT/2){  //轿顶超时未接收心跳
				My_Attribute.car_line=0;
				My_Attribute.car_line_temp=0;
			}
			if(My_Attribute.lif_line_temp>=MAX_TIME_OUT/2){  //轿厢超时未接收心跳
				My_Attribute.lif_line=0;
				My_Attribute.lif_line_temp=0;
			}
			if(My_Attribute.pit_line_temp>=MAX_TIME_OUT/2){  //底坑超时未接收心跳
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

			
			if(My_Attribute.car_line==1){   //轿顶在线
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x01;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfe;
			}
			if(My_Attribute.lif_line==1){   //轿厢在线
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x02;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfd;
			}
			if(My_Attribute.pit_line==1){   //底坑在线
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x04;
			}else{
			  my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data &= 0xfb;
			}			
			my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data |= 0x08;  //机房
			my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/
      time_cnt=0;			  
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
#if 1		
		/*usart消息检测*/
		if(my_usart_msg.msg_output!=my_usart_msg.msg_input){
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
		}
#endif
#if 1		
    /*can2语音消息检测*/
   	if(can2_send_buff.input != can2_send_buff.output){   
		  rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/
		}
#endif		
    /*检测can命令*/
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

/*命令处理函数*/
static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
 
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://副机呼叫机房信号
			rt_kprintf("recv AUX_Call_Com\n");
			/*反馈信息*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =AUX_Call_Com; 
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
			if(My_Attribute.state==Stand_State){//待机状态
				 My_Attribute.state=Call_State;
				 open_call();
			}else{
				 ;
			}
      open_rest();			
			break;
		
		case Com_Call_Aux://机房呼叫内部副机信号
			stop_call();
			start_voice_play();
			My_Attribute.state=Tall_State;
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号
			/*转发值班室*/
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr)){  //轿顶和底坑发送呼叫信号
				if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1){  //关闭报警过滤
					rt_kprintf("recv Aux_Call_DUT from Car_roof | Pit_Addr\n");
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/	
				}else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0){  //打开报警过滤
					open_rest();
					/*反馈信息*/
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Aux_Call_DUT; 
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
					if(My_Attribute.state==Stand_State){//待机状态
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}				
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)  //轿厢主动发起呼叫信号
			{
				open_rest();
				if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==1){  //关闭报警过滤
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
					my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/	
					/*改变状态*/
					if(My_Attribute.state==Stand_State){//待机状态
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}
				}else if(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)==0){  //打开报警过滤
					/*反馈信息*/
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from; 		
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Aux_Call_DUT; 
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
					if(My_Attribute.state==Stand_State){//待机状态
						 My_Attribute.state=Call_State;
						 open_call();
					}else{
						 ;
					}				
				}				
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) {  //值班室主动发送
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
        rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
        rt_kprintf("recv Aux_Call_DUT from Dutt_Room");			  
			}		
			break;
		
		case Com_Call_Dut://机房呼叫值班室
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //值班室反馈信息
			{
				if(My_Attribute.state==Stand_State){//待机状态
					start_voice_play();
					My_Attribute.state=Tall_State;
				}else{
					 ;
				}				
			}				
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
			open_rest();
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //值班室主动发送
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
				if(My_Attribute.state==Stand_State){//待机状态
					 My_Attribute.state=Call_State;
					 open_call();
				}else{
					 ;
				}					
			}else {  //副机反馈
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/			  
			}				
			break;
		
		case Take_Down://挂机命令
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //值班室主动发送
			{rt_kprintf("recv Take_Down from Dutt_Room\n");
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //轿顶的挂机信号
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/					
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr){  //底坑的挂机信号
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/					
			}			
			break;
		
		case Rst_Comm://报警终止
			stop_rest();
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==ALL_AUX__Room))  //值班室主动发送,复位副机
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 			
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
			}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))  //机房发送后值班室的反馈信号
			{
			
			}else if(((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)) \
			  &&(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Dutt_Room))	//副机反馈值班室信号
			{
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_from;	
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_to; 				
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.cmd =msg->Can_Msg[msg->Msg_Out].msg.cmd;
				my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_In].msg.data=msg->Can_Msg[msg->Msg_Out].msg.data;		
				my_can2_send_remote_msg.Msg_In=(my_can2_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
				rt_sem_release(my_can2_needsend_sem);  /*释放信号量,开启硬件发送*/			  
			}else if(((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)) \
			  &&(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Com_Room))	//副机反馈机房信号
			{	
				;
			}			
			break;
		
		case Check_Com://手动检测
			break;
		
		case HearBeat://心跳包上传
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof)  //轿顶心跳包
			{
			  My_Attribute.car_line_temp=0;   My_Attribute.car_line=1;
			}
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr)  //底坑心跳包
			{
			  My_Attribute.pit_line_temp=0;  My_Attribute.pit_line=1;
			}
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr)  //轿厢心跳包
			{
			  My_Attribute.lif_line_temp=0;  My_Attribute.lif_line=1;
			}					
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

#if 0
		switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
		{
		  case AUX_Call_Com://副机呼叫机房信号
				break;
			case Com_Call_Aux://挂机信号
				break;
			case Aux_Call_DUT://副机呼叫值班室信号
				break;
			case Com_Call_Dut://机房呼叫值班室
				break;	
			case Dut_Call_Aux://值班室呼叫副机
				break;
			case Take_Down://挂机命令
				break;
			case Rst_Comm://报警终止
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

#endif

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}