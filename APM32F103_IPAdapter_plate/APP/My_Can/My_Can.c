#include "My_Can.h"


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


/*����һ��can�����߳�*/
int build_can_recv_thread(void)
{
	/*����һ������4���ͻ�����*/
		can1_send_mutex=rt_mutex_create("can1_send_mutex",RT_IPC_FLAG_FIFO);			
		/*�����߳�*/
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
}
/*����һ��can�����߳�*/
int build_can_send_thread(void)
{
	/*�������ͳɹ��ź���*/
	  my_cansend_sem = rt_sem_create("my_cansend_sem",3,RT_IPC_FLAG_FIFO);
	/*����can�����ź���*/
		my_can_needsend_sem = rt_sem_create("my_cannendsend_sem",0,RT_IPC_FLAG_FIFO);	
	
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
}

/*���մ�����*/
static void Can_Recv_thread_entry(void *parameter)
{

	  /*�����ź���*/
	my_can_sem = rt_sem_create("can_sem",0,RT_IPC_FLAG_FIFO);	
	static rt_err_t result;
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
				rt_kprintf("send remote_can msg\n");
				my_can_tx_msg.extID=my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_Out].id;
				my_can_tx_msg.remoteTxReq=CAN_RTXR_REMOTE;     //֡����
				my_can_tx_msg.typeID=CAN_TYPEID_EXT;          //��ʶ�����
				my_can_tx_msg.dataLengthCode=8;
				can1_send_msg(CAN1,&my_can_tx_msg);
				my_send_remote_msg.Msg_Out=(my_send_remote_msg.Msg_Out+1)%MAX_MSG_NUM;
			}else{				//������֡��Ҫ����
				for(i=0;i<MAX_SEND_NUM;i++){
					rt_memcpy(my_can_tx_msg.data,&udp_7777_recv_buff.msg_buff[udp_7777_recv_buff.msg_output][i*8],8);
					my_can_tx_msg.stdID=i | 0x0040;
					my_can_tx_msg.remoteTxReq=CAN_RTXR_DATA;     //֡����	
					my_can_tx_msg.dataLengthCode=8;  //���ݳ���		
					my_can_tx_msg.typeID=CAN_TYPEID_STD;          //��ʶ�����					
					ret=can1_send_msg(CAN1,&my_can_tx_msg);  
				}
			udp_7777_recv_buff.msg_output=((udp_7777_recv_buff.msg_output+1)%MAX_NET_MSG_NUM);
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
		rt_memcpy(&my_can_msg[num].msg_buff[my_can_msg[num].msg_input][id*8],my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		if(id==0x0009) { /*������ʣ��ռ�*/
			my_can_msg[num].msg_input=((my_can_msg[num].msg_input+1)%MAX_MSG_NUM);
		}else{  /*��������*/
			;
		}
	}else if(my_can_rxmessage->remoteTxReq){  /*Զ��֡*/
		rt_kprintf("recv remoteTxReq msg\n");
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

