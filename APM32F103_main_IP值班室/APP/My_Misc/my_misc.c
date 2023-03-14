#include "my_misc.h"

void start_voice_play(void)
{
	/*串口使能*/
	USART_EnableInterrupt(USART2, USART_INT_IDLE|USART_INT_RXBNE);   /*打开空闲中断*/	
	NVIC_EnableIRQRequest(USART2_IRQn, 0, 0);        /*配置中断优先级*/
	
	DMA_Disable(DMA1_Channel6);//关闭DMA
	DMA_ConfigDataNumber(DMA1_Channel6,MAX_USART_RXBUFLEN);
	DMA_Enable(DMA1_Channel6);
//	DMA_EnableInterrupt(DMA1_Channel6, ENABLE);	
	
//	GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,1);
//	GPIO_WriteBitValue(GPIOB,GPIO_PIN_15,0);	
}


void stop_voice_play(void)
{
	/*串口失能*/
	USART_DisableInterrupt(USART2, USART_INT_IDLE|USART_INT_RXBNE);   /*关闭空闲中断*/		
	DMA_Disable(DMA1_Channel6);//关闭DMA
	DMA_ClearStatusFlag(DMA1_INT_FLAG_GINT6);	
  
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,0);
}

void init_data(void)
{
  My_Usart_Send_Buff.temp=0;
	my_usart_msg.temp=0;
	My_Attribute.state=Stand_State;
	My_Attribute.addr=0;
	
}

