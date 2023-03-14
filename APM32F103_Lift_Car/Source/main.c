#include "main.h"

#define  HERAT_TIME     3

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
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=Dutt_Room;
						my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=Aux_Call_DUT;     						
						my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
						rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
						rt_kprintf("key1 is end\n");  
						x1_lin_time=0;  open_rest();
					}else{
						x1_lin_time=0;
					}
				}
				if(My_Attribute.state==Call_State)
				{
					ring_time++;
					if(ring_time>=MAX_Rint_Rime){
						ring_time=0;
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,0);
						
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
						My_Attribute.state=Stand_State;
					}			
				}					
        rt_thread_delay(100); time_cnt++;
				if(time_cnt>HERAT_TIME){
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
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
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
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
			if(My_Attribute.state==Stand_State){
				start_voice_play();
				My_Attribute.state=Tall_State;
			}				
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room)//机房主动发起命令
			{
				start_voice_play();
				My_Attribute.state=Tall_State;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Com_Call_Aux;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
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
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)  //值班室命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Dut_Call_Aux;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
				if(My_Attribute.state==Stand_State){
					start_voice_play();
					My_Attribute.state=Tall_State;
				}				
			}
			break;
		
		case Take_Down://挂机命令	
      			
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room)  //机房挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.com_talk=0;   rt_kprintf("My_Attribute.dut_talk=%d\n",My_Attribute.dut_talk);
				Recv_Can_buff[0].msg_input=Recv_Can_buff[0].msg_output=0;
				if(My_Attribute.dut_talk==0){
					My_Attribute.car_talk=0;My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Dutt_Room)//值班室挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址	
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.dut_talk=0;   rt_kprintf("My_Attribute.com_talk=%d\n",My_Attribute.com_talk);
				rt_kprintf("My_Attribute.dut_talk=%d\n",My_Attribute.dut_talk);
				Recv_Can_buff[4].msg_input=Recv_Can_buff[4].msg_output=0;
				if(My_Attribute.com_talk==0){
					My_Attribute.car_talk=0;My_Attribute.lif_talk=0;  My_Attribute.pit_talk=0;
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}				  
			}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Lift_Addr))  //轿顶挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.car_talk=0;
				Recv_Can_buff[3].msg_input=Recv_Can_buff[3].msg_output=0;
				if((My_Attribute.com_talk==0) && (My_Attribute.dut_talk==0) && (My_Attribute.pit_talk==0)){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}			  
			}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Pit_Addr) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Lift_Addr))  //底坑挂机命令
			{
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =Take_Down;
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Pit_Addr;	
				my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
        my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;				
				rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
				My_Attribute.pit_talk=0; 
        Recv_Can_buff[2].msg_input=Recv_Can_buff[2].msg_output=0;				
				if((My_Attribute.com_talk==0) && (My_Attribute.dut_talk==0) && (My_Attribute.car_talk==0)){
					stop_voice_play();
					My_Attribute.state=	Stand_State;
				}			  
			}
			break;
		
		case Rst_Comm://报警终止
			stop_rest();
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=Lift_Addr;	
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
#if 0	
	switch(msg->Can_Msg[msg->Msg_Out].msg.head)
	{
		case 0x01:    //反馈信息
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm|Rest_Comm))  //呼叫反馈
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Lift_Addr))    //收到机房的呼叫反馈
				{
					if(My_Attribute.state==Stand_State){   			//待机状态
						GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,0);
						My_Attribute.state=Call_State;
					}else if(My_Attribute.state==Call_State){   //呼叫状态
						;
					}else if(My_Attribute.state==Tall_State){   //通话状态
						;
					}					
					rt_kprintf("recv Com_Room call back\n");
				}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Duty_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Lift_Addr))   //收到值班室的呼叫反馈
				{
					if(My_Attribute.state==Stand_State){   			//待机状态
						GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,1);
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,0);
						My_Attribute.state=Call_State;
					}else if(My_Attribute.state==Call_State){   //呼叫状态
						;
					}else if(My_Attribute.state==Tall_State){   //通话状态
						;
					}					
					rt_kprintf("recv Duty_Room call back\n");				  
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down|Rest_Comm))  //挂机反馈
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room)
				{
					stop_voice_play();
					rt_kprintf("recv Com_Room end tall back\n");
				}
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
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room) || (msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room))
				{
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Call_Comm | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
					/**/	
          if(My_Attribute.state==Stand_State){
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;
					}else if(My_Attribute.state==Call_State){
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);							
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;
					}else if(My_Attribute.state==Tall_State){
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);							
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;
					}else if(My_Attribute.state==Tall_State_DUTH){
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);							
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State_DUTH;
					}				
					rt_kprintf("recv call from Com_Room\n");				
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down))  //挂机
			{  
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (My_Attribute.state==Tall_State))  //机房挂机
				{				
          /*反馈机房主机*/
//					while(my_usart_msg.msg_output!=my_usart_msg.msg_input){rt_thread_delay(10);}
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Take_down | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
					stop_voice_play();
					GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
					GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,0);
					My_Attribute.state=	Stand_State;					
					rt_kprintf("recv end tall from Com_Room\n");
				}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Duty_Room) && (My_Attribute.state==Tall_State_DUTH))  //值班室挂机
				{
          /*反馈值班室主机*/
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Take_down | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
					stop_voice_play();
					GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
					GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,0);
					My_Attribute.state=	Stand_State;					
					rt_kprintf("recv end tall from Duty_Room\n");				  
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm))  //报警复位
			{
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Rst_Comm | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
					GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,0);				
					rt_kprintf("recv end tall from Com_Room\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com))  //手动检测
			{
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(Check_Com | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(RES_CALL))  //呼叫应答
			{
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.head=0x01;
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd =(RES_CALL | Rest_Comm);
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=msg->Can_Msg[msg->Msg_Out].msg.msg_to;	
					my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=msg->Can_Msg[msg->Msg_Out].msg.msg_from;  //机房地址									
					my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
					rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/				
				  if(My_Attribute.state==Call_State){
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);						
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;						
					}else if(My_Attribute.state==Stand_State){	
						start_voice_play();
						GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,1);
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;							
					}else if(My_Attribute.state==Tall_State){  //和机房通话
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State;					
					}else if(My_Attribute.state==Tall_State_DUTH){  //和值班室通话
            if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Duty_Room)						
							My_Attribute.state=	Tall_State_DUTH;
						else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from == Com_Room)						
							My_Attribute.state=	Tall_State_DUTH;					
					}					
			}			
			break;
		
		default:
			break;
	}
#endif
}

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}