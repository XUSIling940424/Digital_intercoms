#ifndef  __MY_MISC_H__
#define  __MY_MISC_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_gpio.h"
#include "Can.h"
#include "Usart.h"

#define  MAX_Time_Out    3

void start_voice_play(void);
void stop_voice_play(void);
void init_data(void);
u8  Read_Addr_Msg(void);

#endif

