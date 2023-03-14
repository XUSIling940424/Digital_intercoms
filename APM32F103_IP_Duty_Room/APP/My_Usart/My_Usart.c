#include "My_Usart.h"

/*串口接收*/
/*指向串口信号量的指针*/
rt_sem_t  my_usart_sem = RT_NULL;
/*串口2数据缓存*/
struct Usart_Msg  my_usart_msg;
/*定义线程控制块指针*/
static rt_thread_t Usart_Recv_thread = RT_NULL;	
/*函数声明*/
static void Usart_Recv_thread_entry(void *parameter);

/*串口发送*/
/*指向串口信号量的指针*/
rt_sem_t  my_usart2_send_sem = RT_NULL;
/*定义线程控制块指针*/
static rt_thread_t Usart2_Send_thread = RT_NULL;
/*函数声明*/
static void Usart2_Send_thread_entry(void *parameter);

/*串口1发送互斥锁*/
rt_mutex_t usart1_send_mutex;
/*串口2发送互斥锁*/
rt_mutex_t usart2_send_mutex;

/*串口三信号量*/
rt_sem_t  my_usart3_sem = RT_NULL;
/*定义线程控制块指针*/
static rt_thread_t Usart3_Recv_thread = RT_NULL;	
/*函数声明*/
static void Usart3_Recv_thread_entry(void *parameter);
/*串口3发送互斥锁*/
rt_mutex_t usart3_send_mutex;
/*串口3数据缓存*/
struct USART3_MSG  my_usart3_msg;


int build_usart_recv_thread(void)
{
	  /*创建信号量*/
	  my_usart_sem = rt_sem_create("usart_sem",0,RT_IPC_FLAG_FIFO);
		/*创建线程*/
		Usart_Recv_thread=                              /* 线程控制块指针 */
		rt_thread_create( "usart_recv",                 /* 线程名字 */
											Usart_Recv_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											2,                      /* 线程的优先级 */
											1000);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(Usart_Recv_thread != RT_NULL){
			rt_kprintf("rt_thread_usartrecv_create is success\n");
			rt_thread_startup(Usart_Recv_thread);
		}
		else{ 
			rt_kprintf("rt_thread_usartrecv_create is faile\n");
			return -1;	
		}
}

/*创建串口三接收线程*/
int build_usart3_recv_thread(void)
{
	  /*创建信号量*/
	  my_usart3_sem = rt_sem_create("usart3_sem",0,RT_IPC_FLAG_FIFO);
		/*创建线程*/
		Usart3_Recv_thread=                              /* 线程控制块指针 */
		rt_thread_create( "usart_recv",                 /* 线程名字 */
											Usart3_Recv_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											2,                      /* 线程的优先级 */
											1000);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(Usart3_Recv_thread != RT_NULL){
			rt_kprintf("rt_thread_usart3recv_create is success\n");
			rt_thread_startup(Usart3_Recv_thread);
		}
		else{ 
			rt_kprintf("rt_thread_usart3recv_create is faile\n");
			return -1;	
		}
}

int build_usart2_send_thread(void)
{
	  /*创建信号量*/
	  my_usart2_send_sem = rt_sem_create("usart_send_sem",0,RT_IPC_FLAG_FIFO);
		/*创建线程*/
		Usart2_Send_thread=                              /* 线程控制块指针 */
		rt_thread_create( "usart_send",                 /* 线程名字 */
											Usart2_Send_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											2,                      /* 线程的优先级 */
											200);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(Usart2_Send_thread != RT_NULL){
			rt_kprintf("rt_thread_usartrecv_create is success\n");
			rt_thread_startup(Usart2_Send_thread);
		}
		else{ 
			rt_kprintf("rt_thread_usartrecv_create is faile\n");
			return -1;	
		}
}

