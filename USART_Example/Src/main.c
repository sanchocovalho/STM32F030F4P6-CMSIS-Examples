#include "stm32f0xx.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_usart.h"
//*************************************************************************************
int main(void)
{
	char buffer[64];
	RCC_Init();
	USART_Init(9600);
	while(1)
	{
		/* Get string */
		if (USART_GetString(buffer, sizeof(buffer)))
		{
			/* Return string back */
			USART_SendString(buffer);
		}
	}
}
//*************************************************************************************
