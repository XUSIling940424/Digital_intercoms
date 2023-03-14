#include "Time.h"

/*
��ʱ��1��ʼ������
div:��Ƶϵ��
per:����ֵ
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


/*��ʱ���жϺ���*/
void TMR1_UP_IRQHandler(void)
{
    if(TMR_ReadIntFlag(TMR1, TMR_INT_UPDATE) == SET)
    {
			if(Rx_USART2_Flags==1){
				stop_time();
				rt_sem_release(my_usart_sem);  /*�ͷ��ź���*/
			}
      TMR_ClearIntFlag(TMR1, TMR_INT_UPDATE);
    }
}

