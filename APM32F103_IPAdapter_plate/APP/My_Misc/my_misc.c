#include "my_misc.h"

void start_voice_play(void)
{
	/*串口使能*/
	USART_EnableInterrupt(USART2, USART_INT_IDLE|USART_INT_RXBNE);   /*打开空闲中断*/	
	NVIC_EnableIRQRequest(USART2_IRQn, 0, 0);        /*配置中断优先级*/
		
	DMA_ConfigDataNumber(DMA1_Channel6,MAX_USART_RXBUFLEN);
	DMA_Enable(DMA1_Channel6);
//	DMA_EnableInterrupt(DMA1_Channel6, ENABLE);	
	
	my_cansend_sem->value=3;
	my_send_buff_fifo.send_input=0;
	my_send_buff_fifo.send_output=0;
	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,1);
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_15,0);	
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
	My_Attribute.state=Stand_State;
	My_Attribute.addr=0;
	
}
/*读取地址信息*/
u8  Read_Addr_Msg(void)
{
	u8 addr=0;
	
	if(GPIO_ReadInputBit(GPIOC,GPIO_PIN_13) == 0)   {addr |= (0x01);}
	if(GPIO_ReadInputBit(GPIOA,GPIO_PIN_0) == 0)   {addr |= (0x02);}
	if(GPIO_ReadInputBit(GPIOA,GPIO_PIN_1) == 0)   {addr |= (0x04);}
	if(GPIO_ReadInputBit(GPIOB,GPIO_PIN_12) == 0)   {addr |= (0x08);}
	if(GPIO_ReadInputBit(GPIOB,GPIO_PIN_13) == 0)   {addr |= (0x10);}
	if(GPIO_ReadInputBit(GPIOB,GPIO_PIN_14) == 0)   {addr |= (0x20);}
	if(GPIO_ReadInputBit(GPIOB,GPIO_PIN_15) == 0)   {addr |= (0x40);}
	if(GPIO_ReadInputBit(GPIOA,GPIO_PIN_8) == 0)   {addr |= (0x80);}
	
	return addr;
	
}
