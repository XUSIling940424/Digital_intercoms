#ifndef  __GPIO_H__
#define  __GPIO_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"

#define X1     GPIO_ReadInputBit(GPIOB,GPIO_PIN_9)
#define X2     GPIO_ReadInputBit(GPIOB,GPIO_PIN_8)
#define K1     GPIO_ReadInputBit(GPIOB,GPIO_PIN_0)

#define  r_call   GPIO_WriteBitValue(GPIOB,GPIO_PIN_7,~(GPIO_ReadOutputBit(GPIOB,GPIO_PIN_7)))

void gpio_init(void);

#endif

