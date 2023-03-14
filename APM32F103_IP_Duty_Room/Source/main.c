#include "main.h"

/*定义线程控制块指针*/
static rt_thread_t msg_thread = RT_NULL;
	
/*函数声明*/
static void msg_thread_entry(void *parameter);
static unsigned char voice_chang1[5]={0xef,0xfa,0x04,0x01,0x06};  /*0-13级*/
static unsigned char voice_chang2[5]={0xef,0xfa,0x05,0x01,0x20};  /*0-63级*/

static  void  remote_msg_handle(struct Net_Port_Msg * msg);
static void usart3_msg_handle(unsigned char *buf,int len);
static int ReviceComm(struct Usart_Comm *com ,unsigned char buf );

int hold_lin_time=0;
char hold_temp=0;

int x0_lin_time=0;
int x1_lin_time=0;
int x2_lin_time=0;
int x3_lin_time=0;
int x4_lin_time=0;

int main(void)
{	
	 struct Usart_Comm  my_usart3_comm;
	
	 my_usart3_comm.Head[0]=0xff;  my_usart3_comm.Head[1]=0x55; my_usart3_comm.Head[2]=0x00;
	 /*网络初始化*/
	 reset_w5500(); 		                    // w5500硬件复位 
	 PHY_check();                          // 网线检测程序	
	 set_w5500_mac();                      // 设置w5500MAC地址
	 set_w5500_ip();                      // 设置w5500网络参数
	 setRTR(500);                         //设置超时时间 100us*time_out  100us*200=50ms
	 setRCR(3);                           //设置重传次数
	 socket_buf_init(txsize, rxsize);      // 初始化4个Socket的发送接收缓存大小	
	
	 init_data();
   build_usart_recv_thread();
	 build_usart3_recv_thread();
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
#if 1		
    while (1)
    {
			/*手柄检测*/
			if(HOLD==0){  //挂机状态
				if(hold_lin_time<MAX_Time_Out)  hold_lin_time++;
				else{
				 if(hold_temp==1){  //挂机动作
					 my_usart3_comm.Comm=HANDLE_TAKE_DOWN;  my_usart3_comm.DatLen=0x04;
					 rt_memset(my_usart3_comm.Data,0,4);
					 usart3_send_comm(&my_usart3_comm);
				 }
				 hold_lin_time=MAX_Time_Out;  hold_temp=0;
				}				
			}else if(HOLD==1){  //摘机状态
				if(hold_lin_time<MAX_Time_Out)  hold_lin_time++;
				else{
				 if(hold_temp==0){  //摘机动作
					 my_usart3_comm.Comm=HANDLE_TAKE_UP;  my_usart3_comm.DatLen=0x04;
					 rt_memset(my_usart3_comm.Data,0,4);
           usart3_send_comm(&my_usart3_comm);					 
				 }
				 hold_lin_time=MAX_Time_Out;  hold_temp=1;
				}
			}
     /*按键检测*/
		if((~R_X &(0x03e0))!=0){  //有按钮按下
			if((~R_X &(0x03e0))==0x0200){  //x0
				if(x0_lin_time<MAX_Time_Out) x0_lin_time++;
					else x0_lin_time=MAX_Time_Out;				
			}else if((~R_X &(0x03e0))==0x0100){ //x1
				if(x1_lin_time<MAX_Time_Out) x1_lin_time++;
					else x1_lin_time=MAX_Time_Out;
			}else if((~R_X &(0x03e0))==0x0080){ //x2
				if(x2_lin_time<MAX_Time_Out) x2_lin_time++;
					else x2_lin_time=MAX_Time_Out;				
			}else if((~R_X &(0x03e0))==0x0040){ //x3
				if(x3_lin_time<MAX_Time_Out) x3_lin_time++;
					else x3_lin_time=MAX_Time_Out;				
			}else if((~R_X &(0x03e0))==0x0020){ //x4
				if(x4_lin_time<MAX_Time_Out) x4_lin_time++;
					else x4_lin_time=MAX_Time_Out;				
			}	
		}else{
		  if(x0_lin_time>=MAX_Time_Out){
			  my_usart3_comm.Comm=KEY0_KEEP;  my_usart3_comm.DatLen=0x04;
			  rt_memset(my_usart3_comm.Data,0,4);
			  usart3_send_comm(&my_usart3_comm);				
				rt_kprintf("x0\n");
			}else if(x1_lin_time>=MAX_Time_Out){
			  my_usart3_comm.Comm=KEY1_KEEP;  my_usart3_comm.DatLen=0x04;
			  rt_memset(my_usart3_comm.Data,0,4);
			  usart3_send_comm(&my_usart3_comm);				
				rt_kprintf("x1\n");
			}else if(x2_lin_time>=MAX_Time_Out){
			  my_usart3_comm.Comm=KEY2_KEEP;  my_usart3_comm.DatLen=0x04;
			  rt_memset(my_usart3_comm.Data,0,4);
			  usart3_send_comm(&my_usart3_comm);				
				rt_kprintf("x2\n");
			}else if(x3_lin_time>=MAX_Time_Out){
			  my_usart3_comm.Comm=KEY3_KEEP;  my_usart3_comm.DatLen=0x04;
			  rt_memset(my_usart3_comm.Data,0,4);
			  usart3_send_comm(&my_usart3_comm);				
				rt_kprintf("x3\n");
			}else if(x4_lin_time>=MAX_Time_Out){
			  my_usart3_comm.Comm=KEY4_KEEP;  my_usart3_comm.DatLen=0x04;
			  rt_memset(my_usart3_comm.Data,0,4);
			  usart3_send_comm(&my_usart3_comm);				
				rt_kprintf("x4\n");
			}
      x0_lin_time=0; x1_lin_time=0; x2_lin_time=0; x3_lin_time=0; x4_lin_time=0;			
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
	struct Usart_Comm  my_usart3_comm;
	
	my_usart3_comm.Head[0]=0xff;  my_usart3_comm.Head[1]=0x55; my_usart3_comm.Head[2]=0x00;
	
  /*音量调节*/
	CAN_EnableInterrupt(CAN1, CAN_INT_F0MP );  		
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 0, 1);
	while(1){				
		/*TCP server 6667 port消息检测*/
		if(ser_6667_recv.msg_input!=ser_6667_recv.msg_output){   //接收到新消息
			remote_msg_handle(&ser_6667_recv);
			ser_6667_recv.msg_output=(ser_6667_recv.msg_output+1)%MAX_NET_MSG_NUM;
		}
		/*TCP server 6666 port消息检测*/
		if(ser_6666_recv.msg_input!=ser_6666_recv.msg_output){   //接收到新消息
			remote_msg_handle(&ser_6666_recv);
			ser_6666_recv.msg_output=(ser_6666_recv.msg_output+1)%MAX_NET_MSG_NUM;
		}		
		/*屏幕消息检测*/
		if(my_usart3_msg.msg_input!=my_usart3_msg.msg_output){
		  usart3_msg_handle(my_usart3_msg.msg_buff[my_usart3_msg.msg_output].msg_buff,my_usart3_msg.msg_buff[my_usart3_msg.msg_output].msg_len);
			my_usart3_msg.msg_output=((my_usart3_msg.msg_output+1)%(MAX_USART_MSG_NUM));
		}
		/*状态检测*/
		if(My_Attribute.com_talk==1){  //机房通话状态
			if(My_Attribute.com_talk_temp==0){
				my_usart3_comm.Comm=TALL_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=remote_ip[3];
				my_usart3_comm.Data[1]=Com_Room;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);  rt_thread_delay(100);
			}else if(My_Attribute.com_talk_temp<250){
			  My_Attribute.com_talk_temp++;
			}else{
			  My_Attribute.com_talk_temp=250;
			}
		}
		if(My_Attribute.car_talk==1){  //轿顶通话状态
			if(My_Attribute.car_talk_temp==0){
				my_usart3_comm.Comm=TALL_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=remote_ip[3];
				my_usart3_comm.Data[1]=Car_roof;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);  rt_thread_delay(100);
			}else if(My_Attribute.car_talk_temp<250){
			  My_Attribute.car_talk_temp++;
			}else{
			  My_Attribute.car_talk_temp=250;
			}
		}
		if(My_Attribute.pit_talk==1){  //底坑通话状态
			if(My_Attribute.pit_talk_temp==0){
				my_usart3_comm.Comm=TALL_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=remote_ip[3];
				my_usart3_comm.Data[1]=Pit_Addr;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);  rt_thread_delay(100);
			}else if(My_Attribute.pit_talk_temp<250){
			  My_Attribute.pit_talk_temp++;
			}else{
			  My_Attribute.pit_talk_temp=250;
			}
		}
		if(My_Attribute.lif_talk==1){  //轿厢通话状态
			if(My_Attribute.lif_talk_temp==0){
				my_usart3_comm.Comm=TALL_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=remote_ip[3];
				my_usart3_comm.Data[1]=Lift_Addr;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);  rt_thread_delay(100);
			}else if(My_Attribute.lif_talk_temp<250){
			  My_Attribute.lif_talk_temp++;
			}else{
			  My_Attribute.lif_talk_temp=250;
			}
		}		
		rt_thread_delay(200);		
	}
}

