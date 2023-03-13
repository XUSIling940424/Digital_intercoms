#ifndef  __MY_TIME_H__
#define  __MY_TIME_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32e10x.h"
#include "apm32e10x_tmr.h"
#include "Time.h"

void start_time(void);
void stop_time(void);

#endif