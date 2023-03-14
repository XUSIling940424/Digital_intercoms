#include "my_netapp.h"

/*定义线程控制块指针*/
static rt_thread_t my_net_thread = RT_NULL;	
/*函数声明*/
static void mu_net_thread_entry(void *parameter);

struct Net_Port_Msg ser_6667_recv;   /*6667服务器端口缓存*/
struct Net_Port_Msg ser_6666_recv;   /*6666服务器端口缓存*/

static u8 Client_HandData(u8 *buf);
static uint8_t  server_rest(uint8_t * buf,u8 Number);

static uint8 getSn_DIPR(SOCKET s,u8* buf);

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
   /*监听服务器6666端口*/		
    tcp_server_list(SOCK_TCPS_2,6666);		
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
	u8  out_time_num=0;
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
    else{
		    ;
		}
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
	uint8_t clietn_ip[4]={0};
	
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
      if((len>0) && (len<=sizeof(recv_tcp_erver_buff)))
      {
        recv(Number,recv_tcp_erver_buff,len);                         // 接收来自Client的数据
        recv_tcp_erver_buff[len]=0x00;                                   // 添加字符串结束符
				/*读取客户端IP地址*/
				getSn_DIPR(Number,clietn_ip);
        rt_kprintf("clietn_ip=%d.%d.%d.%d\n",clietn_ip[0],clietn_ip[1],clietn_ip[2],clietn_ip[3]);				
				/*返回响应*/
				server_rest(recv_tcp_erver_buff,Number);	
				disconnect(Number);                              // 断开当前TCP连接
				close(Number);                                   // 关闭当前所使用socket				
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
static uint8_t server_rest(uint8_t * buf,u8 Number)
{
 struct Net_Comm   net_comm;
 u8 server_back_buf[10]={0};
 uint8_t clietn_ip[4]={0};
 
 getSn_DIPR(Number,clietn_ip);
 /**/
 net_comm.msg_from=buf[0];
 net_comm.msg_to=buf[1]; 
 net_comm.cmd=buf[2];
 net_comm.data=(buf[3]<<8) | buf[4];
 net_comm.zeros=buf[5];
 net_comm.client_ip[0]=clietn_ip[0];
 net_comm.client_ip[1]=clietn_ip[1];
 net_comm.client_ip[2]=clietn_ip[2];
 net_comm.client_ip[3]=clietn_ip[3];
 
// rt_kprintf("net_comm.msg_from=%x\n",net_comm.msg_from);
// rt_kprintf("net_comm.msg_to=%x\n",net_comm.msg_to);
// rt_kprintf("net_comm.cmd=%x\n",net_comm.cmd);
 
 switch(net_comm.cmd)
 {
		case AUX_Call_Com://副机呼叫机房信号			
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*拷贝结构体到缓存*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 		
			break;
		
		case Com_Call_Dut://机房呼叫值班室
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*拷贝结构体到缓存*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 			
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
			break;
		
		case Take_Down://挂机命令				
			break;
		
		case Rst_Comm://报警终止
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*拷贝结构体到缓存*/
//		 rt_kprintf("net_comm.cmd=%d\n",net_comm.cmd);
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=Rst_Comm;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
//		  rt_kprintf("ser_6667_recv.msg_buff[%d].cmd=%x\n",ser_6667_recv.msg_input,ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 
      		
			break;
		
		case Check_Com://手动检测
			break;
		
		case HearBeat://心跳包上传
			rt_kprintf("recv HearBeat msg\n");
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*拷贝结构体到缓存*/
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].cmd=net_comm.cmd;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].data=net_comm.data;
			ser_6666_recv.msg_buff[ser_6666_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6666_recv.msg_buff[ser_6666_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6666_recv.msg_input=(ser_6666_recv.msg_input+1)%MAX_NET_MSG_NUM; 							
			break;
		
		case ALA_FILT://机房报警过滤上传
			server_back_buf[0]=net_comm.msg_to;;
			server_back_buf[1]=net_comm.msg_from;
			server_back_buf[2]=net_comm.cmd;
			server_back_buf[3]=net_comm.data>>8;
			server_back_buf[4]=net_comm.data;
			server_back_buf[5]=net_comm.zeros;
			send(Number,server_back_buf,6);
			/*拷贝结构体到缓存*/
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_from=net_comm.msg_from;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].msg_to=net_comm.msg_to;		
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].cmd=net_comm.cmd;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].data=net_comm.data;
			ser_6667_recv.msg_buff[ser_6667_recv.msg_input].zeros=net_comm.zeros;
			rt_memcpy(ser_6667_recv.msg_buff[ser_6667_recv.msg_input].client_ip,net_comm.client_ip,4);
			ser_6667_recv.msg_input=(ser_6667_recv.msg_input+1)%MAX_NET_MSG_NUM; 			
			break;
		
		case Elec_DULER://电池欠压
			break;
		
		case VOILD_Chang://音量调节
			break;
		
		default:
			break;
 }
}

