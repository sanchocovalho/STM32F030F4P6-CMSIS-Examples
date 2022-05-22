#include "stm32f0xx.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_usart.h"
#include "stm32f03x_onewire.h"
#include "stm32f03x_ds18b20.h"
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
void FloatTempToStrTemp(char* buf, float* temperature)
{
	char nul = '0';
	int8_t i = 0;
	int16_t temp = (int16_t)(*temperature * 16.0);
	if(temp < 0)
	{
		buf[i++] = '-';
		*temperature = ((float)temp) * (-0.0625);
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
	/* Define onewire structure */
	OneWire_t onewire;
	/* One wire on A4 port */
	OneWire_Init(&onewire, GPIOA, GPIO_Pin_4);
	//--------------------------
	/* Wait until USART port is ready */
	uint8_t msg = 0;
	while(!(USART1->ISR & USART_ISR_RXNE))
	{
		if(!msg)
		{
			USART_SendString("Send any character on serial port!\r\n");
			msg = 1;
		}
	}
	//--------------------------
	char buf[64];
	uint8_t addr[8];
	float temperature;
	uint8_t device = 0;
	//--------------------------
	USART_SendString("Seaching DS18B20 temperature sensor...\r\n");
	while(!device)
	{
		OneWire_ResetSearch(&onewire);
		device = OneWire_Search(&onewire);
		/* If any devices on onewire */
		if (device)
		{
			OneWire_GetFullROM(&onewire, addr);
			USART_SendString("Device found on 1-wire: ");
			/* Display 64bit rom code for each device */
			USART_SendBytesAsHex(addr, 8, ' ');
			USART_SendString("\r\n");
			if (Is_DS18B20(addr))
			{
				USART_SendString("It's DS18B20 temperature sensor!\r\n");
			}
			else
			{
				USART_SendString("It's not DS18B20 temperature sensor!\r\n");
				device = 0;
				continue;
			}
		}
		else
		{
			USART_SendString("No device on OneWire!\r\n");
			DelayMs(2000);
			//LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
		}
	}
	/* Set resolution to 12bits */
	DS18B20_SetResolution(&onewire, addr, DS18B20_Resolution_12bits);
	while(1)
	{
		/* Start temperature conversion on device on bus */
		if (DS18B20_Start(&onewire, addr))
		{
			/* Wait 800ms until all are done on onewire port */
			DelayMs(800);
			/* Read temperature from ROM address and store it to temperature variable */
			if (DS18B20_Read(&onewire, addr, &temperature))
			{
				/* Print temperature */
				FloatTempToStrTemp(buf, &temperature);
				USART_SendString("temp = ");
				USART_SendString(buf);
				USART_SendString("\r\n");
			}
			else
			{
				/* Reading error */
				USART_SendString("Reading Error!\r\n");
				DelayMs(1200);
				//LED TOGGLE
				GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
			}
		}
	}
}
//*************************************************************************************
