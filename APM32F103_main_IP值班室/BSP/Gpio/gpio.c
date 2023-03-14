#include "gpio.h"


void gpio_init(void)
{
	GPIO_Config_T GPIO_ConfigStruct;
	
	RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)RCM_APB2_PERIPH_GPIOB | RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_GPIOC);
	/*x0*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_9;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);
	/*x1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_8;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*X2*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_7;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*X3*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_6;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*X4*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_5;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*HOLD*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_12;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);		
	
  /*led1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_15;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_15,1);	
  /*led0*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_8;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_8,1);
  /*gsm_rest*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_12;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_12,0);	
	
  /*ring_c*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_0;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_0,0);	
  /*cf3_c*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_1;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_1,0);		
  /*cf1_c*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_13;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_13,0);		
	/*切换*/
  /*E0*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_12;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_12,1);	
  /*E1*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_11;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOA, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOA,GPIO_PIN_11,1);		
	/*S0*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_14;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_14,1);
	/*S1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_15;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_15,1);
  /*S2*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_13;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_13,1);	
  /*S3*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_14;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_14,1);		
}