static void Usart_Recv_thread_entry(void *parameter)
{
	static rt_err_t result;
	int i=0, dix=0 ,cpylen=0;
	unsigned char    my_usart_send[150]={0};
	while(1)
	{
		/*等待信号量*/
		result = rt_sem_take(my_usart_sem,RT_WAITING_FOREVER);
//		rt_kprintf("OK\n");
		if(result != RT_EOK){
			rt_sem_delete(my_usart_sem);
		}else{  /*读出一级缓存*/
			/*拷贝DMA数据*/
			Rx_USART2_Len=MAX_USART_RXBUFLEN-DMA_ReadDataNumber(DMA1_Channel6);
			DMA_Disable(DMA1_Channel6);//关闭DMA准备重新配置
			DMA_ClearStatusFlag(DMA1_INT_FLAG_GINT6);
			rt_memset(RX_USART2_BUFF,0,sizeof(RX_USART2_BUFF));
			rt_memcpy(RX_USART2_BUFF,RxBuffer2,Rx_USART2_Len);
			DMA_ConfigDataNumber(DMA1_Channel6,MAX_USART_RXBUFLEN);  /*重新配置DMA*/
			DMA_EnableInterrupt(DMA1_Channel6, ENABLE);	
      Rx_USART2_Flags=0;
      /*将语音输出打包*/			
			if(Rx_USART2_Len >=120){ 
				rt_memcpy(&my_usart_msg.msg_buff[my_usart_msg.msg_input],RX_USART2_BUFF,Rx_USART2_Len);
				my_usart_msg.msg_input=((my_usart_msg.msg_input+1)%(MAX_USART_MSG_NUM));
				cpylen=0;    my_usart_msg.temp=0;		 			
			}else{
				;
			}
      /*将网络接收到的数据播放*/
      if(udp_7777_recv_buff[0].msg_input!=udp_7777_recv_buff[0].msg_output){ //第一路，机房语音
				usart2_dma_send(udp_7777_recv_buff[0].msg_buff[udp_7777_recv_buff[0].msg_output],120);
				udp_7777_recv_buff[0].msg_input=((udp_7777_recv_buff[0].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.com_talk==0){
				   My_Attribute.com_talk=1;  
				}else{
				   My_Attribute.com_talk_temp=1;
				}
//        rt_kprintf("1\n");				
			}	
      if(udp_7777_recv_buff[1].msg_input!=udp_7777_recv_buff[1].msg_output){ //第二路，轿厢语音
				usart2_dma_send(udp_7777_recv_buff[1].msg_buff[udp_7777_recv_buff[1].msg_output],120);
				udp_7777_recv_buff[1].msg_input=((udp_7777_recv_buff[1].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.lif_talk==0){
				   My_Attribute.lif_talk=1;  
				}else{
				   My_Attribute.lif_talk_temp=1;
				}				
//        rt_kprintf("2\n");			  
			}
      if(udp_7777_recv_buff[2].msg_input!=udp_7777_recv_buff[2].msg_output){ //第三路，底坑语音
				usart2_dma_send(udp_7777_recv_buff[2].msg_buff[udp_7777_recv_buff[2].msg_output],120);
				udp_7777_recv_buff[2].msg_input=((udp_7777_recv_buff[2].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.pit_talk==0){
				   My_Attribute.pit_talk=1;  
				}else{
				   My_Attribute.pit_talk_temp=1;
				}					
//        rt_kprintf("3\n");			  
			}
      if(udp_7777_recv_buff[3].msg_input!=udp_7777_recv_buff[3].msg_output){ //第四路，轿顶语音
				usart2_dma_send(udp_7777_recv_buff[3].msg_buff[udp_7777_recv_buff[3].msg_output],120);
				udp_7777_recv_buff[3].msg_input=((udp_7777_recv_buff[3].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.car_talk==0){
				   My_Attribute.car_talk=1;  
				}else{
				   My_Attribute.car_talk_temp=1;
				}					
//        rt_kprintf("4\n");				
			}
      if(udp_7777_recv_buff[4].msg_input!=udp_7777_recv_buff[4].msg_output){  //第五路，值班室语音
				usart2_dma_send(udp_7777_recv_buff[4].msg_buff[udp_7777_recv_buff[4].msg_output],120);
				udp_7777_recv_buff[4].msg_input=((udp_7777_recv_buff[4].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.dut_talk==0){
				   My_Attribute.dut_talk=1;  
				}else{
				   My_Attribute.dut_talk_temp=1;
				}					
//        rt_kprintf("5\n");			  
			}			
		}
	}
}

static void Usart3_Recv_thread_entry(void *parameter)
{
	static rt_err_t result;
	while(1)
	{
		/*等待信号量*/
		result = rt_sem_take(my_usart3_sem,RT_WAITING_FOREVER);
    rt_kprintf("usart3 _ recv buff\n");
		if(result != RT_EOK){
			rt_sem_delete(my_usart3_sem);
		}else{  /*读出一级缓存*/
			DMA_Disable(DMA1_Channel3);//关闭DMA准备重新配置
			Rx_USART3_Len=MAX_USART_RXBUFLEN-DMA_ReadDataNumber(DMA1_Channel3);
			rt_memset(RX_USART3_BUFF,0,sizeof(RX_USART3_BUFF));
			rt_memcpy(RX_USART3_BUFF,RxBuffer3,Rx_USART3_Len);
      rt_memset(RxBuffer3,0,sizeof(RxBuffer3));			
			DMA_ConfigDataNumber(DMA1_Channel3,MAX_USART_RXBUFLEN);
			DMA_EnableInterrupt(DMA1_Channel3, ENABLE);
			Rx_USART3_Flags=1;  
			/*将数据送入缓存*/
			if(Rx_USART3_Len>0){
				rt_memcpy(&my_usart3_msg.msg_buff[my_usart3_msg.msg_input].msg_buff,RX_USART3_BUFF,Rx_USART3_Len);
				my_usart3_msg.msg_buff[my_usart3_msg.msg_input].msg_len=Rx_USART3_Len;
				my_usart3_msg.msg_input=((my_usart3_msg.msg_input+1)%(MAX_USART_MSG_NUM));			
			}else {
				;
			}		
		}
	}
}

static void Usart2_Send_thread_entry(void *parameter)
{
	static rt_err_t result;
	
  while(1)
	{
		/*等待信号量*/
		result = rt_sem_take(my_usart2_send_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){
			rt_sem_delete(my_usart2_send_sem);
		}else{  /*开启发送*/
			 ;
		}
	}
}

/*串口1发送函数*/
void usart1_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
	rt_mutex_take(usart1_send_mutex,RT_WAITING_FOREVER);   //试图持有锁，没有一直等待
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART1, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART1,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART1, USART_FLAG_TXC) == RESET);
	rt_mutex_release(usart1_send_mutex);		  //释放锁
}

/*串口2发送函数*/
void usart2_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
//	rt_mutex_take(usart2_send_mutex,RT_WAITING_FOREVER);   //试图持有锁，没有一直等待
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART2, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART2,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART2, USART_FLAG_TXC) == RESET);
//	rt_mutex_release(usart2_send_mutex);		  //释放锁
}

