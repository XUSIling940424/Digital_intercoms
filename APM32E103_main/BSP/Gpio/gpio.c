#include "gpio.h"


void gpio_init(void)
{
	GPIO_Config_T GPIO_ConfigStruct;
	
	RCM_EnableAPB2PeriphClock((RCM_APB2_PERIPH_T)RCM_APB2_PERIPH_GPIOB | RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_GPIOC);
	/*x1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_14;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);
	/*x2*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_15;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*x3*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_0;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	/*k1*/
	GPIO_ConfigStruct.mode = GPIO_MODE_IN_PU;  /*上拉输入*/
	GPIO_ConfigStruct.pin = GPIO_PIN_11;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);		
	/*mute*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_10;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_10,0);
  /*ring*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_9;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_9,1);		
  /*c_ring*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_0;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_0,1);
/*指示灯*/	
	/*call_led*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_6;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_6,1);		
  /*ALM*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_7;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,1);		
  /*rest*/
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_8;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOB, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOB,GPIO_PIN_8,1);
  /*under_vol*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_1;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_1,1);		
  /*hitch*/	
	GPIO_ConfigStruct.mode = GPIO_MODE_OUT_PP;  /*推挽输出*/
	GPIO_ConfigStruct.pin = GPIO_PIN_2;
	GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIOC, &GPIO_ConfigStruct);	
	GPIO_WriteBitValue(GPIOC,GPIO_PIN_2,1);	
}


