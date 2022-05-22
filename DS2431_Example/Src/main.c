#include <string.h>
#include "stm32f0xx.h"
#include "stm32f03x_rcc.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_usart.h"
#include "stm32f03x_onewire.h"
#include "stm32f03x_ds2431.h"
//*************************************************************************************
uint8_t rdata[128];
uint8_t wdata[128] = {
		0xA8,0x24,0x36,0xF7,0xAB,0xDB,0x1A,0x2C,
		0x7C,0xCC,0x14,0x00,0x86,0x80,0x30,0x00,
		0xD4,0x7E,0x05,0x38,0x0D,0x12,0x6B,0x0C,
		0xBB,0x6A,0x8E,0x01,0x60,0xE3,0x16,0x33,
		0x6B,0xDD,0x5E,0xDC,0xDD,0x34,0x97,0xBB,
		0x74,0x02,0xD0,0xE3,0x03,0xC6,0x89,0x6C,
		0x61,0xCA,0xD3,0xFC,0x6E,0x95,0x67,0x96,
		0x45,0xBA,0xFE,0x1F,0x83,0x99,0x25,0xF0,
		0x04,0x20,0x6F,0xEF,0x69,0xDB,0x1A,0x00,
		0x45,0x4C,0x16,0x92,0x56,0x87,0x37,0xF3,
		0xD4,0x7E,0x05,0x07,0x0D,0x12,0x6B,0x0C,
		0xBB,0x6A,0x8E,0x01,0x60,0xE3,0x16,0x00,
		0x7C,0xDD,0x5E,0xDC,0xDD,0x34,0x97,0xBB,
		0x77,0x24,0xD5,0xE7,0x13,0xC0,0x81,0x66,
		0x69,0xCF,0xD1,0xF9,0xAE,0x9B,0x56,0x93,
		0x48,0xD3,0xCD,0x84,0x26,0x79,0x1A,0x19};
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
int main(void)
{
	RCC_Init();
	Delay_Init();
	Led_Init();
	USART_Init(9600);
	//--------------------------
	/* Define onewire structure */
	OneWire_t ds2431;
	/* Onewire on A4 port */
	OneWire_Init(&ds2431, GPIOA, GPIO_Pin_4);
	//-------------------------
	USART_SendString("Seaching DS2431 EEPROM device...\r\n");
	//-------------------------
	char buf[64];
	uint8_t rom[8];
	uint8_t device = 0;
	//-------------------------
	while(!device)
	{
		OneWire_ResetSearch(&ds2431);
		device = OneWire_Search(&ds2431);
		/* If any devices on onewire */
		if (device)
		{
			/* Device is found */
			OneWire_GetFullROM(&ds2431, rom);
			USART_SendString("Device found on 1-wire: ");
			/* Display 64bit rom code for each device */
			USART_SendBytesAsHex(rom, 8, ' ');
			USART_SendString("\r\n");
			if (Is_DS2431(rom))
			{
				/* It's DS2431 */
				USART_SendString("It's DS2431 EEPROM device!\r\n");
			}
			else
			{
				/* It's another device */
				USART_SendString("It's not DS2431 EEPROM device!\r\n");
				device = 0;
				DelayMs(2000);
				continue;
			}
		}
		else
		{
			/* Device is not found */
			USART_SendString("No device on OneWire!\r\n");
			DelayMs(2000);
			//LED TOGGLE
			GPIO_TogglePinValue(GPIOB, GPIO_Pin_1);
		}
	}
	//------------------------
	uint8_t row;
	uint16_t addr = 0;
	uint16_t buflen;
	while(1)
	{
		buflen = USART_GetString(buf, sizeof(buf));
		if(buflen == 3)
		{
			if(!memcmp(buf,"-rb", 3))
			{
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					/* Read one byte by 0x00 address */
					if(DS2431_Read(&ds2431, rom, addr, rdata, 1))
					{
						/* Print the read byte */
						USART_SendString("Read byte: ");
						USART_SendBytesAsHex(rdata, 1, 0);
						USART_SendString("\r\n");
					}
					else
					{
						USART_SendString("Reading error!\r\n");
					}
				}
			}
			else if (!memcmp(buf,"-rm", 3))
			{
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					/* Read all bytes */
					if(DS2431_Read(&ds2431, rom, addr, rdata, DS2431_EEPROM_SIZE))
					{
						/* Print the read bytes */
						USART_SendString("Read bytes:\r\n");
						for (uint8_t i = 0; i < 16; i++)
						{
							row = i * DS2431_ROW_SIZE;
							USART_SendBytesAsHex((uint8_t*)&row, 1, 0);
							USART_SendString(" : ");
							USART_SendBytesAsHex(rdata + row, DS2431_ROW_SIZE, ',');
							USART_SendString("\r\n");
						}
					}
					else
					{
						USART_SendString("Reading error!\r\n");
					}
				}
			}
			else if(!memcmp(buf,"-wb", 3))
			{
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					for (uint8_t i = 0; i < DS2431_EEPROM_SIZE; i++)
					{
						/* Write to device per one generated byte */
						rdata[i] = i + 1;
						row = DS2431_Write(&ds2431, rom, addr + i, rdata + i, 1);
						if(row)
						{
							if(i % 8 == 7)
							{
								USART_SendString("##\r\n");
							}
							else
							{
								USART_SendString("##");
							}
						}
						else
						{
							USART_SendString("\r\n");
							USART_SendString("Write status: Error.\r\n");
							USART_SendString("Error #: ");
							USART_SendBytesAsHex((uint8_t*)&row, 1, 0);
							USART_SendString("\r\n");
							break;
						}
					}
					if(row)
					{
						USART_SendString("Write status: Success.\r\n");
					}
				}
			}
			else if(!memcmp(buf,"-wm", 3))
			{
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					for(uint8_t i = 0 ; i < 16; i++)
					{
						/* Write 128 bytes array from RAM per 8 bytes by aligned address */
						row = DS2431_WriteAlignedRow(&ds2431, rom, addr + i * DS2431_ROW_SIZE,
								wdata + i * DS2431_ROW_SIZE);
						if(row)
						{
							if(i % 16 == 15)
							{
								USART_SendString("##\r\n");
								USART_SendString("Write status: Success.\r\n");
							}
							else
							{
								USART_SendString("##");
							}
						}
						else
						{
							USART_SendString("\r\n");
							USART_SendString("Write status: Error.\r\n");
							break;
						}
					}

				}
			}
			else if(!memcmp(buf,"-ww", 3))
			{
				for(uint8_t i = 0 ; i < DS2431_EEPROM_SIZE; i++)
				{
					rdata[i] = i + 1;
				}
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					/* Write 128 bytes array from RAM */
					row = DS2431_Write(&ds2431, rom, addr, rdata, DS2431_EEPROM_SIZE);
					if(row)
					{
						USART_SendString("Write status: Success.\r\n");
					}
					else
					{
						USART_SendString("Write status: Error.\r\n");
					}
				}
			}
			else if(!memcmp(buf,"-em", 3))
			{
				/* Start communication with DS2431 */
				if (DS2431_Start(&ds2431, rom))
				{
					/* Erase full memory on device */
					DS2431_EraseFullMemory(&ds2431, rom);
					USART_SendString("Erase status: Success.\r\n");
				}
			}
		}
	}
}
//*************************************************************************************