/*串口二dma发送函数*/
void  usart2_dma_send(unsigned char*buf,unsigned char len)
{
	static rt_err_t result;
	
	/*拷贝需要发送的数据到发送缓存区*/ 
	if(My_Usart_Send_Buff.temp==0){   /*DMA为空,且不在接收状态*/
		DMA_Disable(DMA1_Channel7);
		rt_memcpy(TxBuffer2,buf,len);
		DMA_ConfigDataNumber(DMA1_Channel7,len);
		DMA_Enable(DMA1_Channel7);
		My_Usart_Send_Buff.temp=1;
	}else{   /*发送缓存未完成*/
	   rt_memcpy(My_Usart_Send_Buff.msg[My_Usart_Send_Buff.input],buf,len);
		 My_Usart_Send_Buff.input=(My_Usart_Send_Buff.input+1)%MAX_USART_SENDLEN;
	}
}

/*串口3发送函数*/
void usart3_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
//	rt_mutex_take(usart3_send_mutex,RT_WAITING_FOREVER);   //试图持有锁，没有一直等待
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART3, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART3,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART3, USART_FLAG_TXC) == RESET);
//	rt_mutex_release(usart3_send_mutex);		  //释放锁
}

/*串口三dma发送函数*/
void  usart3_dma_send(unsigned char*buf,unsigned char len)
{
	static rt_err_t result;
	
	/*拷贝需要发送的数据到发送缓存区*/ 
	DMA_Disable(DMA1_Channel2);
	rt_memcpy(TxBuffer3,buf,len);
	DMA_ConfigDataNumber(DMA1_Channel2,len);
	DMA_Enable(DMA1_Channel2);
}

/*串口三发送命令函数*/
void usart3_send_comm(struct Usart_Comm * comm)
{
	unsigned char usart_send_buff[20]={0};
  
	usart_send_buff[0]=comm->Head[0];  /*头部信息*/
	usart_send_buff[1]=comm->Head[1];
	usart_send_buff[2]=comm->Head[2];
	
	usart_send_buff[3]=comm->Comm;  /*命令*/
	
	usart_send_buff[4]=comm->DatLen;  /*数据长度*/
	
	rt_memcpy(&usart_send_buff[5],comm->Data,comm->DatLen);  //具体数据
	
	usart3_dma_send(usart_send_buff,comm->DatLen+5);
}
                                                                                                                                                                                  