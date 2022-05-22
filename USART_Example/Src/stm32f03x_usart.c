#include "stm32f03x_usart.h"
//*************************************************************************************
/* Set variables for buffers */
uint8_t USART_Buffer[USART_BUFFER_SIZE];
USART_t usart1 = {0, 0, 0, USART_BUFFER_SIZE, USART_Buffer, 0};
//*************************************************************************************
/* Private function */
static inline USART_t* USART_INT_GetUsart(USART_TypeDef* USARTx)
{
	return &usart1;
}
//*************************************************************************************
void USART_Init(uint32_t baudrate)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* We are not initialized */
	u->Initialized = 0;
	/* Enable GPIOA clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	/* Enable USART clock */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	/* PA9 (TX) & PA10 (RX) - Alternate function mode */
	GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
	GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL9_Pos) | (1 << GPIO_AFRH_AFSEL10_Pos);
	/* Set USART baudrate */
	USART1->BRR = (F_CPU+baudrate/2)/baudrate;
	/* Set 8 bit and 1 stop bit */
	USART1->CR1 &= ~(USART_CR1_M | USART_CR2_STOP);
	/* Enable transmitter and receiver USART1 */
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	/* Enable RX interrupt */
	USART1->CR1 |= USART_CR1_RXNEIE;
	/* Set IRQ priority */
	NVIC_SetPriority(USART1_IRQn, USART_NVIC_PRIORITY);
	/* Enable RX interrupt */
	NVIC_EnableIRQ(USART1_IRQn);
	/* We are initialized now */
	u->Initialized = 1;
	/* Enable USART peripheral */
	USART1->CR1 |= USART_CR1_UE;
}
//*************************************************************************************
uint8_t USART_GetChar(void)
{
	int8_t c = 0;
	USART_t* u = USART_INT_GetUsart(USART1);
	/* Check if we have any data in buffer */
	if (u->Num > 0 || u->In != u->Out)
	{
		/* Check overflow */
		if (u->Out == u->Size)
		{
			u->Out = 0;
		}
		/* Read character */
		c = u->Buffer[u->Out];
		/* Increase output pointer */
		u->Out++;
		/* Decrease number of elements */
		if (u->Num)
		{
			u->Num--;
		}
	}
	/* Return character */
	return c;
}
//*************************************************************************************
uint16_t USART_GetString(char* buffer, uint16_t bufsize)
{
	/* Get USART structure */
	USART_t* u = USART_INT_GetUsart(USART1);
    /* Check for any data on USART */
	if ((u->Num == 0) ||             /*!< Buffer empty */
	((!USART_FindCharacter('\n')) && /*!< String delimiter not in buffer */
	(u->Num != u->Size)))            /*!< Buffer is not full */
	{
		/* Return 0 */
		return 0;
	}
	uint16_t i = 0;
	//uint16_t num = 0;
	/* If available buffer size is more than 0 characters */
	while (i < (bufsize - 1))
	{
    	/* We have available data */
    	char symb = (char)USART_GetChar();
    	buffer[i++] = symb;
		if (symb == '\n')
		{
			break;
		}
	}
	/* Add zero to the end of string */
	buffer[i] = 0;
	/* Return number of characters in buffer */
	return i;
}
//*************************************************************************************
uint8_t USART_BufferEmpty(void)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* Check if number of characters is zero in buffer */
	return (u->Num == 0 && u->In == u->Out);
}
//*************************************************************************************
uint8_t USART_BufferFull(void)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* Check if number of characters is the same as buffer size */
	return (u->Num == u->Size);
}
//*************************************************************************************
void USART_ClearBuffer(void)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* Reset variables */
	u->Num = 0;
	u->In = 0;
	u->Out = 0;
}
//*************************************************************************************
uint8_t USART_FindCharacter(uint8_t c)
{
	uint16_t num, out;
	USART_t* u = USART_INT_GetUsart(USART1);
	/* Temp variables */
	num = u->Num;
	out = u->Out;
	while (num > 0)
	{
		/* Check overflow */
		if (out == u->Size)
		{
			out = 0;
		}
		/* Check if characters matches */
		if ((uint8_t) u->Buffer[out] == (uint8_t) c)
		{
			/* Character found */
			return 1;
		}
		/* Set new variables */
		out++;
		num--;
	}
	/* Character is not in buffer */
	return 0;
}
//*************************************************************************************
void USART_SendChar(char c)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* If we are not initialized */
	if (u->Initialized == 0)
	{
		return;
	}
	/* Check USART if enabled */
	if ((USART1->CR1 & USART_CR1_UE))
	{
		/* Wait to be ready, buffer empty */
		while (!(USART1->ISR & USART_ISR_TC));
		/* Send data */
    	USART1->TDR = (uint16_t)(c & 0x01FF);
	}
}
//*************************************************************************************
void USART_SendString(char* str)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* If we are not initialized */
	if (u->Initialized == 0)
	{
		return;
	}
	/* Go through entire string */
  	while (*str)
  	{
    	/* Wait to be ready, buffer empty */
    	while (!(USART1->ISR & USART_ISR_TC));
    	/* Send data */
    	USART1->TDR = (uint16_t)(*str++ & 0x01FF);
	}
}
//*************************************************************************************
void USART_SendBytes(uint8_t* DataArray, uint16_t count)
{
	USART_t* u = USART_INT_GetUsart(USART1);
	/* If we are not initialized */
	if (u->Initialized == 0)
	{
		return;
	}
	/* Go through entire data array */
  	for (uint16_t i = 0; i < count; i++)
  	{
		/* Wait to be ready, buffer empty */
		while (!(USART1->ISR & USART_ISR_TC));
		/* Send data */
		USART1->TDR = (uint16_t)(DataArray[i]);
	}
}
//*************************************************************************************
void USART_SendBytesAsHex(uint8_t* DataArray, uint16_t count, char separator)
{
	char str[2];
	USART_t* u = USART_INT_GetUsart(USART1);
	/* If we are not initialized */
	if (u->Initialized == 0)
	{
		return;
	}
	/* Go through entire data array */
	for (uint16_t i = 0; i < count; i++)
	{
		str[0] = (DataArray[i] >> 4) + 0x30;
		if (str[0] > 0x39)
			str[0] += 7;
		str[1] = (DataArray[i] & 0x0F) + 0x30;
		if (str[1] > 0x39)
			str[1] += 7;
		for (uint16_t j = 0; j < 2; j++)
		{
			/* Wait to be ready, buffer empty */
			while (!(USART1->ISR & USART_ISR_TC));
			/* Send data */
			USART1->TDR = (uint16_t)(str[j] & 0x01FF);
		}
		if(separator && (i < count - 1))
		{
			/* Wait to be ready, buffer empty */
			while (!(USART1->ISR & USART_ISR_TC));
			/* Send data */
			USART1->TDR = (uint16_t)(separator);
		}
	}
}
//*************************************************************************************
void USART1_IRQHandler(void)
{
	/* Check if interrupt was because data is received */
	if (USART1->ISR & USART_ISR_RXNE)
	{
		/* Put received data into internal buffer */
		USART_t* u = USART_INT_GetUsart(USART1);
		if (u->Num < u->Size)
		{
			/* Check overflow */
			if (u->In == u->Size)
			{
				u->In = 0;
			}
			/* Add to buffer */
			u->Buffer[u->In] = USART1->RDR;
			u->In++;
			u->Num++;
		}
	}
}
//*************************************************************************************