/*网口命令处理函数*/
static  void  remote_msg_handle(struct Net_Port_Msg * msg)
{
	struct Usart_Comm  my_usart3_comm;
	unsigned char client_send_buff[10]={0};
	
	my_usart3_comm.Head[0]=0xff;  my_usart3_comm.Head[1]=0x55; my_usart3_comm.Head[2]=0x00;
	
 switch(msg->msg_buff[msg->msg_output].cmd)
 {
		case AUX_Call_Com://副机呼叫机房信号			
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号	
			my_usart3_comm.Comm=EXTEN_CALL;   	my_usart3_comm.DatLen=0x04;
			my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
			my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
			my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
			usart3_send_comm(&my_usart3_comm); 
  		rt_kprintf("recv Aux_Call_DUT\n");			
			break;
		
		case Com_Call_Dut://机房呼叫值班室
			my_usart3_comm.Comm=EXTEN_CALL;   	my_usart3_comm.DatLen=0x04;
			my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
			my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
			my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
			usart3_send_comm(&my_usart3_comm); 
  		rt_kprintf("recv Com_Call_Dut\n");			
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
			break;
		
		case Take_Down://挂机命令
      if(msg->msg_buff[msg->msg_output].msg_from==Com_Room){  //机房挂机信号
				my_usart3_comm.Comm=TAKE_DOWN_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
				my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);
        My_Attribute.com_talk=0;  My_Attribute.com_talk_temp=0;
			}else if(msg->msg_buff[msg->msg_output].msg_from==Car_roof){  //轿顶挂机信号
				my_usart3_comm.Comm=TAKE_DOWN_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
				my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);				
			}else if(msg->msg_buff[msg->msg_output].msg_from==Pit_Addr){  //底坑挂机信号
				my_usart3_comm.Comm=TAKE_DOWN_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
				my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);
        				
			}else if(msg->msg_buff[msg->msg_output].msg_from==Lift_Addr){  //轿厢挂机信号
				my_usart3_comm.Comm=TAKE_DOWN_STAT;   	my_usart3_comm.DatLen=0x04;
				my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
				my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
				my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
				usart3_send_comm(&my_usart3_comm);
        My_Attribute.lif_talk=0;  My_Attribute.lif_talk_temp=0;				
			}	
			rt_kprintf("recv Take_Down\n");				
			break;
		
		case Rst_Comm://报警终止
			my_usart3_comm.Comm=COMPUTER_ROOM_REST;   	my_usart3_comm.DatLen=0x04;
			my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
			my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].msg_from;
			my_usart3_comm.Data[2]=0x00; my_usart3_comm.Data[3]=0x00;
			usart3_send_comm(&my_usart3_comm); 
  		rt_kprintf("recv Rst_Comm\n");	
			break;
		
		case Check_Com://手动检测
			break;
		
		case HearBeat://心跳包上传
			my_usart3_comm.Comm=HEART_KEEP;   	my_usart3_comm.DatLen=0x04;
			my_usart3_comm.Data[0]=msg->msg_buff[msg->msg_output].client_ip[3];
			my_usart3_comm.Data[1]=msg->msg_buff[msg->msg_output].data>>8;
			my_usart3_comm.Data[2]=msg->msg_buff[msg->msg_output].data; my_usart3_comm.Data[3]=0x00;
			usart3_send_comm(&my_usart3_comm); 
  		rt_kprintf("recv HEART_KEEP data =%x \n",msg->msg_buff[msg->msg_output].data);	
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

