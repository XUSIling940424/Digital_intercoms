#ifndef  __MY_MISC_H__
#define  __MY_MISC_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "apm32e10x_gpio.h"
#include "Can.h"
#include "Usart.h"

#define  MAX_Time_Out    3
#define  MAX_Rint_Rime   100

extern unsigned int ring_time;

void start_voice_play(void);
void stop_voice_play(void);
void init_data(void);
void  open_call(void);
void  stop_call(void);
void open_rest(void);
void stop_rest(void);

#endif

