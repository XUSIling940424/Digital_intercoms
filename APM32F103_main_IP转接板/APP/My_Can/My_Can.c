#include "My_Can.h"


/*主机状态属性*/
struct Attribute_t  My_Attribute;

/*can语音消息队列*/
struct Can_Msg  Recv_Can_buff[5];
/*can接收回调函数管理器*/
can_recv_data_t  my_can_recv_data;
/*can控制命令消息队列*/
struct Remote_Msg   my_remote_msg;
/*can发送控制命令消息队列*/
struct Remote_Msg   my_send_remote_msg;

/*can消息发送缓存*/
struct Can_Send_Buff   my_send_buff_fifo;

/*can1消息队列*/
static rt_mq_t can1_mq=RT_NULL;
/*定义线程控制块指针*/
static rt_thread_t Can_Recv_thread = RT_NULL;	
/*函数声明*/
static void Can_Recv_thread_entry(void *parameter);
/*定义线程控制块指针*/
static rt_thread_t Can_Send_thread = RT_NULL;	
/*函数声明*/
static void Can_Send_thread_entry(void *parameter);
/*can1发送互斥锁*/
rt_mutex_t can1_send_mutex;
/*指向can信号量的指针*/
rt_sem_t  my_can_sem = RT_NULL;
/*can发送信号量*/
rt_sem_t  my_cansend_sem = RT_NULL;  
/*can发送信号量*/
rt_sem_t  my_can_needsend_sem = RT_NULL;


/*创建一个can接收线程*/
int build_can_recv_thread(void)
{
	/*创建一个串口4发送互斥锁*/
		can1_send_mutex=rt_mutex_create("can1_send_mutex",RT_IPC_FLAG_FIFO);			
		/*创建线程*/
		Can_Recv_thread=                              /* 线程控制块指针 */
		rt_thread_create( "can_recv",                 /* 线程名字 */
											Can_Recv_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											2,                      /* 线程的优先级 */
											1000);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(Can_Recv_thread != RT_NULL){
			rt_kprintf("rt_thread_canrecv_create is success\n");
			rt_thread_startup(Can_Recv_thread);
		}
		else{ 
			rt_kprintf("rt_thread_canrecv_create is faile\n");
			return -1;	
		}
}
/*创建一个can发送线程*/
int build_can_send_thread(void)
{
	/*创建发送成功信号量*/
	  my_cansend_sem = rt_sem_create("my_cansend_sem",3,RT_IPC_FLAG_FIFO);
	/*创建can发送信号量*/
		my_can_needsend_sem = rt_sem_create("my_cannendsend_sem",0,RT_IPC_FLAG_FIFO);	
	
		/*创建线程*/
		Can_Send_thread=                              /* 线程控制块指针 */
		rt_thread_create( "can_send",                 /* 线程名字 */
											Can_Send_thread_entry,      /* 线程入口函数 */
											RT_NULL,                /* 线程入口函数参数 */
											1024,                    /* 线程栈大小 */
											3,                      /* 线程的优先级 */
											1000);                    /* 线程时间片 */
		
		/*启动线程，开启调度*/
		if(Can_Send_thread != RT_NULL){
			rt_kprintf("rt_thread_cansend_create is success\n");
			rt_thread_startup(Can_Send_thread);
		}
		else{ 
			rt_kprintf("rt_thread_cansend_create is faile\n");
			return -1;	
		}
}

