#include "My_Can.h"
#include "gpio.h"

/*����״̬����*/
struct Attribute_t  My_Attribute;

/*can������Ϣ����*/
struct Can_Msg  Recv_Can_buff[5];
/*can���ջص�����������*/
can_recv_data_t  my_can_recv_data;
/*can����������Ϣ����*/
struct Remote_Msg   my_remote_msg;
/*can���Ϳ���������Ϣ����*/
struct Remote_Msg   my_send_remote_msg;

/*can��Ϣ���ͻ���*/
struct Can_Send_Buff   my_send_buff_fifo;

/*can1��Ϣ����*/
static rt_mq_t can1_mq=RT_NULL;
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Can_Recv_thread = RT_NULL;	
/*��������*/
static void Can_Recv_thread_entry(void *parameter);
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Can_Send_thread = RT_NULL;	
/*��������*/
static void Can_Send_thread_entry(void *parameter);
/*can1���ͻ�����*/
rt_mutex_t can1_send_mutex;
/*ָ��can�ź�����ָ��*/
rt_sem_t  my_can_sem = RT_NULL;
/*can�����ź���*/
rt_sem_t  my_cansend_sem = RT_NULL;  
/*can�����ź���*/
rt_sem_t  my_can_needsend_sem = RT_NULL;


/*can2*/
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Can2_Send_thread = RT_NULL;
/*��������*/
static void Can2_Send_thread_entry(void *parameter);
/*can2�����ź���*/
rt_sem_t  my_can2_needsend_sem = RT_NULL;
/*�����߳̿��ƿ�ָ��*/
static rt_thread_t Can2_Recv_thread = RT_NULL;	
/*��������*/
static void Can2_Recv_thread_entry(void *parameter);
/*ָ��can2�ź�����ָ��*/
rt_sem_t  my_can2_sem = RT_NULL;
/*can2���Ϳ���������Ϣ����*/
struct Remote_Msg   my_can2_send_remote_msg;
/*can2����������Ϣ����*/
struct Remote_Msg   my_can2_remote_msg;


/*����һ��can�����߳�*/
int build_can_recv_thread(void)
{
	/*����һ������4���ͻ�����*/
		can1_send_mutex=rt_mutex_create("can1_send_mutex",RT_IPC_FLAG_FIFO);			
		/*����can1�����߳�*/
		Can_Recv_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "can_recv",                 /* �߳����� */
											Can_Recv_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(Can_Recv_thread != RT_NULL){
			rt_kprintf("rt_thread_canrecv_create is success\n");
			rt_thread_startup(Can_Recv_thread);
		}
		else{ 
			rt_kprintf("rt_thread_canrecv_create is faile\n");
			return -1;	
		}
		
		/*����can2�����߳�*/
		Can2_Recv_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "can_recv",                 /* �߳����� */
											Can2_Recv_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											2,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(Can2_Recv_thread != RT_NULL){
			rt_kprintf("rt_thread_can2recv_create is success\n");
			rt_thread_startup(Can2_Recv_thread);
		}
		else{ 
			rt_kprintf("rt_thread_can2recv_create is faile\n");
			return -1;	
		}		
}
/*����һ��can�����߳�*/
int build_can_send_thread(void)
{
	/*�������ͳɹ��ź���*/
	  my_cansend_sem = rt_sem_create("my_cansend_sem",3,RT_IPC_FLAG_FIFO);
	/*����can�����ź���*/
		my_can_needsend_sem = rt_sem_create("my_cannendsend_sem",0,RT_IPC_FLAG_FIFO);	
	/*����can2�����ź���*/
		my_can2_needsend_sem = rt_sem_create("my_can2nendsend_sem",0,RT_IPC_FLAG_FIFO);		
		/*�����߳�*/
		Can_Send_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "can_send",                 /* �߳����� */
											Can_Send_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											3,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(Can_Send_thread != RT_NULL){
			rt_kprintf("rt_thread_cansend_create is success\n");
			rt_thread_startup(Can_Send_thread);
		}
		else{ 
			rt_kprintf("rt_thread_cansend_create is faile\n");
			return -1;	
		}
		/*�����߳�*/
		Can2_Send_thread=                              /* �߳̿��ƿ�ָ�� */
		rt_thread_create( "can2_send",                 /* �߳����� */
											Can2_Send_thread_entry,      /* �߳���ں��� */
											RT_NULL,                /* �߳���ں������� */
											1024,                    /* �߳�ջ��С */
											3,                      /* �̵߳����ȼ� */
											1000);                    /* �߳�ʱ��Ƭ */
		
		/*�����̣߳���������*/
		if(Can2_Send_thread != RT_NULL){
			rt_kprintf("rt_thread_can2send_create is success\n");
			rt_thread_startup(Can2_Send_thread);
		}
		else{ 
			rt_kprintf("rt_thread_can2send_create is faile\n");
			return -1;	
		}		
}

