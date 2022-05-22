#include "stm32f03x_delay.h"
//*************************************************************************************
void Delay_Init(void)
{
	// TIM1 enable
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	// TIM1 freq = 2 MHz
	TIM1->PSC = (F_CPU/2000000UL)-1;
	// Period = 65535
	TIM1->ARR = 0xFFFF;
	// Counter is cleared
	TIM1->CNT = 0;
	// External clock disable
	TIM1->SMCR &= ~TIM_SMCR_ECE;
	// Clock division is DIV1, Direction is up, Auto-reload preload is disable
	TIM1->CR1 |= TIM_CR1_CEN;
	// Wait TIM1 update flag
	while (!(TIM1->SR & TIM_SR_UIF));
}
//*************************************************************************************
void Delay500ns(uint32_t time_500ns)
{
	TIM1->CNT = 0;
	while(TIM1->CNT <= time_500ns);
}
//*************************************************************************************
void DelayUs(uint32_t time_us)
{
	TIM1->CNT = 0;
	while(TIM1->CNT <= (time_us << 1));
}
//*************************************************************************************
void DelayMs(uint32_t time_ms)
{
	while(time_ms)
	{
		time_ms--;
	    DelayUs(1000);
	}
}
//*************************************************************************************
