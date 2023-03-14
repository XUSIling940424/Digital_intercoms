#include "Usart.h"

/*串口2发送缓冲区*/
u8 TxBuffer2[MAX_USART_TXBUFLEN]={0};
/*串口2接收缓冲区*/
u8 RxBuffer2[MAX_USART_RXBUFLEN]={0};
/*串口2接收缓冲区*/
u8 RX_USART2_BUFF[MAX_USART_RXBUFLEN]={0};
/*串口2接收标志位*/
u8 Rx_USART2_Flags=0;
/*串口2接收长度*/
u8 Rx_USART2_Len=0;

/*串口3发送缓冲区*/
u8 TxBuffer3[MAX_USART_TXBUFLEN]={0};
/*串口3接收缓冲区*/
u8 RxBuffer3[MAX_USART_RXBUFLEN]={0};
/*串口3接收缓冲区*/
u8 RX_USART3_BUFF[MAX_USART_RXBUFLEN]={0};
/*串口3接收标志位*/
u8 Rx_USART3_Flags=0;
/*串口3接收长度*/
u8 Rx_USART3_Len=0;

struct  Usart_Msg_send  My_Usart_Send_Buff;

unsigned char Usart2_Dma_Init(unsigned int boom)
{
	USART_Config_T USART_ConfigStruct;
	GPIO_Config_T GPIO_ConfigStruct;
	DMA_Config_T    DMA_ConfigStruct;
	
	RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)RCM_APB2_PERIPH_GPIOA);
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);
	RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);
	/*Usart tx*/
	GPIO_ConfigStruct.mode = GPIO_MODE_AF_PP;
	GPIO_ConfigStruct.pin = GPIO_PIN_2;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);
  /*Usart rx*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_FLOATING;
	GPIO_ConfigStruct.pin = GPIO_PIN_3;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
  /*usart config*/
	USART_ConfigStruct.baudRate = boom;
	USART_ConfigStruct.mode = USART_MODE_TX_RX;
	USART_ConfigStruct.parity = USART_PARITY_NONE;
	USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
	USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
	USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
	USART_Config(USART2, &USART_ConfigStruct);
	
//	USART_EnableInterrupt(USART2, USART_INT_IDLE);   /*打开空闲中断*/	
//	NVIC_EnableIRQRequest(USART2_IRQn, 1, 0);        /*配置中断优先级*/
	USART_Enable(USART2);	
	/*USART2_RXDMAconfig*/
	DMA_Reset(DMA1_Channel6);
  DMA_ConfigStructInit(&DMA_ConfigStruct);
	DMA_ConfigStruct.peripheralBaseAddr = (u32)&(USART2->DATA);  //外设地址
	DMA_ConfigStruct.memoryBaseAddr = (uint32_t)RxBuffer2;    //存储器地址
	DMA_ConfigStruct.dir = DMA_DIR_PERIPHERAL_SRC;     //传输方向
	DMA_ConfigStruct.bufferSize = MAX_USART_RXBUFLEN;     //传输数目
	DMA_ConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;   //外设地址增量模式
	DMA_ConfigStruct.memoryInc = DMA_MEMORY_INC_ENABLE;   //存储器地址增量模式
	DMA_ConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;   //外设数据宽度
	DMA_ConfigStruct.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;  //存储器数据宽度
	DMA_ConfigStruct.loopMode = DMA_MODE_NORMAL;   //模式选择
	DMA_ConfigStruct.priority = DMA_PRIORITY_HIGH;   //通道优先级
	DMA_ConfigStruct.M2M = DMA_M2MEN_DISABLE;     //存储器到存储器模式
	DMA_Config(DMA1_Channel6, &DMA_ConfigStruct);
	
	/*USART2_TXDMAconfig*/
	DMA_Reset(DMA1_Channel7);
  DMA_ConfigStructInit(&DMA_ConfigStruct);
	DMA_ConfigStruct.peripheralBaseAddr = (u32)&(USART2->DATA);  //外设地址
	DMA_ConfigStruct.memoryBaseAddr = (u32)TxBuffer2;    //存储器地址
	DMA_ConfigStruct.dir = DMA_DIR_PERIPHERAL_DST;     //传输方向
	DMA_ConfigStruct.bufferSize = MAX_USART_RXBUFLEN;     //传输数目
	DMA_ConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;   //外设地址增量模式
	DMA_ConfigStruct.memoryInc = DMA_MEMORY_INC_ENABLE;   //存储器地址增量模式
	DMA_ConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;   //外设数据宽度
	DMA_ConfigStruct.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;  //存储器数据宽度
	DMA_ConfigStruct.loopMode = DMA_MODE_NORMAL;   //模式选择
	DMA_ConfigStruct.priority = DMA_PRIORITY_HIGH;   //通道优先级
	DMA_ConfigStruct.M2M = DMA_M2MEN_DISABLE;     //存储器到存储器模式
	DMA_Config(DMA1_Channel7, &DMA_ConfigStruct);	

  USART_EnableDMA(USART2,USART_DMA_TX| USART_DMA_RX);
	DMA_EnableInterrupt(DMA1_Channel7, DMA_INT_TC);
  NVIC_EnableIRQRequest(DMA1_Channel7_IRQn, 1, 0);        /*配置中断优先级*/	

	return 0;	
}


