#ifndef  __GPIO_H__
#define  __GPIO_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "apm32e10x_gpio.h"

#define ON	                 		1
#define OFF	                 		0
#define HIGH	           	 			1
#define LOW		             			0

#define X1     GPIO_ReadInputBit(GPIOB,GPIO_PIN_14)
#define X2     GPIO_ReadInputBit(GPIOB,GPIO_PIN_15)
#define X3     GPIO_ReadInputBit(GPIOB,GPIO_PIN_0)
#define K1     GPIO_ReadInputBit(GPIOB,GPIO_PIN_11)


#define  r_call   GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,~(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)))

#define  wiz_CALL_C(val)    		(val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_6):GPIO_SetBit(GPIOB, GPIO_PIN_6)
#define  wiz_ALM_C(val)     		(val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_7):GPIO_SetBit(GPIOB, GPIO_PIN_7)
#define  wiz_REST_C(val)    		(val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_8):GPIO_SetBit(GPIOB, GPIO_PIN_8)
#define  wiz_UNDR_V(val)     		(val==LOW)?GPIO_ResetBit(GPIOC, GPIO_PIN_1):GPIO_SetBit(GPIOC, GPIO_PIN_1)
#define  wiz_HITCH(val)     		(val==LOW)?GPIO_ResetBit(GPIOC, GPIO_PIN_2):GPIO_SetBit(GPIOC, GPIO_PIN_2)

void gpio_init(void);

#endif

