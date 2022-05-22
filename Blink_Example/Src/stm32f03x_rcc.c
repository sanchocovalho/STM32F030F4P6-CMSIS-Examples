#include "stm32f03x_rcc.h"
//*************************************************************************************
// RCC initialization
void RCC_Init(void)
{
	/* Enable Prefetch Buffer */
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	/* Flash 1 wait state */
	FLASH->ACR |= FLASH_ACR_LATENCY;
#ifdef INTERNAL_OSCILLATOR
	/* Enable HSI */
	RCC->CR |= RCC_CR_HSION;
	/* Wait for HSI to be ready */
	while(!(RCC->CR & RCC_CR_HSIRDY));
	/* Peripheral Clock divisors */
	RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV1;
	/* PLLCLK */
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;
	RCC->CFGR &= ~RCC_CFGR_PLLMUL;
	RCC->CFGR |= RCC_CFGR_PLLMUL12;
#else
	/* Enable HSE */
	RCC->CR |= RCC_CR_HSEON;
	/* Wait for HSE to be ready */
	while(!(RCC->CR & RCC_CR_HSERDY));
	/* Peripheral Clock divisors */
	RCC->CFGR2 |= RCC_CFGR2_PREDIV_DIV1;
	/* PLLCLK */
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC);
	RCC->CFGR &= RCC_CFGR_PLLMUL;
	RCC->CFGR |= (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL6);
#endif
	/* Enable PLL */
	RCC->CR |= RCC_CR_PLLON;
	/* Wait until the PLL is ready */
	while(!(RCC->CR & RCC_CR_PLLRDY));
	/* Select PLL as system Clock */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	/* Wait for PLL to become system core clock */
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
//*************************************************************************************