/*���մ�����*/
static void Can_Recv_thread_entry(void *parameter)
{
  static rt_err_t result;
	  /*�����ź���*/
	my_can_sem = rt_sem_create("can_sem",0,RT_IPC_FLAG_FIFO);	
	
	while(1)
	{
		/*�ȴ��ź���*/
		result = rt_sem_take(my_can_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){
			rt_sem_delete(my_can_sem);
		}else{ 
			while(my_recv_can.input!=my_recv_can.output){  
				my_can_recv_msg(Recv_Can_buff,&my_recv_can.msg[my_recv_can.output]);
				my_recv_can.output=(my_recv_can.output+1)%MAX_FILE_NUM;
			}
		}
	}
}

static void Can_Send_thread_entry(void *parameter)
{
	int i=0;   int ret=0;
	static rt_err_t result;
	CAN_TxMessage_T  my_can_tx_msg;  
	
	my_can_tx_msg.stdID=0x0420000;   //��׼֡��ʶ��
  my_can_tx_msg.extID=0x00420000;  //��չ֡��ʶ��
	my_can_tx_msg.typeID=CAN_TYPEID_EXT;          //��ʶ�����
	my_can_tx_msg.remoteTxReq=CAN_RTXR_DATA;     //֡����	
	my_can_tx_msg.dataLengthCode=8;  //���ݳ���
	my_can_tx_msg.remoteTxReq=0;     //��������
	
	while(1){
	/*�ȴ������ź���*/
		result = rt_sem_take(my_can_needsend_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){   /*�ź���δ�ͷ�*/
			rt_sem_delete(my_can_needsend_sem);
		}else{    /*��������Ҫ����*/
			if(my_send_remote_msg.Msg_In!=my_send_remote_msg.Msg_Out)   //��Զ��֡��Ҫ����
			{
				rt_kprintf("send remote_can msg \n");
				my_can_tx_msg.extID=my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_Out].id;
				my_can_tx_msg.remoteTxReq=CAN_RTXR_REMOTE;     //֡����
				my_can_tx_msg.typeID=CAN_TYPEID_EXT;          //��ʶ�����
				my_can_tx_msg.dataLengthCode=8;
				can1_send_msg(CAN1,&my_can_tx_msg);
				my_send_remote_msg.Msg_Out=(my_send_remote_msg.Msg_Out+1)%MAX_MSG_NUM;
			}else{				//������֡��Ҫ����
				for(i=0;i<MAX_SEND_NUM;i++){
					rt_memcpy(my_can_tx_msg.data,&my_usart_msg.msg_buff[my_usart_msg.msg_output][i*8],8);
					my_can_tx_msg.stdID=i | 0x0000;
					my_can_tx_msg.remoteTxReq=CAN_RTXR_DATA;     //֡����	
					my_can_tx_msg.dataLengthCode=8;  //���ݳ���		
					my_can_tx_msg.typeID=CAN_TYPEID_STD;          //��ʶ�����
          rt_memcpy(&can2_send_buff.msg[can2_send_buff.input],&my_can_tx_msg,sizeof(my_can_tx_msg));		//������can2������			
					ret=can1_send_msg(CAN1,&my_can_tx_msg);  
          can2_send_buff.input=(can2_send_buff.input+1)%MAX_KEEP_NUM;
				}
				my_usart_msg.msg_output=((my_usart_msg.msg_output+1)%MAX_USART_MSG_NUM);
			}
		}			
	}
}