/*接收处理函数*/
static void Can_Recv_thread_entry(void *parameter)
{

	  /*创建信号量*/
	my_can_sem = rt_sem_create("can_sem",0,RT_IPC_FLAG_FIFO);	
	static rt_err_t result;
	while(1)
	{
		/*等待信号量*/
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
	
	my_can_tx_msg.stdID=0x0420000;   //标准帧标识符
  my_can_tx_msg.extID=0x00420000;  //扩展帧标识符
	my_can_tx_msg.typeID=CAN_TYPEID_EXT;          //标识符类别
	my_can_tx_msg.remoteTxReq=CAN_RTXR_DATA;     //帧类型	
	my_can_tx_msg.dataLengthCode=8;  //数据长度
	my_can_tx_msg.remoteTxReq=0;     //具体数据
	
	while(1){
	/*等待发送信号量*/
		result = rt_sem_take(my_can_needsend_sem,RT_WAITING_FOREVER);
		if(result != RT_EOK){   /*信号量未释放*/
			rt_sem_delete(my_can_needsend_sem);
		}else{    /*有数据需要发送*/
			if(my_send_remote_msg.Msg_In!=my_send_remote_msg.Msg_Out)   //有远程帧需要发送
			{
				rt_kprintf("send remote_can msg\n");
				my_can_tx_msg.extID=my_send_remote_msg.Can_Msg[my_send_remote_msg.Msg_Out].id;
				my_can_tx_msg.remoteTxReq=CAN_RTXR_REMOTE;     //帧类型
				my_can_tx_msg.typeID=CAN_TYPEID_EXT;          //标识符类别
				my_can_tx_msg.dataLengthCode=8;
				can1_send_msg(CAN1,&my_can_tx_msg);
				my_send_remote_msg.Msg_Out=(my_send_remote_msg.Msg_Out+1)%MAX_MSG_NUM;
			}else{				//有数据帧需要发送
				for(i=0;i<MAX_SEND_NUM;i++){
					rt_memcpy(my_can_tx_msg.data,&udp_7777_recv_buff.msg_buff[udp_7777_recv_buff.msg_output][i*8],8);
					my_can_tx_msg.stdID=i | 0x0040;
					my_can_tx_msg.remoteTxReq=CAN_RTXR_DATA;     //帧类型	
					my_can_tx_msg.dataLengthCode=8;  //数据长度		
					my_can_tx_msg.typeID=CAN_TYPEID_STD;          //标识符类别					
					ret=can1_send_msg(CAN1,&my_can_tx_msg);  
				}
			udp_7777_recv_buff.msg_output=((udp_7777_recv_buff.msg_output+1)%MAX_NET_MSG_NUM);
			}
		}			
	}
}

/*具体接收函数*/
int my_can_recv_msg(struct Can_Msg my_can_msg[5],CAN_RxMessage_T* my_can_rxmessage)
{
	int id=0;   int  num=0;

	num=(((my_can_rxmessage->stdID)>>4)&(0x000f));
	id =((my_can_rxmessage->stdID)&0x000f);	
	if(my_can_rxmessage->remoteTxReq==0){   /*数据帧*/
		rt_memcpy(&my_can_msg[num].msg_buff[my_can_msg[num].msg_input][id*8],my_can_rxmessage->data,my_can_rxmessage->dataLengthCode);
		if(id==0x0009) { /*缓存有剩余空间*/
			my_can_msg[num].msg_input=((my_can_msg[num].msg_input+1)%MAX_MSG_NUM);
		}else{  /*其余条件*/
			;
		}
	}else if(my_can_rxmessage->remoteTxReq){  /*远程帧*/
		rt_kprintf("recv remoteTxReq msg\n");
		my_remote_msg.Can_Msg[my_remote_msg.Msg_In].id=my_can_rxmessage->extID;
		my_remote_msg.Msg_In=(my_remote_msg.Msg_In+1)%MAX_MSG_NUM;
	}	
}

/*注册函数*/
int Can_Register(can_recv_data_t *data)
{
	data->recv_can_back(&data->a,&data->b);
	return 1;
}


/*
can发送函数

*/
int can1_send_msg(CAN_T* can, CAN_TxMessage_T* TxMessage)
{ 
	static rt_err_t result;
	int i=0;	
	if(CAN_TxMessage(CAN1,TxMessage)!=3){   /*硬件存在空邮箱，直接发送*/
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
			CAN_EnableInterrupt(CAN1,CAN_INT_TXME);               /*打开空邮箱中断*/
			can_file_buff.input=(can_file_buff.input+1)%MAX_FILE_NUM;
			return 1;
		}
	}
}

