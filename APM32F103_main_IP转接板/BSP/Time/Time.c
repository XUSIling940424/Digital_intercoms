#include "Time.h"

/*
定时器1初始化函数
div:分频系数
per:重载值
*/
void time1_init(uint16_t div, uint16_t per)
{
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = div;
    TMR_BaseConfigStruct.period = per;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_UP_IRQn, 0, 1);

}

/*
定时器2初始化函数
div:分频系数
per:重载值
*/
void time2_init(uint16_t div, uint16_t per)
{
    TMR_BaseConfig_T TMR_BaseConfigStruct;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);

    TMR_BaseConfigStruct.clockDivision = TMR_CLOCK_DIV_1;
    TMR_BaseConfigStruct.countMode = TMR_COUNTER_MODE_UP;
    TMR_BaseConfigStruct.division = div;
    TMR_BaseConfigStruct.period = per;
    TMR_BaseConfigStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR2, &TMR_BaseConfigStruct);

    TMR_EnableInterrupt(TMR2, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR2_IRQn, 0, 1);

}


/*定时器中断函数*/
void TMR1_UP_IRQHandler(void)
{
    if(TMR_ReadIntFlag(TMR1, TMR_INT_UPDATE) == SET)
    {
			if(Rx_USART2_Flags==1){
				stop_time();
//				rt_sem_release(my_usart_sem);  /*释放信号量*/
			}
      TMR_ClearIntFlag(TMR1, TMR_INT_UPDATE);
    }
}

