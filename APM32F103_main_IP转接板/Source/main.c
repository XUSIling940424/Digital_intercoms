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


uint8  local_ips[4]={0};
	

int main(void)
{	
	uint8_t client_send_buf[10]={0};	  uint8_t ret=0;   uint16_t   time_cnt=0;
	
	client_send_buf[0]=0x00;client_send_buf[1]=0x05;client_send_buf[2]=HearBeat;
	client_send_buf[3]=0x00;
	client_send_buf[4]=0x00;
	client_send_buf[5]=0;
	/*读取地址信息*/
	local_ip[3]=Read_Addr_Msg();	
	/*网络初始化*/
	 reset_w5500(); 		                    // w5500硬件复位 
	 PHY_check();                          // 网线检测程序	
	 set_w5500_mac();                      // 设置w5500MAC地址
	 set_w5500_ip();                      // 设置w5500网络参数
	 setRTR(500);                         //设置超时时间 100us*time_out  100us*500=50ms
	 setRCR(3);                           //设置重传次数	
	 socket_buf_init(txsize, rxsize);      // 初始化4个Socket的发送接收缓存大小	
	 GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
	 init_data();
	 build_can_recv_thread();
	 build_can_send_thread();
	 build_my_net_thread();

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
    while (1)
    {
			GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,!GPIO_ReadOutputBit(GPIOB,GPIO_PIN_9));	

			if((ret==0) && (time_cnt>=100)){
				client_send_buf[4]=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			  ret=tcp_client_send(SOCK_TCPC_2,6669,remote_ip,6666,client_send_buf,6);
				time_cnt=0;
				if(ret==0)  GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
				else GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,0);
			}
			if((ret==1) && (time_cnt>=200)){
				client_send_buf[4]=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			  ret=tcp_client_send(SOCK_TCPC_2,6669,remote_ip,6666,client_send_buf,6);
				time_cnt=0;
				if(ret==0)  GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
				else GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,0);
			}
			
			rt_thread_delay(100);   time_cnt++;
    }	
}


/*线程定义*/
static void msg_thread_entry(void *paramter)
{
	int   i=0;
	int 	ret=0;
	
  
	Can1_Init();
	while(1){	
		
    /*检测can命令*/
		if(my_remote_msg.Msg_Out!=my_remote_msg.Msg_In){
			remote_msg_handle(&my_remote_msg);  
			my_remote_msg.Msg_Out=((my_remote_msg.Msg_Out+1)%MAX_MSG_NUM);
		}
		/*检测网络语音*/
		if(udp_7777_recv_buff.msg_input!=udp_7777_recv_buff.msg_output){
		  rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/
		}
		rt_thread_delay(10);
	}
}


/*命令处理函数*/

