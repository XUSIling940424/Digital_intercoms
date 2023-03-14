#include "My_Usart.h"

/*串口接收*/
/*指向串口信号量的指针*/
rt_sem_t  my_usart_sem = RT_NULL;
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
			if(Rx_USART2_Len >=120){
       for(i=0;i<8;i++){
				 rt_memcpy(&my_usart_msg.msg_buff[my_usart_msg.msg_input][i*10],&RX_USART2_BUFF[i*15+5],10);
			 }
					my_usart_msg.msg_input=((my_usart_msg.msg_input+1)%(MAX_USART_MSG_NUM));
				  cpylen=0;    my_usart_msg.temp=0;
	#if 1	
			/*can消息检测*/
			if(Recv_Can_buff[0].msg_output!=Recv_Can_buff[0].msg_input){  //第一路
				rt_memset(my_usart_send,0,sizeof(my_usart_send));
				for(i=0;i<8;i++){
					my_usart_send[i*15]=0xef;    my_usart_send[i*15+1]=0xfa;
					my_usart_send[i*15+2]=0x08;  my_usart_send[i*15+3]=0x0b; my_usart_send[i*15+4]=0x01;		
					rt_memcpy(&my_usart_send[i*15+5],&Recv_Can_buff[0].msg_buff[Recv_Can_buff[0].msg_output][i*10],10);
				}
//				rt_kprintf("send is num 1\n");
				usart2_dma_send(my_usart_send,120);
				Recv_Can_buff[0].msg_output=((Recv_Can_buff[0].msg_output+1)%MAX_MSG_NUM);
			}		
		  if(Recv_Can_buff[1].msg_output!=Recv_Can_buff[1].msg_input){  //第二路
				rt_memset(my_usart_send,0,sizeof(my_usart_send));
				for(i=0;i<8;i++){
					my_usart_send[i*15]=0xef;    my_usart_send[i*15+1]=0xfa;
					my_usart_send[i*15+2]=0x08;  my_usart_send[i*15+3]=0x0b;  my_usart_send[i*15+4]=0x02;	
					rt_memcpy(&my_usart_send[i*15+5],&Recv_Can_buff[1].msg_buff[Recv_Can_buff[1].msg_output][i*10],10);
				}
//				rt_kprintf("send is num 2\n");
				usart2_dma_send(my_usart_send,120);
				Recv_Can_buff[1].msg_output=((Recv_Can_buff[1].msg_output+1)%MAX_MSG_NUM);
			}
		 if(Recv_Can_buff[2].msg_output!=Recv_Can_buff[2].msg_input){   //第三路
				rt_memset(my_usart_send,0,sizeof(my_usart_send));
				for(i=0;i<8;i++){
					my_usart_send[i*15]=0xef;    my_usart_send[i*15+1]=0xfa;
					my_usart_send[i*15+2]=0x08;  my_usart_send[i*15+3]=0x0b;  my_usart_send[i*15+4]=0x03;		
					rt_memcpy(&my_usart_send[i*15+5],&Recv_Can_buff[2].msg_buff[Recv_Can_buff[2].msg_output][i*10],10);
				}
//				rt_kprintf("send is num 3\n");
				usart2_dma_send(my_usart_send,120);
				Recv_Can_buff[2].msg_output=((Recv_Can_buff[2].msg_output+1)%MAX_MSG_NUM);
			}	
		if(Recv_Can_buff[3].msg_output!=Recv_Can_buff[3].msg_input){   //第四路
				rt_memset(my_usart_send,0,sizeof(my_usart_send));
				for(i=0;i<8;i++){
					my_usart_send[i*15]=0xef;    my_usart_send[i*15+1]=0xfa;
					my_usart_send[i*15+2]=0x08;  my_usart_send[i*15+3]=0x0b;  my_usart_send[i*15+4]=0x04;			
					rt_memcpy(&my_usart_send[i*15+5],&Recv_Can_buff[3].msg_buff[Recv_Can_buff[3].msg_output][i*10],10);
				}
//				rt_kprintf("send is num 4\n");
				usart2_dma_send(my_usart_send,120);
				Recv_Can_buff[3].msg_output=((Recv_Can_buff[3].msg_output+1)%MAX_MSG_NUM);
			}
		if(Recv_Can_buff[4].msg_output!=Recv_Can_buff[4].msg_input){   //第五路
				rt_memset(my_usart_send,0,sizeof(my_usart_send));
				for(i=0;i<8;i++){
					my_usart_send[i*15]=0xef;    my_usart_send[i*15+1]=0xfa;
					my_usart_send[i*15+2]=0x08;  my_usart_send[i*15+3]=0x0b;  my_usart_send[i*15+4]=0x01;			
					rt_memcpy(&my_usart_send[i*15+5],&Recv_Can_buff[4].msg_buff[Recv_Can_buff[4].msg_output][i*10],10);
				}
//				rt_kprintf("send is num 4\n");
				usart2_dma_send(my_usart_send,120);
				Recv_Can_buff[4].msg_output=((Recv_Can_buff[4].msg_output+1)%MAX_MSG_NUM);
			}		
	#endif			 			
			}else{
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


                                                                                                                                                                                  