/*客户端数据处理函数*/
static u8 Client_HandData(u8 *buf)
{
	u8 ret=0;
	struct Net_Comm   net_comm;
	struct Usart_Comm  my_usart3_comm;
	uint8_t usart_send_buff[20]={0};

	my_usart3_comm.Head[0]=0xff;  my_usart3_comm.Head[1]=0x55; my_usart3_comm.Head[2]=0x00;

  net_comm.msg_from=buf[0];
  net_comm.msg_to=buf[1];	
  net_comm.cmd=buf[2];
  net_comm.data=(buf[3]<<8) | buf[4];
  net_comm.zeros=buf[5];

 switch(net_comm.cmd)
 {
		case AUX_Call_Com://副机呼叫机房信号	
      ret=AUX_Call_Com;		
			break;
		
		case Com_Call_Aux://机房呼叫副机信号
			ret=Com_Call_Aux;		
			break;
		
		case Aux_Call_DUT://副机呼叫值班室信号
      ret=Aux_Call_DUT;					
			break;
		
		case Com_Call_Dut://机房呼叫值班室
      ret=Com_Call_Dut;			
			break;
		
		case Dut_Call_Aux://值班室呼叫副机
  		ret=Dut_Call_Aux;   rt_kprintf("recv Dut_Call_Aux msg\n");
			break;
		
		case Take_Down://挂机命令	
  		ret=Take_Down;		rt_kprintf("recv Take_Down msg\b");
			break;
		
		case Rst_Comm://报警终止
		  ret=Rst_Comm;		rt_kprintf("recv Rst_Comm msg\b");		
			break;
		
		case Check_Com://手动检测
			if(net_comm.data & 0x08){   //机房在线信息
				My_Attribute.com_line=1;
			}else{
			  My_Attribute.com_line=0;
			}
			if(net_comm.data & 0x04){   //底坑在线信息
				My_Attribute.pit_line=1;
			}else{
			  My_Attribute.pit_line=0;
			}
			if(net_comm.data & 0x02){   //轿厢在线信息
				My_Attribute.lif_line=1;
			}else{
			  My_Attribute.lif_line=0;
			}
			if(net_comm.data & 0x01){   //轿顶在线信息
				My_Attribute.car_line=1;
			}else{
			  My_Attribute.car_line=0;
			}				
			ret=Check_Com;
			break;
		
		case HearBeat://心跳包上传	
      ret=HearBeat;			
			break;
		
		case ALA_FILT://机房报警过滤上传
			ret=ALA_FILT;
			break;
		
		case Elec_DULER://电池欠压
			ret=Elec_DULER;
			break;
		
		case VOILD_Chang://音量调节
			ret=VOILD_Chang;
			break;
		
		default:
			break;
 }
#if 0	
  switch(net_comm.head)
  {
	  case 0x01:
		  switch(net_comm.cmd)
		  {
			  case Call_Comm| Rest_Comm:  //呼叫反馈
					my_usart3_comm.Comm=SCREE_CALL | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=net_comm.msg_from;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);  ret=SCREE_CALL;
				  break;
				
			  case Take_down| Rest_Comm:  //挂机反馈
					my_usart3_comm.Comm=SCREE_TAKE_DOWN | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=0x01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);  ret=SCREE_TAKE_DOWN;
				  break;

				case Rst_Comm | Rest_Comm: rt_kprintf("Rst_Comm\n");//复位反馈
					my_usart3_comm.Comm=SCREE_REST | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);	ret=SCREE_REST;		
					break;
				
				case RES_CALL | Rest_Comm:	rt_kprintf("RES_CALL\n");//呼叫应答
					my_usart3_comm.Comm=SCREE_RES_CALL | USART3_BLACK;   	my_usart3_comm.DatLen=0x04;
        	my_usart3_comm.Data[0]=remote_ip[3];
          my_usart3_comm.Data[1]=01;
					my_usart3_comm.Data[2]=0x01; my_usart3_comm.Data[3]=0x00;
          usart3_send_comm(&my_usart3_comm);	ret=SCREE_RES_CALL;				  
					break;
		  }
		  break;
	 
	  case 0x00:
		  break;
	 
	  default:
		  break;
 }
#endif 
	return ret;
}

/*读取客户端IP地址*/
static uint8 getSn_DIPR(SOCKET s,u8* buf)
{
	buf[0] = IINCHIP_READ(Sn_DIPR0(s));
	buf[1] = IINCHIP_READ(Sn_DIPR1(s));
	buf[2] = IINCHIP_READ(Sn_DIPR2(s));
	buf[3] = IINCHIP_READ(Sn_DIPR3(s));
	
	return 1;
}

