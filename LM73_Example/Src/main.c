#include "stm32f0xx.h"
#include "stm32f03x_defs.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_usart.h"
#include "stm32f03x_twi.h"
#include "stm32f03x_lm73.h"
//*************************************************************************************
// LED initialization
void Led_Init(void)
{
	//Enable PORTB bus
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
void DoubleTempToStrTemp(char* buf, double* temperature)
{
	char nul = '0';
	int8_t i = 0;
	int16_t temp = (int16_t)(*temperature * 128.0);
	if(temp < 0)
	{
		buf[i++] = '-';
		*temperature = ((double)temp) * (-0.0078125);
	}
	else
	{
		buf[i++] = '+';
	}
	uint16_t num = (uint16_t)(*temperature * 10.0);
	if(num < 10)
		buf[i++] = nul;
	else
	{
		if(num >= 1000)
		{
			buf[i++] = (num / 1000) + nul;
			num %= 1000;
			if(num < 100)
				buf[i++] = nul;
		}
		if(num >= 100)
		{
			buf[i++] = (num / 100) + nul;
				num %= 100;
			if(num < 10)
				buf[i++] = nul;
		}
		if(num>=10)
		{
			buf[i++] = (num / 10) + nul;
			num %= 10;
		}
	}
	buf[i++] = '.';
	buf[i++] = num + nul;
	buf[i++] = 0xB0;
	buf[i++] = 'C';
	buf[i] = '\0';
}
//*************************************************************************************
int main(void)
{
	RCC_Init();
	Delay_Init();
	Led_Init();
	USART_Init(9600);
	//--------------------------
	/* SCL on A0 port, SDA on A1 port */
	TwoWire_Init(GPIOA, GPIO_Pin_0, GPIOA, GPIO_Pin_1);
	//--------------------------
	char buf[12];
	uint8_t id[2];
	LM73_twiaddr_t twiaddr;
	double temperature;
	uint8_t device;
	//--------------------------
start:
	device = 0;
	USART_SendString("Seaching LM73 temperature sensor...\r\n");
	while(!device)
	{
		device = LM73_Start(LM73_RESOLUTION_12BIT);
		/* If any devices on 2-wire */
		if (device)
		{
			/* Device is found */
			twiaddr = LM73_GetAddress();
			/* Print device I2C address*/
			USART_SendString("LM73 sensor is found with address: ");
			USART_SendBytesAsHex(&twiaddr, sizeof(twiaddr), 0);
			USART_SendString("\r\n");
			if(LM73_GetIdCode(id))
			{
				/* Print device ID code*/
				USART_SendString("LM73 sensor has ID code: ");
				USART_SendBytesAsHex(id, sizeof(id), ' ');
				USART_SendString("\r\n");
			}
		}
		else
		{
			/* Device is not found */
			USART_SendString("LM73 sensor is not found!\r\n");
			DelayMs(2000);
			//LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
		}
	}
	//--------------------------
	while(1)
	{
		/* Get temperature */
		if(LM73_GetTemperature(&temperature, LM73_REG_TEMPERATURE))
		{
			/* Print temperature */
			DoubleTempToStrTemp(buf, &temperature);
			USART_SendString("temp = ");
			USART_SendString(buf);
			USART_SendString("\r\n");
			DelayMs(800);
		}
		else
		{
			/* Reading error */
			USART_SendString("Reading Error!\r\n");
			DelayMs(800);
			//LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
			goto start;
		}
	}
}
//*************************************************************************************