unsigned char Usart3_Dma_Init(unsigned int boom)
{
	USART_Config_T USART_ConfigStruct;
	GPIO_Config_T GPIO_ConfigStruct;
	DMA_Config_T    DMA_ConfigStruct;
	
	RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)RCM_APB2_PERIPH_GPIOB);
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART3);
	RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);
	/*Usart tx*/
	GPIO_ConfigStruct.mode = GPIO_MODE_AF_PP;
	GPIO_ConfigStruct.pin = GPIO_PIN_10;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);
  /*Usart rx*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_FLOATING;
	GPIO_ConfigStruct.pin = GPIO_PIN_11;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
  /*usart config*/
	USART_ConfigStruct.baudRate = boom;
	USART_ConfigStruct.mode = USART_MODE_TX_RX;
	USART_ConfigStruct.parity = USART_PARITY_NONE;
	USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
	USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
	USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
	USART_Config(USART3, &USART_ConfigStruct);
	
	USART_EnableInterrupt(USART3, USART_INT_IDLE | USART_INT_RXBNE);   /*打开空闲中断和接收中断*/	
	NVIC_EnableIRQRequest(USART3_IRQn, 1, 0);        /*配置中断优先级*/
	
	USART_Enable(USART3);	
	
	/*DMA_rxd_config*/
	DMA_Reset(DMA1_Channel3);
  DMA_ConfigStructInit(&DMA_ConfigStruct);
	DMA_ConfigStruct.peripheralBaseAddr = (u32)&(USART3->DATA);  //外设地址
	DMA_ConfigStruct.memoryBaseAddr = (uint32_t)RxBuffer3;    //存储器地址
	DMA_ConfigStruct.dir = DMA_DIR_PERIPHERAL_SRC;     //传输方向
	DMA_ConfigStruct.bufferSize = MAX_USART_RXBUFLEN;     //传输数目
	DMA_ConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;   //外设地址增量模式
	DMA_ConfigStruct.memoryInc = DMA_MEMORY_INC_ENABLE;   //存储器地址增量模式
	DMA_ConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;   //外设数据宽度
	DMA_ConfigStruct.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;  //存储器数据宽度
	DMA_ConfigStruct.loopMode = DMA_MODE_NORMAL;   //模式选择
	DMA_ConfigStruct.priority = DMA_PRIORITY_HIGH;   //通道优先级
	DMA_ConfigStruct.M2M = DMA_M2MEN_DISABLE;     //存储器到存储器模式
	DMA_Config(DMA1_Channel3, &DMA_ConfigStruct);
	DMA_EnableInterrupt(DMA1_Channel3, DMA_INT_TC);		
	NVIC_EnableIRQRequest(DMA1_Channel3_IRQn, 1, 0);        /*配置中断优先级*/
	DMA_Enable(DMA1_Channel3);

	/*USART3_TXDMAconfig*/
	DMA_Reset(DMA1_Channel2);
  DMA_ConfigStructInit(&DMA_ConfigStruct);
	DMA_ConfigStruct.peripheralBaseAddr = (u32)&(USART3->DATA);  //外设地址
	DMA_ConfigStruct.memoryBaseAddr = (u32)TxBuffer3;    //存储器地址
	DMA_ConfigStruct.dir = DMA_DIR_PERIPHERAL_DST;     //传输方向
	DMA_ConfigStruct.bufferSize = MAX_USART_RXBUFLEN;     //传输数目
	DMA_ConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;   //外设地址增量模式
	DMA_ConfigStruct.memoryInc = DMA_MEMORY_INC_ENABLE;   //存储器地址增量模式
	DMA_ConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_BYTE;   //外设数据宽度
	DMA_ConfigStruct.memoryDataSize = DMA_MEMORY_DATA_SIZE_BYTE;  //存储器数据宽度
	DMA_ConfigStruct.loopMode = DMA_MODE_NORMAL;   //模式选择
	DMA_ConfigStruct.priority = DMA_PRIORITY_HIGH;   //通道优先级
	DMA_ConfigStruct.M2M = DMA_M2MEN_DISABLE;     //存储器到存储器模式
	DMA_Config(DMA1_Channel2, &DMA_ConfigStruct);		

  USART_EnableDMA(USART3,USART_DMA_TX| USART_DMA_RX);
	DMA_EnableInterrupt(DMA1_Channel2, DMA_INT_TC);		
	NVIC_EnableIRQRequest(DMA1_Channel2_IRQn, 1, 0);        /*配置中断优先级*/

	return 0;
}