static  void  remote_msg_handle(struct Remote_Msg * msg)
{
	uint8_t client_send_buf[10]={0};
  uint8_t ret=0;	
	
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_from=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_from);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.msg_to=%d\n",msg->Can_Msg[msg->Msg_Out].msg.msg_to);
	rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.cmd=%d\n",msg->Can_Msg[msg->Msg_Out].msg.cmd);
	
	switch(msg->Can_Msg[msg->Msg_Out].msg.cmd)
	{
		case AUX_Call_Com://副机呼叫机房信号			
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号	
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv Com_Room rest  back\n");			
			break;
		
		case Com_Call_Dut://机房呼叫值班室
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv Com_Call_Dut\n");				
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
			rt_kprintf("recv Dut_Call_Aux rest  back\n");
			break;
		
		case Take_Down://挂机命令		
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room){  //来自机房挂机
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //来自轿顶挂机
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}	else if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Car_roof){  //来自底坑挂机
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			}
      rt_kprintf("recv Take_Down\n");			
			break;
		
		case Rst_Comm://报警终止
			if(msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room){  //来自机房复位请求
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv Com_Room rest  back\n");
			}else{
			 ;
			}				
			break;
		
		case Check_Com://手动检测			
			break;
		
		case HearBeat://心跳包上传
			rt_kprintf("msg->Can_Msg[msg->Msg_Out].msg.data=%x\n",msg->Can_Msg[msg->Msg_Out].msg.data);
		  if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x01){  //轿顶在线
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,0);
				My_Attribute.car_line=1;
			}else{
				My_Attribute.car_line=0;
			  GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x02){  //轿厢在线
				My_Attribute.lif_line=1;
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,0);			 
			}else{
				My_Attribute.lif_line=0;
				GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,1);	
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x04){  //底坑在线
			  My_Attribute.pit_line=1;
			}else{
			  My_Attribute.pit_line=0;
			}if(msg->Can_Msg[msg->Msg_Out].msg.data & 0x08){  //机房在线
			   My_Attribute.com_line=1;
			}else{
			   My_Attribute.com_line=0;
			} 				
			break;
		
		case ALA_FILT://机房报警过滤上传
			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
			rt_kprintf("recv ALA_FILT\n");				
			break;
		
		case Elec_DULER://电池欠压
			break;
		
		case VOILD_Chang://音量调节
//			client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
//			client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
//			client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
//		  client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.data>>8;
//      client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
//			tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
//			rt_kprintf("recv Com_Room rest  back\n");					
//			break;
		
		default:
			break;
	}	

#if 0
	
	switch(msg->Can_Msg[msg->Msg_Out].msg.head)
	{
		case 0x01:    //反馈信息
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm|Rest_Comm))  //呼叫反馈
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room)
				{
					rt_kprintf("recv Duty_Room call back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down|Rest_Comm))  //挂机反馈
			{
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm|Rest_Comm))  //报警复位反馈
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv Com_Room rest  back\n");
				}else if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
					rt_kprintf("recv Com_Room rest  back\n");
				}				
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com|Rest_Comm))  //手动检测反馈
			{
			if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Lift_Addr) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
					rt_kprintf("recv Com_Room check  back\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(RES_CALL|Rest_Comm))  //呼叫应答反馈
			{
				if(msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room)
				{
					rt_kprintf("recv Duty_Room call back\n");
				}
			}
			break;
		
		case 0x00:    //主发信息
			if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Call_Comm))  //呼叫
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from | (Com_Room | Lift_Addr | Car_roof | Pit_Addr)) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{
					rt_kprintf("recv call from Com_Room\n");
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Take_down))  //挂机
			{
				if((msg->Can_Msg[msg->Msg_Out].msg.msg_from==Com_Room) && (msg->Can_Msg[msg->Msg_Out].msg.msg_to==Duty_Room))
				{							
					client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
					client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
					client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
					client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
					client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
					client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
					tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
					rt_kprintf("recv end tall from Com_Room\n");
				}
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Rst_Comm))  //报警复位
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);				
				rt_kprintf("recv end tall from Com_Room\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(Check_Com))  //手动检测
			{
				rt_kprintf("recv end tall from Com_Room\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(ADD_TALK))		 //加入通话		
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv add tall\n");
			}else if(msg->Can_Msg[msg->Msg_Out].msg.cmd==(TAKE_UP))		 //机房摘机信号	
			{
				client_send_buf[0]=msg->Can_Msg[msg->Msg_Out].msg.head;
				client_send_buf[1]=msg->Can_Msg[msg->Msg_Out].msg.cmd;
				client_send_buf[2]=msg->Can_Msg[msg->Msg_Out].msg.msg_from;
				client_send_buf[3]=msg->Can_Msg[msg->Msg_Out].msg.msg_to;
				client_send_buf[4]=msg->Can_Msg[msg->Msg_Out].msg.data;
				client_send_buf[5]=msg->Can_Msg[msg->Msg_Out].msg.zeros;
				tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buf,6);
				rt_kprintf("recv take up \n");
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