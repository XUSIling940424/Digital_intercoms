#include "my_time.h"

void start_time(void)
{
 TMR1->CNT=0;
 TMR_Enable(TMR1);
}

void stop_time(void)
{
	TMR_Disable(TMR1);
}