void USART2_IRQHandler(void)
{
	u16 clear;
	rt_ubase_t len=0;	
 if(USART_ReadIntFlag(USART2, USART_INT_IDLE) != RESET) //进入空闲中断
 {
	 USART_ClearIntFlag(USART2,USART_INT_IDLE);
   USART2->DATA;  //读取状态寄存器再读取数据缓存清零标志位
	 DMA_ClearStatusFlag(DMA1_INT_FLAG_GINT6);  /*清除标志位*/
	 start_time();   /*开启定时器检测*/
	 Rx_USART2_Flags=1;
 }else if(USART_ReadIntFlag(USART2, USART_INT_RXBNE) == RESET){  //进入接收中断
	 my_usart_msg.temp=1;   
	 USART_ClearIntFlag(USART2,USART_INT_RXBNE);
	 USART_DisableInterrupt(USART2,USART_INT_RXBNE);
 }
}


void USART3_IRQHandler(void)
{
	u16 clear;

 if(USART_ReadIntFlag(USART3, USART_INT_IDLE) != RESET) //进入空闲中断
 {	
   rt_kprintf("USART3_IRQHandler\n");	 
   USART3->DATA;  //读取状态寄存器再读取数据缓存清零标志位
	 Rx_USART3_Flags=1;
	 USART_ClearIntFlag(USART3,USART_INT_IDLE);
	 rt_sem_release(my_usart3_sem);  /*释放信号量*/
 }
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA_ReadIntFlag(DMA1_INT_FLAG_TC7) !=RESET){   /*dma发送完毕中断*/
		if(My_Usart_Send_Buff.input!=My_Usart_Send_Buff.output){
		  DMA_Disable(DMA1_Channel7);//关闭DMA
			rt_memcpy(TxBuffer2,My_Usart_Send_Buff.msg[My_Usart_Send_Buff.output],120);
			DMA_ConfigDataNumber(DMA1_Channel7,120);
			DMA_Enable(DMA1_Channel7);
      My_Usart_Send_Buff.output=(My_Usart_Send_Buff.output+1)%MAX_USART_SENDLEN;			
		}else if(My_Usart_Send_Buff.input==My_Usart_Send_Buff.output){
		  DMA_Disable(DMA1_Channel7);//关闭DMA
			My_Usart_Send_Buff.temp=0;
		}
		DMA_ClearStatusFlag(DMA1_INT_FLAG_TC7);
	}
}

void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_ReadIntFlag(DMA1_INT_FLAG_TC2) !=RESET){   /*dma发送完毕中断*/
//		if(My_Usart_Send_Buff.input!=My_Usart_Send_Buff.output){
//		  DMA_Disable(DMA1_Channel7);//关闭DMA
//			rt_memcpy(TxBuffer2,My_Usart_Send_Buff.msg[My_Usart_Send_Buff.output],120);
//			DMA_ConfigDataNumber(DMA1_Channel7,120);
//			DMA_Enable(DMA1_Channel7);
//      My_Usart_Send_Buff.output=(My_Usart_Send_Buff.output+1)%MAX_USART_SENDLEN;			
//		}else if(My_Usart_Send_Buff.input==My_Usart_Send_Buff.output){
//		  DMA_Disable(DMA1_Channel7);//关闭DMA
//			My_Usart_Send_Buff.temp=0;
//		}
		DMA_ClearStatusFlag(DMA1_INT_FLAG_TC2);
	}
}
