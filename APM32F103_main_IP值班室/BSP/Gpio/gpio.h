#ifndef  __GPIO_H__
#define  __GPIO_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"
#include "w5500_conf.h"

#define HOLD   GPIO_ReadInputBit(GPIOB,GPIO_PIN_12)

#define X0     GPIO_ReadInputBit(GPIOB,GPIO_PIN_9)
#define X1     GPIO_ReadInputBit(GPIOB,GPIO_PIN_8)
#define X2     GPIO_ReadInputBit(GPIOB,GPIO_PIN_7)
#define X3     GPIO_ReadInputBit(GPIOB,GPIO_PIN_6)
#define X4     GPIO_ReadInputBit(GPIOB,GPIO_PIN_5)

#define R_X    GPIO_ReadInputPort(GPIOB)

#define  wiz_LED0(val)     		(val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_8):GPIO_SetBit(GPIOA, GPIO_PIN_8)
#define  wiz_LED1(val)     		(val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_15):GPIO_SetBit(GPIOB, GPIO_PIN_15)

#define  wiz_CF1_C(val)    		(val==LOW)?GPIO_ResetBit(GPIOC, GPIO_PIN_13):GPIO_SetBit(GPIOC, GPIO_PIN_13)
#define  wiz_RING_C(val)   		(val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_0):GPIO_SetBit(GPIOA, GPIO_PIN_0)
#define  wiz_CF3_C(val)    		(val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_1):GPIO_SetBit(GPIOA, GPIO_PIN_1)
	
#define  wiz_NET_REST(val)     (val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_12):GPIO_SetBit(GPIOA, GPIO_PIN_12)

#define  wiz_E0(val)     (val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_12):GPIO_SetBit(GPIOA, GPIO_PIN_12)
#define  wiz_E1(val)     (val==LOW)?GPIO_ResetBit(GPIOA, GPIO_PIN_11):GPIO_SetBit(GPIOA, GPIO_PIN_11)
#define  wiz_S0(val)     (val==LOW)?GPIO_ResetBit(GPIOC, GPIO_PIN_14):GPIO_SetBit(GPIOC, GPIO_PIN_14)
#define  wiz_S1(val)     (val==LOW)?GPIO_ResetBit(GPIOC, GPIO_PIN_15):GPIO_SetBit(GPIOC, GPIO_PIN_15)
#define  wiz_S2(val)     (val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_13):GPIO_SetBit(GPIOB, GPIO_PIN_13)
#define  wiz_S3(val)     (val==LOW)?GPIO_ResetBit(GPIOB, GPIO_PIN_14):GPIO_SetBit(GPIOB, GPIO_PIN_14)

void gpio_init(void);

#endif

