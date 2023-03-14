#include "my_netapp.h"

struct Net_Port_Msg ser_6667_recv;   /*6667服务器端口缓存*/

static u8 Client_HandData(u8 *buf);
static uint8_t  server_rest(uint8_t * buf,u8 Number);

/*定义线程控制块指针*/
static rt_thread_t my_net_thread = RT_NULL;	
/*函数声明*/
static void mu_net_thread_entry(void *parameter);

/*建立网络监听线程*/
int build_my_net_thread(void)
{
		/*创建线程*/
		my_net_thread=                              /* 线程控制块指针 */
		rt_thread_create( "my_net",                 /* 线程名字 */
											mu_net_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											2,                      /* 线程的优先级 */
											500);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(my_net_thread != RT_NULL){
			rt_kprintf("rt_thread_my_net_thread is success\n");
			rt_thread_startup(my_net_thread);
		}
		else{ 
			rt_kprintf("rt_thread_my_net_thread is faile\n");
			return -1;	
		}
}

static void mu_net_thread_entry(void *parameter)
{
	while(1){
   /*监听服务器6667端口*/		
    tcp_server_list(SOCK_TCPS,6667);
		/*检测网络语音*/
		do_udp();		
		rt_thread_delay(10);  
	}
}

/*
客户端发送函数
Number:w5500端口号
client_poll:客户端端口
server_ip:连接服务器地址
server_poll:连接服务器端口号
sendbuf:需要发送的数据地址
send_len:需要发送数据的长度
返回值：EXIT_FAILURE  
*/
uint8 tcp_client_send(u8 Number,u16 client_poll,uint8 * server_ip,u16 server_poll,u8 *sendbuf,u16 send_len)
{
	u16  out_time_num=0;
	uint16 len=0;	
	uint8_t tcp_client_buff[20];
	rt_err_t uwRet=RT_EOK;
	uint8_t ret=0;
	
	len = send_len;
	 
  do{ 
		if(getSn_SR(Number)==SOCK_CLOSED)                                   // socket处于关闭状态
		{ 
			IINCHIP_WRITE(Sn_KPALVTR(Number),0x02);//增加保活机制
			socket(Number,Sn_MR_TCP,client_poll++,Sn_MR_ND);
		}
		else if(getSn_SR(Number)==SOCK_INIT)                                     // socket处于初始化状态
		{ connect(Number,server_ip,server_poll);}
    else if(getSn_SR(Number)==SOCK_CLOSE_WAIT)                               // socket处于等待关闭状态
		{
			disconnect(Number);                                             // 断开当前TCP连接
			close(Number);                                                  // 关闭当前所使用socket	
			rt_thread_delay(100);
		}				
    else{;}
		rt_thread_delay(10);
    out_time_num++;	
    if(out_time_num>=10){
			  out_time_num=0;
				disconnect(Number);                                             // 断开当前TCP连接
				close(Number);                                                  // 关闭当前所使用socket	
			  rt_thread_delay(100);
				return 0;
		}
	}
	while(getSn_SR(Number)!=SOCK_ESTABLISHED);                          //等待连接状态
	  out_time_num=0;
	  send(Number,sendbuf,len);                                         //向Server发送数据
	/*等待服务器反馈*/
	do{
	  if(getSn_IR(Number) & Sn_IR_CON)
     {
        setSn_IR(Number, Sn_IR_CON);                 // 清除接收中断标志位
     }
		 len=getSn_RX_RSR(Number);
		 rt_thread_delay(10);
     out_time_num++;		 
	 if(out_time_num>=10){
		 out_time_num=0;
		 disconnect(Number);                                             // 断开当前TCP连接
		 close(Number);                                                  // 关闭当前所使用socket	
		 rt_thread_delay(100);
	   return 0;
		}
	}while(len<=0);
	  out_time_num=0;
	  recv(Number,tcp_client_buff,len);                       // 接收来自Server的数据
    tcp_client_buff[len]=0x00;                                 // 添加字符串结束符  	
		ret=Client_HandData(tcp_client_buff);
		disconnect(Number);                                             // 断开当前TCP连接
		close(Number);                                                  // 关闭当前所使用socket	
    rt_thread_delay(100);	
	  return ret;
}


