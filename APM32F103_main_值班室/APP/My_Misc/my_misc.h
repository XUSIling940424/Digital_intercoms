#ifndef  __MY_MISC_H__
#define  __MY_MISC_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"
#include "Can.h"
#include "Usart.h"

#define  MAX_Time_Out    3
#define  MAX_Rint_Rime   500

extern unsigned int ring_time;


void start_voice_play(void);
void stop_voice_play(void);
void init_data(void);

#endif

