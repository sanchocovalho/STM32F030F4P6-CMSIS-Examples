#include "stm32f0xx.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_delay.h"
//*************************************************************************************
// LED initialization
void LED_Init(void)
{
	//Enable PORTC bus
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	//PORTB1
	//MODER1[1:0] = 01 - General purpose output mode
	GPIOB->MODER &= ~GPIO_MODER_MODER1;
	GPIOB->MODER |= GPIO_MODER_MODER1_0;
	//OTYPER1 = 0 - Output push-pull
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_1;
	//OSPEEDR1[1:0] = 00 - Low speed
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR1;
	//PUPDR1[1:0] = 00 - No pull-up, pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR1;
	//LED ON
	GPIOB->BSRR |= GPIO_BSRR_BR_1;
}
//*************************************************************************************
int main(void)
{
	RCC_Init();
	Delay_Init();
	LED_Init();
	while(1)
	{
		// LED OFF
		GPIOB->BSRR |= GPIO_BSRR_BS_1;
		// Delay in 500 ms
		DelayMs(500);
		// LED ON
		GPIOB->BSRR |= GPIO_BSRR_BR_1;
		// Delay in 500 ms
		DelayMs(500);
	}
}
//*************************************************************************************