static void Can2_Send_thread_entry(void *parameter)
{
	static rt_err_t result;
	uint8_t  ret=0;
	CAN_TxMessage_T  my_can2_tx_msg;  
	
  while(1){
	/*�ȴ������ź���*/
		result = rt_sem_take(my_can2_needsend_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){   /*�ź���δ�ͷ�*/
			rt_sem_delete(my_can2_needsend_sem);
		}else{    /*��������Ҫ����*/
      if(my_can2_send_remote_msg.Msg_In!=my_can2_send_remote_msg.Msg_Out){   /*����Ϣ��Ҫ����*/
				my_can2_tx_msg.extID=my_can2_send_remote_msg.Can_Msg[my_can2_send_remote_msg.Msg_Out].id;
				my_can2_tx_msg.remoteTxReq=CAN_RTXR_REMOTE;     //֡����
				my_can2_tx_msg.typeID=CAN_TYPEID_EXT;          //��ʶ�����
				my_can2_tx_msg.dataLengthCode=8;  
        can2_send_msg(CAN2,&my_can2_tx_msg);			
			  my_can2_send_remote_msg.Msg_Out=(my_can2_send_remote_msg.Msg_Out+1)%MAX_MSG_NUM;
			} 					
			while(can2_send_buff.input != can2_send_buff.output){	
				can2_send_msg(CAN2,&can2_send_buff.msg[can2_send_buff.output]);
				can2_send_buff.output=(can2_send_buff.output+1)%MAX_KEEP_NUM;
			}
		}
	}
}

static void Can2_Recv_thread_entry(void *parameter)
{
  static rt_err_t result;
	  /*�����ź���*/
	my_can2_sem = rt_sem_create("can2_sem",0,RT_IPC_FLAG_FIFO);	
	
	while(1)
	{
		/*�ȴ��ź���*/
		result = rt_sem_take(my_can2_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){
			rt_sem_delete(my_can2_sem);
		}else{    
			while(my_recv_can2.input!=my_recv_can2.output){  
				my_can2_recv_msg(Recv_Can_buff,&my_recv_can2.msg[my_recv_can2.output]);
				my_recv_can2.output=(my_recv_can2.output+1)%MAX_FILE_NUM;
			}			
		}
	}
}