/*屏幕消息命令处理*/
static void usart3_msg_handle(unsigned char *buf,int len)
{
	int i=0;
	struct Usart_Comm  my_usart3_comm;
	struct Usart_Comm  my_usart3send_comm;
	unsigned char client_send_buff[10]={0};
	unsigned char usart2_send_buff[20]={0};
	
	my_usart3send_comm.Head[0]=0xff;  my_usart3send_comm.Head[1]=0x55; my_usart3send_comm.Head[2]=0x00;
	my_usart3_comm.idx=0;
	
  for(i=0;i<len;i++){
	  if(ReviceComm(&my_usart3_comm,buf[i]))
		{
		   if(my_usart3_comm.Comm>0x80){   //屏幕反馈信息
					my_usart3_comm.Comm=(my_usart3_comm.Comm-0x80);
			    switch(my_usart3_comm.Comm)
					{
						case EXTEN_CALL://分机呼叫					
							break;
						
						case HANDLE_TAKE_DOWN://手柄挂机
							break;	

						case HANDLE_TAKE_UP://手柄摘机
							break;	

						case EXTEN_ADD_TALK://加入通话
							break;	

						case COMPUTER_ROOM_REST://机房复位
							break;	

						case COMPUTER_ROOM_TAKE_DOWN://机房挂机						
							break;
						
						default:
							break;
					}        				 
			 }else{                          //屏幕主发信息
			    switch(my_usart3_comm.Comm)
					{
						case SCREE_CALL:rt_kprintf("SCREE_CALL\n");//屏幕呼叫 
						  client_send_buff[0]=Dutt_Room;
						  client_send_buff[1]=ALL_AUX__Room;
						  client_send_buff[2]=Dut_Call_Aux;
						  client_send_buff[3]=my_usart3_comm.Data[1];
						  client_send_buff[4]=my_usart3_comm.Data[0];
						  client_send_buff[5]=0x00;
						  remote_ip[3]=my_usart3_comm.Data[0]; 						
						  if(tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buff,6)==0){  //呼叫失败
								my_usart3send_comm.Comm=SCREE_CALL | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=my_usart3_comm.Data[1];		my_usart3send_comm.Data[2]=0x02;	
								usart3_send_comm(&my_usart3send_comm);							
								rt_kprintf("SCREE_CALL send black\n");					
							}else {
								my_usart3send_comm.Comm=SCREE_CALL | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=my_usart3_comm.Data[1];		my_usart3send_comm.Data[2]=0x01;	
								usart3_send_comm(&my_usart3send_comm);								
							  start_voice_play();
							}
							break;
						
						case SCREE_TAKE_DOWN: rt_kprintf("SCREE_TAKE_DOWN\n");//屏幕挂机
						  client_send_buff[0]=Dutt_Room;
						  client_send_buff[1]=ALL_AUX__Room;
						  client_send_buff[2]=Take_Down;
						  client_send_buff[3]=my_usart3_comm.Data[1];
						  client_send_buff[4]=my_usart3_comm.Data[0];
						  client_send_buff[5]=0x00;
						  remote_ip[3]=my_usart3_comm.Data[0];
						  if(tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buff,6)==0){  //挂机失败
								my_usart3send_comm.Comm=SCREE_TAKE_DOWN| USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x02;
                usart3_send_comm(&my_usart3send_comm);								
							}else{
								my_usart3send_comm.Comm=SCREE_TAKE_DOWN | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=remote_ip[3];
								my_usart3send_comm.Data[1]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
								usart3_send_comm(&my_usart3send_comm); 
                stop_voice_play();							  
							}							
							break;
						
						case SCREE_REST:rt_kprintf("SCREE_REST\n");//屏幕复位
						  client_send_buff[0]=Dutt_Room;
						  client_send_buff[1]=ALL_AUX__Room;
						  client_send_buff[2]=Rst_Comm;
						  client_send_buff[3]=my_usart3_comm.Data[1];;
						  client_send_buff[4]=my_usart3_comm.Data[0];
						  client_send_buff[5]=0x00;
						  remote_ip[3]=my_usart3_comm.Data[0];
						  if(tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buff,6)==0){  //复位失败
								my_usart3send_comm.Comm=SCREE_REST | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x02;
								usart3_send_comm(&my_usart3send_comm);								
							}else{
								my_usart3send_comm.Comm=SCREE_REST | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x01;
								usart3_send_comm(&my_usart3send_comm);							
							}							
							break;
						
						case SCREE_CHECK:rt_kprintf("SCREE_CHECK\n");//屏幕检测
						  client_send_buff[0]=Dutt_Room;
						  client_send_buff[1]=ALL_AUX__Room;
						  client_send_buff[2]=Check_Com;
						  client_send_buff[3]=my_usart3_comm.Data[1];;
						  client_send_buff[4]=my_usart3_comm.Data[0];
						  client_send_buff[5]=0x00;
						  remote_ip[3]=my_usart3_comm.Data[0];	
						  if(tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buff,6)==0){  //呼叫应答失败
								my_usart3send_comm.Comm=SCREE_CHECK | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x02;
								usart3_send_comm(&my_usart3send_comm);								
							}else{
								my_usart3send_comm.Comm=SCREE_CHECK | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];  
								my_usart3send_comm.Data[1]=0x01;   my_usart3send_comm.Data[2]=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line; 
								usart3_send_comm(&my_usart3send_comm);	 
								rt_kprintf("my_usart3send_comm.Data[2]=%x\n",my_usart3send_comm.Data[2]);
							}							
							break;

						case SCREE_RES_CALL:rt_kprintf("SCREE_RES_CALL\n");//屏幕呼叫应答
						  client_send_buff[0]=Dutt_Room;
						  client_send_buff[1]=ALL_AUX__Room;
						  client_send_buff[2]=Dut_Call_Aux;
						  client_send_buff[3]=my_usart3_comm.Data[1];;
						  client_send_buff[4]=my_usart3_comm.Data[0];
						  client_send_buff[5]=0x00;
						  remote_ip[3]=my_usart3_comm.Data[0];	
						  if(tcp_client_send(SOCK_TCPC,6668,remote_ip,6667,client_send_buff,6)==0){  //呼叫应答失败
								my_usart3send_comm.Comm=SCREE_RES_CALL | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x02;
								usart3_send_comm(&my_usart3_comm);								
							}else{
								my_usart3send_comm.Comm=SCREE_RES_CALL | USART3_BLACK;   	my_usart3send_comm.DatLen=0x04;
								my_usart3send_comm.Data[0]=my_usart3_comm.Data[0];
								my_usart3send_comm.Data[1]=0x01;
								usart3_send_comm(&my_usart3_comm);
                start_voice_play();								
							}
							break;

						case RING: rt_kprintf("RING\n"); //振铃开关
							if(my_usart3_comm.Data[0]==0x01){  //打开振铃
								wiz_RING_C(LOW);  wiz_CF3_C(HIGH); 
							}else if(my_usart3_comm.Data[0]==0x02){ //关闭振铃
								wiz_RING_C(HIGH);  wiz_CF3_C(LOW);
							}else {
								;
							}								
							break;
						
						case CHANG_AUDIO:rt_kprintf("CHANG_AUDIO=%d\n",my_usart3_comm.Data[0]);//音频切换
							if(my_usart3_comm.Data[0]==0x01){  //切换为4G和电梯
                wiz_E0(LOW);wiz_E1(LOW); wiz_S0(LOW); wiz_S1(HIGH); wiz_S2(LOW); wiz_S3(HIGH);
							}else if(my_usart3_comm.Data[0]==0x02){ //切换为值班室和电梯
								wiz_E0(LOW);wiz_E1(HIGH); wiz_S0(LOW); wiz_S1(LOW); wiz_S2(LOW); wiz_S3(LOW);
							}else if(my_usart3_comm.Data[0]==0x03){  //切换为电梯和免提
								wiz_E0(LOW);wiz_E1(HIGH); wiz_S0(HIGH); wiz_S1(LOW); wiz_S2(LOW); wiz_S3(LOW);  wiz_CF3_C(HIGH);
							}else if(my_usart3_comm.Data[0]==0x04){  //切换为值班室和4g
								wiz_E0(HIGH);wiz_E1(LOW); wiz_S0(HIGH); wiz_S1(LOW); wiz_S2(LOW); wiz_S3(LOW);
							}else if(my_usart3_comm.Data[0]==0x05){  //切换为4G和免提
								wiz_E0(HIGH);wiz_E1(LOW); wiz_S0(HIGH); wiz_S1(LOW); wiz_S2(HIGH); wiz_S3(LOW);  wiz_CF3_C(HIGH);
							}else{
							  ;
							}  							
							break;
						
						case AUDIO_CF: rt_kprintf("AUDIO_CF\n"); //音频功放
							if(my_usart3_comm.Data[0]==0x01){  //打开音频功放
								wiz_CF1_C(HIGH);  
							}else if(my_usart3_comm.Data[0]==0x02){ //关闭音频功放
								wiz_CF1_C(LOW);  
							}else {
								;
							}							
							break;
							
						case REST_4G: rt_kprintf("REST_4G\n");//4G模块复位
							/**/
							wiz_NET_REST(HIGH);  rt_thread_delay(1000);wiz_NET_REST(LOW);							
							break;
							
						case 	VOLUME: rt_kprintf("VOLUME\n"); //音量
							if(my_usart3_comm.Data[0]==0x01){  //mic调节
								usart2_send_buff[0]=0xef;   usart2_send_buff[1]=0xfa;
								usart2_send_buff[2]=0x05;   usart2_send_buff[3]=0x01;
								if(my_usart3_comm.Data[1]<=63)
									usart2_send_buff[4]=my_usart3_comm.Data[1];
								else
									usart2_send_buff[4]=50;
								usart2_dma_send(usart2_send_buff,5);
							}else if(my_usart3_comm.Data[0]==0x02){  //spk调节
								usart2_send_buff[0]=0xef;   usart2_send_buff[1]=0xfa;
								usart2_send_buff[2]=0x03;   usart2_send_buff[3]=0x01;
								if(my_usart3_comm.Data[1]<=50)
									usart2_send_buff[4]=my_usart3_comm.Data[1];
								else
									usart2_send_buff[4]=50;
                usart2_dma_send(usart2_send_buff,5);								
							}
							break;
						
						case ON_SPEAK: //免提
							break;
						
						default:
							break;
					}
			 }
		}
	}
}

/*屏幕消息整理*/
static int ReviceComm(struct Usart_Comm *com ,unsigned char buf )
{
	int cnt = 0;
	int nRet = 0;
//  rt_kprintf("buf=%x\n",buf);
	switch(com->idx)
	{
		case 0:  
			if(buf == 0xff) {
				com->idx++;
				com->Head[0]=buf;
			}
			else {
				com->idx=0;
			}
		break;
			
		case 1:
			if(buf == 0xa5) {
				com->idx++;
				com->Head[1]=buf;
			}
			else {
				com->idx=0;
			}
		break;

		case 2:
			if(buf == 0x00) {
				com->idx++;
				com->Head[2]=buf;
			}
			else {
				com->idx=0;
			}
		break;	

		case 3:
			com->Comm=buf;
		  com->idx++;
		break;

		case 4:  
			com->DatLen=buf;
		  com->len=buf;
		  com->idx++;
		break;

		default:  
			if(com->len){
			  com->Data[com->idx-5]=buf;
				com->idx++;  com->len--;
			}
			if(com->len==0){
				com->idx=0;
				nRet=1;			   
			}
		break;			
	}
	return nRet;
}

int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(USART1, (uint8_t)ch);
    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);

    return (ch);
}