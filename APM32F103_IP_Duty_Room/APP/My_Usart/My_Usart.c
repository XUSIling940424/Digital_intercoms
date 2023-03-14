#include "My_Usart.h"

/*���ڽ���*/
/*ָ�򴮿��ź�����ָ��*/
rt_sem_t  my_usart_sem = RT_NULL;
/*����2���ݻ���*/
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

/*�������ź���*/
rt_sem_t  my_usart3_sem = RT_NULL;
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Usart3_Recv_thread = RT_NULL;	
/*��������*/
static void Usart3_Recv_thread_entry(void *parameter);
/*����3���ͻ�����*/
rt_mutex_t usart3_send_mutex;
/*����3���ݻ���*/
struct USART3_MSG  my_usart3_msg;


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

/*���������������߳�*/
int build_usart3_recv_thread(void)
{
	  /*�����ź���*/
	  my_usart3_sem = rt_sem_create("usart3_sem",0,RT_IPC_FLAG_FIFO);
		/*�����߳�*/
		Usart3_Recv_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "usart_recv",                 /* �߳����� */
											Usart3_Recv_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
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
//		rt_kprintf("OK\n");
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
      /*������������*/			
			if(Rx_USART2_Len >=120){ 
				rt_memcpy(&my_usart_msg.msg_buff[my_usart_msg.msg_input],RX_USART2_BUFF,Rx_USART2_Len);
				my_usart_msg.msg_input=((my_usart_msg.msg_input+1)%(MAX_USART_MSG_NUM));
				cpylen=0;    my_usart_msg.temp=0;		 			
			}else{
				;
			}
      /*��������յ������ݲ���*/
      if(udp_7777_recv_buff[0].msg_input!=udp_7777_recv_buff[0].msg_output){ //��һ·����������
				usart2_dma_send(udp_7777_recv_buff[0].msg_buff[udp_7777_recv_buff[0].msg_output],120);
				udp_7777_recv_buff[0].msg_input=((udp_7777_recv_buff[0].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.com_talk==0){
				   My_Attribute.com_talk=1;  
				}else{
				   My_Attribute.com_talk_temp=1;
				}
//        rt_kprintf("1\n");				
			}	
      if(udp_7777_recv_buff[1].msg_input!=udp_7777_recv_buff[1].msg_output){ //�ڶ�·����������
				usart2_dma_send(udp_7777_recv_buff[1].msg_buff[udp_7777_recv_buff[1].msg_output],120);
				udp_7777_recv_buff[1].msg_input=((udp_7777_recv_buff[1].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.lif_talk==0){
				   My_Attribute.lif_talk=1;  
				}else{
				   My_Attribute.lif_talk_temp=1;
				}				
//        rt_kprintf("2\n");			  
			}
      if(udp_7777_recv_buff[2].msg_input!=udp_7777_recv_buff[2].msg_output){ //����·���׿�����
				usart2_dma_send(udp_7777_recv_buff[2].msg_buff[udp_7777_recv_buff[2].msg_output],120);
				udp_7777_recv_buff[2].msg_input=((udp_7777_recv_buff[2].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.pit_talk==0){
				   My_Attribute.pit_talk=1;  
				}else{
				   My_Attribute.pit_talk_temp=1;
				}					
//        rt_kprintf("3\n");			  
			}
      if(udp_7777_recv_buff[3].msg_input!=udp_7777_recv_buff[3].msg_output){ //����·���ζ�����
				usart2_dma_send(udp_7777_recv_buff[3].msg_buff[udp_7777_recv_buff[3].msg_output],120);
				udp_7777_recv_buff[3].msg_input=((udp_7777_recv_buff[3].msg_input+1)%(MAX_NET_MSG_NUM));
				if(My_Attribute.car_talk==0){
				   My_Attribute.car_talk=1;  
				}else{
				   My_Attribute.car_talk_temp=1;
				}					
//        rt_kprintf("4\n");				
			}
      if(udp_7777_recv_buff[4].msg_input!=udp_7777_recv_buff[4].msg_output){  //����·��ֵ��������
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
		/*�ȴ��ź���*/
		result = rt_sem_take(my_usart3_sem,RT_WAITING_FOREVER);
    rt_kprintf("usart3 _ recv buff\n");
		if(result != RT_EOK){
			rt_sem_delete(my_usart3_sem);
		}else{  /*����һ������*/
			DMA_Disable(DMA1_Channel3);//�ر�DMA׼����������
			Rx_USART3_Len=MAX_USART_RXBUFLEN-DMA_ReadDataNumber(DMA1_Channel3);
			rt_memset(RX_USART3_BUFF,0,sizeof(RX_USART3_BUFF));
			rt_memcpy(RX_USART3_BUFF,RxBuffer3,Rx_USART3_Len);
      rt_memset(RxBuffer3,0,sizeof(RxBuffer3));			
			DMA_ConfigDataNumber(DMA1_Channel3,MAX_USART_RXBUFLEN);
			DMA_EnableInterrupt(DMA1_Channel3, ENABLE);
			Rx_USART3_Flags=1;  
			/*���������뻺��*/
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

/*����3���ͺ���*/
void usart3_send_Data(unsigned char*buf,unsigned char len )
{
	unsigned char t=0;
	
//	rt_mutex_take(usart3_send_mutex,RT_WAITING_FOREVER);   //��ͼ��������û��һֱ�ȴ�
	
	for(t=0;t<len;t++)
	{
		while(USART_ReadStatusFlag(USART3, USART_FLAG_TXC) == RESET);	  
		USART_TxData(USART3,buf[t]);			
	}
	while(USART_ReadStatusFlag(USART3, USART_FLAG_TXC) == RESET);
//	rt_mutex_release(usart3_send_mutex);		  //�ͷ���
}

/*������dma���ͺ���*/
void  usart3_dma_send(unsigned char*buf,unsigned char len)
{
	static rt_err_t result;
	
	/*������Ҫ���͵����ݵ����ͻ�����*/ 
	DMA_Disable(DMA1_Channel2);
	rt_memcpy(TxBuffer3,buf,len);
	DMA_ConfigDataNumber(DMA1_Channel2,len);
	DMA_Enable(DMA1_Channel2);
}

/*���������������*/
void usart3_send_comm(struct Usart_Comm * comm)
{
	unsigned char usart_send_buff[20]={0};
  
	usart_send_buff[0]=comm->Head[0];  /*ͷ����Ϣ*/
	usart_send_buff[1]=comm->Head[1];
	usart_send_buff[2]=comm->Head[2];
	
	usart_send_buff[3]=comm->Comm;  /*����*/
	
	usart_send_buff[4]=comm->DatLen;  /*���ݳ���*/
	
	rt_memcpy(&usart_send_buff[5],comm->Data,comm->DatLen);  //��������
	
	usart3_dma_send(usart_send_buff,comm->DatLen+5);
}
                                                                                                                                                                                  