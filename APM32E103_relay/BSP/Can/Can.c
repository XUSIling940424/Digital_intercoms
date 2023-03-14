#include "Can.h"

struct can_send_file_buff    can_file_buff;
struct can_recv_buff    my_recv_can;
struct can2_send_buff_t     can2_send_buff;
struct can2_send_buff_t     can2_send_file_buff;
struct can_recv_buff    my_recv_can2;

CAN_TxMessage_T   TxMessage;
CAN_RxMessage_T   RxMessage;

int Can1_Init(void)
{
	GPIO_Config_T 		 configStruct;
	CAN_Config_T       canConfig;
	CAN_FilterConfig_T FilterStruct;	
	
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN1);

	configStruct.pin   = GPIO_PIN_12;    // CAN1 Tx
	configStruct.mode  = GPIO_MODE_AF_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &configStruct);

	configStruct.pin  = GPIO_PIN_11;     // CAN1 Rx
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &configStruct);	
	
	CAN_Reset(CAN1);
	CAN_ConfigStructInit(&canConfig);	
	
	/* CAN1 init */
	canConfig.autoBusOffManage = ENABLE;   //自动总线关闭管理
	canConfig.autoWakeUpMode   = ENABLE;   //自动唤醒
	canConfig.nonAutoRetran    = DISABLE;    //自动重传功能
	canConfig.rxFIFOLockMode   = DISABLE;   //FIFO锁定模式
	canConfig.txFIFOPriority   = DISABLE;   //传输FIFO优先级
	canConfig.mode             = CAN_MODE_NORMAL;//模式选择
	canConfig.syncJumpWidth    = CAN_SJW_1;
	canConfig.timeSegment1     = CAN_TIME_SEGMENT1_11;
	canConfig.timeSegment2     = CAN_TIME_SEGMENT2_3;
	canConfig.prescaler = 40;

	CAN_Config(CAN1,&canConfig);	
	
	FilterStruct.filterNumber = 0;
	FilterStruct.filterMode = CAN_FILTER_MODE_IDMASK;
	FilterStruct.filterScale = CAN_FILTER_SCALE_32BIT;
	FilterStruct.filterIdHigh = 0x0000;
	FilterStruct.filterIdLow  = 0x0000;
	FilterStruct.filterMaskIdHigh = 0x0000;
	FilterStruct.filterMaskIdLow  = 0x0000;
	FilterStruct.filterFIFO = CAN_FILTER_FIFO_0;
	FilterStruct.filterActivation =  ENABLE;

	CAN_ConfigFilter(CAN1,&FilterStruct);	
	
	/*can打开*/
//	CAN_EnableInterrupt(CAN1,CAN_INT_TXME);               /*打开空邮箱中断*/	
	NVIC_EnableIRQRequest(USBD1_HP_CAN1_TX_IRQn, 0, 1);  
	
}


int Can2_Init(void)
{
	GPIO_Config_T 		 configStruct;
	CAN_Config_T       canConfig;
	CAN_FilterConfig_T FilterStruct;
	
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOB);
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN2);
	
	configStruct.pin   = GPIO_PIN_13;    // CAN2 Tx
	configStruct.mode  = GPIO_MODE_AF_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &configStruct);

	configStruct.pin  = GPIO_PIN_12;     // CAN2 Rx
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &configStruct);

	CAN_Reset(CAN2);
	CAN_ConfigStructInit(&canConfig);	
	
	/* CAN2 init */
	canConfig.autoBusOffManage = ENABLE;   //自动总线关闭管理
	canConfig.autoWakeUpMode   = ENABLE;   //自动唤醒
	canConfig.nonAutoRetran    = DISABLE;    //自动重传功能
	canConfig.rxFIFOLockMode   = DISABLE;   //FIFO锁定模式
	canConfig.txFIFOPriority   = DISABLE;   //传输FIFO优先级
	canConfig.mode             = CAN_MODE_NORMAL;//模式选择
	canConfig.syncJumpWidth    = CAN_SJW_1;
	canConfig.timeSegment1     = CAN_TIME_SEGMENT1_11;
	canConfig.timeSegment2     = CAN_TIME_SEGMENT2_3;
	canConfig.prescaler = 40;

	CAN_Config(CAN2,&canConfig);

	FilterStruct.filterNumber = 0;
	FilterStruct.filterMode = CAN_FILTER_MODE_IDMASK;
	FilterStruct.filterScale = CAN_FILTER_SCALE_32BIT;
	FilterStruct.filterIdHigh = 0x0000;
	FilterStruct.filterIdLow  = 0x0000;
	FilterStruct.filterMaskIdHigh = 0x0000;
	FilterStruct.filterMaskIdLow  = 0x0000;
	FilterStruct.filterFIFO = CAN_FILTER_FIFO_1;
	FilterStruct.filterActivation =  ENABLE;

	CAN_ConfigFilter(CAN2,&FilterStruct);	
	
	/*can打开*/
