#ifndef  __TIME_H__
#define  __TIME_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_tmr.h"
#include "Usart.h"


void time1_init(uint16_t div, uint16_t per);
void time2_init(uint16_t div, uint16_t per);

#endif