/*������պ���*/
int my_can_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage)
{
	int id=0;   int  num=0;

	num=(((my_can_rxmessage->stdID)>>4)&(0x000f));
	id =((my_can_rxmessage->stdID)&0x000f);	
	if(my_can_rxmessage->remoteTxReq==0){   /*����֡*/
		/*������can2������*/
		rt_memset(&can2_send_buff.msg[can2_send_buff.input],0,sizeof(can2_send_buff.msg[can2_send_buff.input]));
		rt_memcpy(can2_send_buff.msg[can2_send_buff.input].data,my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		rt_memcpy(&can2_send_buff.msg[can2_send_buff.input].stdID,&my_can_rxmessage->stdID,sizeof(my_can_rxmessage->stdID));
		can2_send_buff.msg[can2_send_buff.input].remoteTxReq=CAN_RTXR_DATA;     //֡����
		can2_send_buff.msg[can2_send_buff.input].dataLengthCode=8;  //���ݳ���
		can2_send_buff.msg[can2_send_buff.input].typeID=CAN_TYPEID_STD;          //��ʶ�����
		can2_send_buff.input=(can2_send_buff.input+1)%MAX_KEEP_NUM;
		/*���������ڴ�����*/
		rt_memcpy(&my_can_msg[num].msg_buff[my_can_msg[num].msg_input][id*8],my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		if(id==0x0009) { /*������ʣ��ռ�*/
//			rt_kprintf("recv can msg num=%d\n",num);
			my_can_msg[num].msg_input=((my_can_msg[num].msg_input+1)%MAX_MSG_NUM);
		}else{  /*��������*/
			;
		}
	}else if(my_can_rxmessage->remoteTxReq){  /*Զ��֡*/
		rt_kprintf("recv rcmsg\n");
		my_remote_msg.Can_Msg[my_remote_msg.Msg_In].id=my_can_rxmessage->extID;
		my_remote_msg.Msg_In=(my_remote_msg.Msg_In+1)%MAX_MSG_NUM;	
	}	
}

/*ע�ắ��*/
int Can_Register(can_recv_data_t *data)
{
	data->recv_can_back(&data->a,&data->b);
	return 1;
}


/*
can���ͺ���

*/
int can1_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage)
{ 
	static rt_err_t result;
	int i=0;	
	
//	rt_kprintf("TxMessage->stdID=%x\n",TxMessage->stdID);
	if(CAN_TxMessage(CAN1,TxMessage)!=3){   /*Ӳ�����ڿ����䣬ֱ�ӷ���*/
	   return 1;
	}else {
		if((can_file_buff.input+1)%MAX_FILE_NUM==can_file_buff.output){
		  return 0;
		}else{
			can_file_buff.msg[can_file_buff.input].dataLengthCode=TxMessage->dataLengthCode;
			can_file_buff.msg[can_file_buff.input].extID=TxMessage->extID;
			can_file_buff.msg[can_file_buff.input].remoteTxReq=TxMessage->remoteTxReq;
			can_file_buff.msg[can_file_buff.input].stdID=TxMessage->stdID;
			can_file_buff.msg[can_file_buff.input].typeID=TxMessage->typeID;
			rt_memcpy(&can_file_buff.msg[can_file_buff.input].data,TxMessage->data,8);
			CAN_EnableInterrupt(CAN1,CAN_INT_TXME);               /*�򿪿������ж�*/
			can_file_buff.input=(can_file_buff.input+1)%MAX_FILE_NUM;
			return 1;
		}
	}
}

/*����can2���պ���*/
int my_can2_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage)
{
	CAN_TxMessage_T  CAN2_TxMessage;
	int id=0;   int  num=0;
	
	num=(((my_can_rxmessage->stdID)>>4)&(0x000f));
	id =((my_can_rxmessage->stdID)&0x000f);	
	if(my_can_rxmessage->remoteTxReq==0){   /*����֡*/
		rt_memcpy(CAN2_TxMessage.data,my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		CAN2_TxMessage.stdID=my_can_rxmessage->stdID;
		CAN2_TxMessage.remoteTxReq=CAN_RTXR_DATA;     //֡����
		CAN2_TxMessage.typeID=CAN_TYPEID_STD;          //��ʶ�����
		CAN2_TxMessage.dataLengthCode=8;		
		/*��can2�յ�����������ͨ��can1���ͳ�ȥ*/
		can1_send_msg(CAN1,&CAN2_TxMessage);
		/*���������ڴ�����*/
		rt_memcpy(&my_can_msg[num].msg_buff[my_can_msg[num].msg_input][id*8],my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		if(id==0x0009) { /*������ʣ��ռ�*/
//			rt_kprintf("recv can msg num=%d\n",num);
			my_can_msg[num].msg_input=((my_can_msg[num].msg_input+1)%MAX_MSG_NUM);
		}else{  /*��������*/
			;
		}
	}else if(my_can_rxmessage->remoteTxReq){  /*Զ��֡*/
		rt_kprintf("recv can2 rcmsg\n");
		my_can2_remote_msg.Can_Msg[my_can2_remote_msg.Msg_In].id=my_can_rxmessage->extID;
		my_can2_remote_msg.Msg_In=(my_can2_remote_msg.Msg_In+1)%MAX_MSG_NUM;
	}	
}

/*
can2���ͺ���

*/
int can2_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage)
{ 
	static rt_err_t result;
	int i=0;

//  rt_kprintf("start send can2 msg\n");	
	if(CAN_TxMessage(CAN2,TxMessage)!=3){   /*Ӳ�����ڿ����䣬ֱ�ӷ���*/
	   return 1;
	}else {
		if((can2_send_file_buff.input+1)%MAX_KEEP_NUM==can2_send_file_buff.output){
		  return 0;
		}else{
			can2_send_file_buff.msg[can2_send_file_buff.input].dataLengthCode=TxMessage->dataLengthCode;
			can2_send_file_buff.msg[can2_send_file_buff.input].extID=TxMessage->extID;
			can2_send_file_buff.msg[can2_send_file_buff.input].remoteTxReq=TxMessage->remoteTxReq;
			can2_send_file_buff.msg[can2_send_file_buff.input].stdID=TxMessage->stdID;
			can2_send_file_buff.msg[can2_send_file_buff.input].typeID=TxMessage->typeID;
			rt_memcpy(&can2_send_file_buff.msg[can2_send_file_buff.input].data,TxMessage->data,8);
			CAN_EnableInterrupt(CAN2,CAN_INT_TXME);               /*�򿪿������ж�*/
			can2_send_file_buff.input=(can2_send_file_buff.input+1)%MAX_KEEP_NUM;
			return 1;
		}
	}
}

