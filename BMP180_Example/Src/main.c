#include "stm32f0xx.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_i2c.h"
#include "stm32f03x_ssd1306.h"
#include "stm32f03x_bmp180.h"
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
	//LED OFF
	GPIOB->BSRR |= GPIO_BSRR_BS_1;
}
//*************************************************************************************
void FloatToString(char* buf, float val, uint8_t sign)
{
	char nul = '0';
	int8_t i = 0;
	uint32_t num;
	if(sign)
	{
		int16_t temp = (int16_t)(val * 128.0);
		if(temp < 0)
		{
			buf[i++] = '-';
			val *= -1.0;
		}
		else
		{
			buf[i++] = '+';
		}
	}
	num = (uint32_t)(val * 10.0);
	if(num < 10)
		buf[i++] = nul;
	else
	{
		if(num >= 1000000)
		{
			buf[i++] = (num / 1000000) + nul;
			num %= 1000000;
			if(num < 100000)
				buf[i++] = nul;
		}
		if(num >= 100000)
		{
			buf[i++] = (num / 100000) + nul;
			num %= 100000;
			if(num < 10000)
				buf[i++] = nul;
		}
		if(num >= 10000)
		{
			buf[i++] = (num / 10000) + nul;
			num %= 10000;
			if(num < 1000)
				buf[i++] = nul;
		}
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
	buf[i] = '\0';
}
//*************************************************************************************
int main(void)
{
	//--------------------------
	BMP180_t bmp180dev;
	uint8_t device;
	//--------------------------
	RCC_Init();
	Delay_Init();
	Led_Init();
	//--------------------------
	I2C_Init(GPIOA, GPIO_Pin_9, GPIO_Pin_10);
	//--------------------------
	device = 0;
	while(!device)
	{
		device = SSD1306_Init();
		if(device)
		{
			// LED ON
			GPIOB->BSRR |= GPIO_BSRR_BR_1;
			SSD1306_PrintText(0, 0, "SSD1306 LCD is found!", SSD1306_BLACK_MASK);
		}
		else
		{
			// LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
		}
		DelayMs(500);
	}
	//--------------------------
start:
	GPIOB->BSRR |= GPIO_BSRR_BS_1;
	SSD1306_FillWindow(SSD1306_COLOR_BLACK);
	DelayMs(20);
	device = 0;
	SSD1306_PrintText(0, 0, "Seaching BMP180 sensor...", SSD1306_BLACK_MASK);
	DelayMs(1000);
	while(!device)
	{
		device = BMP180_StartSensor(&bmp180dev);
		/* If any devices on 2-wire */
		if (device)
		{
			// LED ON
			GPIOB->BSRR |= GPIO_BSRR_BR_1;
			SSD1306_PrintText(0, 2, "BMP180 sensor is found!", SSD1306_BLACK_MASK);
		}
		else
		{
			// LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
		}
		DelayMs(250);
	}
	//--------------------------
	uint8_t x;
	char buf[32];
	DelayMs(1000);
	SSD1306_FillWindow(SSD1306_COLOR_BLACK);
	//--------------------------
	while(1)
	{

		if(BMP180_StartTemperature(&bmp180dev))
		{
			DelayUs(bmp180dev.delay);
			if(BMP180_ReadTemperature(&bmp180dev))
			{
				FloatToString(buf, bmp180dev.temperature, 1);
				x = SSD1306_PrintText(0, 0, "Temperature = ", SSD1306_BLACK_MASK);
				x = SSD1306_PrintText(x, 0, buf, SSD1306_BLACK_MASK);
				SSD1306_PrintText(x, 0, "°C ", SSD1306_BLACK_MASK);
				if(BMP180_StartPressure(&bmp180dev, BMP180_MODE_ULTRA_HIGH_RESOLUTION))
				{
					DelayUs(bmp180dev.delay);
					if(BMP180_ReadPressure(&bmp180dev))
					{
						FloatToString(buf, bmp180dev.pressure, 0);
						x = SSD1306_PrintText(0, 1, "Pressure = ", SSD1306_BLACK_MASK);
						x = SSD1306_PrintText(x, 1, buf, SSD1306_BLACK_MASK);
						SSD1306_PrintText(x, 1, "Pa ", SSD1306_BLACK_MASK);
						FloatToString(buf, BMP180_ConvertPressureToMmHg(bmp180dev.pressure), 0);
						x = SSD1306_PrintText(0, 2, "Pressure = ", SSD1306_BLACK_MASK);
						x = SSD1306_PrintText(x, 2, buf, SSD1306_BLACK_MASK);
						SSD1306_PrintText(x, 2, "mmHg ", SSD1306_BLACK_MASK);
						FloatToString(buf, bmp180dev.altitude, 0);
						x = SSD1306_PrintText(0, 3, "Altitude = ", SSD1306_BLACK_MASK);
						x = SSD1306_PrintText(x, 3, buf, SSD1306_BLACK_MASK);
						SSD1306_PrintText(x, 3, "m ", SSD1306_BLACK_MASK);
						DelayMs(2000);
					}
					else
					{
						//SSD1306_PrintText(0, 3, "Pressure read error!", SSD1306_BLACK_MASK);
						//DelayMs(2000);
						goto start;
					}
				}
				else
				{
					//SSD1306_PrintText(0, 3, "Pressure start error!", SSD1306_BLACK_MASK);
					//DelayMs(2000);
					goto start;
				}

			}
			else
			{
				//SSD1306_PrintText(0, 3, "Temperature read error!", SSD1306_BLACK_MASK);
				//DelayMs(2000);
				goto start;
			}
		}
		else
		{
			//SSD1306_PrintText(0, 3, "Temperature start error!", SSD1306_BLACK_MASK);
			//DelayMs(2000);
			goto start;
		}
	}
}
//*************************************************************************************
