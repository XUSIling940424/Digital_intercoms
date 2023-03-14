#include "My_Usart.h"

/*���ڽ���*/
/*ָ�򴮿��ź�����ָ��*/
rt_sem_t  my_usart_sem = RT_NULL;
struct Usart_Msg  my_usart_msg;
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Usart_Recv_thread = RT_NULL;	
/*��������*/
static void Usart_Recv_thread_entry(void *parameter);

/*���ڷ���*/
/*ָ�򴮿��ź�����ָ��*/
rt_sem_t  my_usart2_send_sem = RT_NULL;
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Usart2_Send_thread = RT_NULL;
/*��������*/
static void Usart2_Send_thread_entry(void *parameter);

/*����1���ͻ�����*/
rt_mutex_t usart1_send_mutex;
/*����2���ͻ�����*/
rt_mutex_t usart2_send_mutex;

int build_usart_recv_thread(void)
{
	  /*�����ź���*/
	  my_usart_sem = rt_sem_create("usart_sem",0,RT_IPC_FLAG_FIFO);
		/*�����߳�*/
		Usart_Recv_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "usart_recv",                 /* �߳����� */
											Usart_Recv_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
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
	  /*�����ź���*/
	  my_usart2_send_sem = rt_sem_create("usart_send_sem",0,RT_IPC_FLAG_FIFO);
		/*�����߳�*/
		Usart2_Send_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "usart_send",                 /* �߳����� */
											Usart2_Send_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											200);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
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
		/*�ȴ��ź���*/
		result = rt_sem_take(my_usart_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){
			rt_sem_delete(my_usart_sem);
		}else{  /*����һ������*/
			/*����DMA����*/
			Rx_USART2_Len=MAX_USART_RXBUFLEN-DMA_ReadDataNumber(DMA1_Channel6);
			DMA_Disable(DMA1_Channel6);//�ر�DMA׼����������
			DMA_ClearStatusFlag(DMA1_INT_FLAG_GINT6);
			rt_memset(RX_USART2_BUFF,0,sizeof(RX_USART2_BUFF));
			rt_memcpy(RX_USART2_BUFF,RxBuffer2,Rx_USART2_Len);
			DMA_ConfigDataNumber(DMA1_Channel6,MAX_USART_RXBUFLEN);  /*��������DMA*/
			DMA_EnableInterrupt(DMA1_Channel6, ENABLE);	
      Rx_USART2_Flags=0;	  
			if(Rx_USART2_Len >=120){
       for(i=0;i<8;i++){
				 rt_memcpy(&my_usart_msg.msg_buff[my_usart_msg.msg_input][i*10],&RX_USART2_BUFF[i*15+5],10);
			 }
					my_usart_msg.msg_input=((my_usart_msg.msg_input+1)%(MAX_USART_MSG_NUM));
				  cpylen=0;    my_usart_msg.temp=0;
	#if 1	
			/*can��Ϣ���*/
			if(Recv_Can_buff[0].msg_output!=Recv_Can_buff[0].msg_input){  //��һ·
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
		  if(Recv_Can_buff[1].msg_output!=Recv_Can_buff[1].msg_input){  //�ڶ�·
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
		 if(Recv_Can_buff[2].msg_output!=Recv_Can_buff[2].msg_input){   //����·
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
		if(Recv_Can_buff[3].msg_output!=Recv_Can_buff[3].msg_input){   //����·
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
		if(Recv_Can_buff[4].msg_output!=Recv_Can_buff[4].msg_input){   //����·
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
		/*�ȴ��ź���*/
		result = rt_sem_take(my_usart2_send_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){
			rt_sem_delete(my_usart2_send_sem);
		}else{  /*��������*/
			 ;
		}
	}
}

/*����1���ͺ���*/
void usart1_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
	rt_mutex_take(usart1_send_mutex,RT_WAITING_FOREVER);   //��ͼ��������û��һֱ�ȴ�
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART1, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART1,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART1, USART_FLAG_TXC) == RESET);
	rt_mutex_release(usart1_send_mutex);		  //�ͷ���
}

/*����2���ͺ���*/
void usart2_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
//	rt_mutex_take(usart2_send_mutex,RT_WAITING_FOREVER);   //��ͼ��������û��һֱ�ȴ�
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART2, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART2,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART2, USART_FLAG_TXC) == RESET);
//	rt_mutex_release(usart2_send_mutex);		  //�ͷ���
}

/*���ڶ�dma���ͺ���*/
void  usart2_dma_send(unsigned char*buf,unsigned char len)
{
	static rt_err_t result;
	
	/*������Ҫ���͵����ݵ����ͻ�����*/ 
	if(My_Usart_Send_Buff.temp==0){   /*DMAΪ��,�Ҳ��ڽ���״̬*/
		DMA_Disable(DMA1_Channel7);
		rt_memcpy(TxBuffer2,buf,len);
		DMA_ConfigDataNumber(DMA1_Channel7,len);
		DMA_Enable(DMA1_Channel7);
		My_Usart_Send_Buff.temp=1;
	}else{   /*���ͻ���δ���*/
	   rt_memcpy(My_Usart_Send_Buff.msg[My_Usart_Send_Buff.input],buf,len);
		 My_Usart_Send_Buff.input=(My_Usart_Send_Buff.input+1)%MAX_USART_SENDLEN;
	}
}


                                                                                                                                                                                  