/*
tcp客户端监听函数
server_poll :需要监听的端口
Number :w5500端口号
*/
void tcp_server_list(u8 Number,u16 server_poll)
{
  uint16 len=0;  
	uint8_t recv_tcp_erver_buff[320];
	rt_err_t uwRet=RT_EOK;
	
  switch(getSn_SR(Number))                             // 获取socket的状态
  {
    case SOCK_CLOSED:                                     // socket处于关闭状态
			IINCHIP_WRITE(Sn_KPALVTR(Number),0x02);//增加保活机制
      socket(Number ,Sn_MR_TCP,server_poll,Sn_MR_ND);   // 打开socket
      break;     
    
    case SOCK_INIT:                                       // socket已初始化状态
      listen(Number);                                  // socket建立监听
      break;
    
    case SOCK_ESTABLISHED:                                // socket处于连接建立状态  
      if(getSn_IR(Number) & Sn_IR_CON)
      {
        setSn_IR(Number, Sn_IR_CON);                   // 清除接收中断标志位
      }
      len=getSn_RX_RSR(Number);                        // 定义len为已接收数据的长度
      if(len>0)
      {
        recv(Number,recv_tcp_erver_buff,len);                         // 接收来自Client的数据
        recv_tcp_erver_buff[len]=0x00;                                   // 添加字符串结束符
				/*返回响应*/
				server_rest(recv_tcp_erver_buff,Number);			
      }
      break;
    
    case SOCK_CLOSE_WAIT:                                 // socket处于等待关闭状态
			disconnect(Number);                              // 断开当前TCP连接
      close(Number);                                   // 关闭当前所使用socket
      break;
  }
}

/*
用于服务器接收到数据后，响应反馈信号
buf:接收到的数据帧起始地址
Number:返送反馈信息的端口号
*/
static uint8_t  server_rest(uint8_t * buf,u8 Number)
{
	struct Net_Comm   net_comm;
  u8 server_back_buf[10]={0};
	
	net_comm.msg_from=buf[0];
	net_comm.msg_to=buf[1]; 
	net_comm.cmd=buf[2];
	net_comm.data=(buf[3]<<8) | buf[4];
	net_comm.zeros=buf[5];
	
	
	rt_kprintf("net_comm.cmd=%d\n",net_comm.cmd);
	switch(net_comm.cmd)
	{
		case AUX_Call_Com://副机呼叫机房信号			
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			break;

		case Aux_Call_DUT://副机呼叫值班室信号	
		 break;
		
		case Com_Call_Dut://机房呼叫值班室
		 break;

		case Dut_Call_Aux://值班室呼叫副机
			/*反馈客户端信息*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*转发机房*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/			
		 break;	

		case Take_Down://挂机命令	
			/*反馈客户端信息*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*转发机房*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;	
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/					
		 break;	

		case Rst_Comm://报警终止	
			/*反馈客户端信息*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*转发机房*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/			
		 break;
		
		case Check_Com://手动检测
			/*反馈客户端信息*/
			net_comm.data=0x00 |( My_Attribute.com_line <<3) | ( My_Attribute.pit_line  <<2)|  (My_Attribute.lif_line <<1) |  My_Attribute.car_line;
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);
      rt_kprintf("server_back_buf[4]=%x\n",server_back_buf[4]);		
		 break;
		
		case HearBeat://心跳包上传
		 break;

		case ALA_FILT://机房报警过滤上传
		 break;

		case Elec_DULER://电池欠压
		 break;	

		case VOILD_Chang://音量调节
			/*反馈客户端信息*/
			server_back_buf[0]=net_comm.msg_to;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;	
			send(Number,server_back_buf,6);	
			 /*转发机房*/
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=net_comm.msg_from;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=net_comm.msg_to;	
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=net_comm.cmd;										
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=net_comm.data;
			my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=net_comm.zeros;		
			
			my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
			rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/			
		 break;
		
		default:
			break;
	}		
}



/*客户端数据处理函数*/
static u8 Client_HandData(u8 *buf)
{
  u8 ret=1;

	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_from=buf[0];	
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.msg_to=buf[1];	
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.cmd=buf[2];										
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.data=(buf[3]<<8) | buf[4];
	my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_In].msg.zeros=buf[5];	
	my_send_remote_msg.Msg_In=(my_send_remote_msg.Msg_In+1)%MAX_MSG_NUM;
	rt_sem_release(my_can_needsend_sem);  /*释放信号量,开启硬件发送*/	

	return ret;
}
