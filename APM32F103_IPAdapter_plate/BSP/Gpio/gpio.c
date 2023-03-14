#include "gpio.h"


void gpio_init(void)
{
	GPIO_Config_T GPIO_ConfigStruct;
	
	RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)RCM_APB2_PERIPH_GPIOB | RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_GPIOC);		
  /*led1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_9;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);	
  /*led2*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_8;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,1);	
  /*led3*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_7;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);
  /*led4*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_6;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);		

	/*add*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_8 | GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	

	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_13;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);		
}