//	CAN_EnableInterrupt(CAN2,CAN_INT_TXME);               /*打开空邮箱中断*/	
	NVIC_EnableIRQRequest(USBD2_HP_CAN2_TX_IRQn, 0, 2); 
}


void USBD1_LP_CAN1_RX0_IRQHandler (void)
{
	
	CAN_TxMessage_T TxMessage;
	
	if(CAN_ReadIntFlag(CAN1,CAN_INT_F0MP) == SET)   /*接收中断*/
	{
		CAN_ClearIntFlag(CAN1,CAN_INT_F0MP);
		if((my_recv_can.input+1)%MAX_FILE_NUM!=my_recv_can.output){
			CAN_RxMessage(CAN1,CAN_RX_FIFO_0,&my_recv_can.msg[my_recv_can.input]);
			my_recv_can.input=(my_recv_can.input+1)%MAX_FILE_NUM;
			TxMessage.dataLengthCode=my_recv_can.msg[my_recv_can.output].dataLengthCode;
			TxMessage.extID=my_recv_can.msg[my_recv_can.output].extID;
			TxMessage.remoteTxReq=my_recv_can.msg[my_recv_can.output].remoteTxReq;
			TxMessage.stdID=my_recv_can.msg[my_recv_can.output].stdID;
			TxMessage.typeID=my_recv_can.msg[my_recv_can.output].typeID;
			rt_memcpy(TxMessage.data,my_recv_can.msg[my_recv_can.output].data,my_recv_can.msg[my_recv_can.output].dataLengthCode);
			my_recv_can.output=(my_recv_can.output+1)%MAX_FILE_NUM;
			can2_send_msg(CAN2,&TxMessage);			
//			rt_sem_release(my_can_sem);  /*释放信号量*/
		}else{
		  ;
		}
	}

}

void USBD1_HP_CAN1_TX_IRQHandler(void)
{
	if(CAN_ReadIntFlag(CAN1,CAN_INT_TXME) == SET)  /*发送完成中断*/
	{
		if(can_file_buff.input!=can_file_buff.output){   /*存在未发送完成*/
			CAN_TxMessage(CAN1,&can_file_buff.msg[can_file_buff.output]);
			can_file_buff.output=(can_file_buff.output+1)%MAX_FILE_NUM;
		}else if(can_file_buff.input==can_file_buff.output){
		  CAN_DisableInterrupt(CAN1,CAN_INT_TXME);
		}
		CAN_ClearIntFlag(CAN1,CAN_INT_TXME);
//		rt_sem_release(my_cansend_sem);  /*释放信号量*/
	}
}

void CAN2_RX1_IRQHandler(void)
{
	CAN_TxMessage_T TxMessage;
	
	
	if(CAN_ReadIntFlag(CAN2,CAN_INT_F1MP) == SET)  /*接收完成中断*/
	{
		if((my_recv_can2.input+1)%MAX_FILE_NUM!=my_recv_can2.output){
			CAN_RxMessage(CAN2,CAN_RX_FIFO_1,&my_recv_can2.msg[my_recv_can2.input]);
			my_recv_can2.input=(my_recv_can2.input+1)%MAX_FILE_NUM;
			TxMessage.dataLengthCode=my_recv_can2.msg[my_recv_can2.output].dataLengthCode;
			TxMessage.extID=my_recv_can2.msg[my_recv_can2.output].extID;
			TxMessage.remoteTxReq=my_recv_can2.msg[my_recv_can2.output].remoteTxReq;
			TxMessage.stdID=my_recv_can2.msg[my_recv_can2.output].stdID;
			TxMessage.typeID=my_recv_can2.msg[my_recv_can2.output].typeID;
			rt_memcpy(TxMessage.data,my_recv_can2.msg[my_recv_can2.output].data,my_recv_can2.msg[my_recv_can2.output].dataLengthCode);
			my_recv_can2.output=(my_recv_can2.output+1)%MAX_FILE_NUM;
			can1_send_msg(CAN1,&TxMessage);
//			rt_sem_release(my_can2_sem);  /*释放信号量*/
		}else{
			;
		}
		CAN_ClearIntFlag(CAN2,CAN_INT_F1MP);
	}
}


void USBD2_HP_CAN2_TX_IRQHandler(void)
{
	if(CAN_ReadIntFlag(CAN2,CAN_INT_TXME) == SET)  /*发送完成中断*/
	{
		if(can2_send_file_buff.input!=can2_send_file_buff.output){
			CAN_TxMessage(CAN2,&can2_send_file_buff.msg[can2_send_file_buff.output]);
			can2_send_file_buff.output=(can2_send_file_buff.output+1)%MAX_KEEP_NUM;
		}else{
		  CAN_DisableInterrupt(CAN2,CAN_INT_TXME);
		}
		CAN_ClearIntFlag(CAN2,CAN_INT_TXME);		
	}
}


