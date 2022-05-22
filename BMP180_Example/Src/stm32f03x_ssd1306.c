#include "stm32f03x_ssd1306.h"
//-------------------------------------------------------------------------
static inline void SSD1306_WriteCMD(uint8_t* cmd, uint8_t size)
{
	I2C_Write(SSD1306_I2C_ADDR, SSD1306_CONTROL_CMD, cmd, size);
}
//-------------------------------------------------------------------------
static inline void SSD1306_WriteData(uint8_t* data, uint8_t size)
{
	I2C_Write(SSD1306_I2C_ADDR, SSD1306_CONTROL_DATA, data, size);
}
//-------------------------------------------------------------------------
uint8_t SSD1306_Init(void)
{
	/* Check if LCD connected to I2C */
	if (!I2C_IsDeviceConnected(SSD1306_I2C_ADDR))
	{
		/* Return false */
		return 0;
	}
	/* A little delay */
	DelayMs(100);
	uint8_t data[3];
	// Set display off
	data[0] = SSD1306_CMD_DISPLAY_OFF;
	SSD1306_WriteCMD(data, 1);
	// Set oscillator frequency
	data[0] = SSD1306_CMD_RATIO_FREQUENCY;
	data[1] = 0x80;
	SSD1306_WriteCMD(data, 2);
	// Enable charge pump regulator
	data[0] = SSD1306_CMD_SET_CHARGE_PUMP;
	data[1] = SSD1306_CMD_CHARGE_PUMP_ENABLE;
	SSD1306_WriteCMD(data, 2);
	// Set display start line
	data[0] = SSD1306_CMD_SET_START_LINE;
	SSD1306_WriteCMD(data, 1);
	// Set segment remap
#ifdef SSD1306_MIRROR_HORIZ
    data[0] = SSD1306_CMD_SEG_DIR_INVERT; //mirror horizontally
#else
    data[0] = SSD1306_CMD_SEG_DIR_NORMAL; //set segment re-map 0 to 127
#endif
	SSD1306_WriteCMD(data, 1);
	// Set COM output scan direction
#ifdef SSD1306_MIRROR_VERT
    data[0] = SSD1306_CMD_COM_DIR_INVERT; //mirror vertically
#else
    data[0] = SSD1306_CMD_COM_DIR_NORMAL; //set COM Output Scan Direction
#endif
	SSD1306_WriteCMD(data, 1);
	// Set COM pins hardware configuration
	data[0] = SSD1306_CMD_COM_CONFIG;
#if (SSD1306_PIXEL_Y == 32)
    data[1] = 0x02;
#else
    data[1] = 0x12;
#endif
	SSD1306_WriteCMD(data, 2);
	// Set MUX ratio
    data[0] = SSD1306_CMD_SET_MULTIPLEX_RATIO; //set multiplex ratio (1 to 64)
#if (SSD1306_PIXEL_Y == 16)
    data[1] = 0x0F;
#elif (SSD1306_PIXEL_Y == 32)
    data[1] = 0x1F;
#elif (SSD1306_PIXEL_Y == 64)
    data[1] = 0x3F;
#else
    data[1] = 0x7F;
#endif
	SSD1306_WriteCMD(data, 2);
	// Set display offset
	data[0] = SSD1306_CMD_DISPLAY_OFFSET;
	data[1] = 0x00;
	SSD1306_WriteCMD(data, 2);
	// Set horizontal addressing mode
	data[0] = SSD1306_CMD_SET_ADDRESS_MODE;
	data[1] = 0x00;
	SSD1306_WriteCMD(data, 2);
	// Set column address
	data[0] = SSD1306_CMD_SET_COLUMN_ADDRESS;
	data[1] = 0x00; // from 0
	data[2] = 0x7F; // to 127
	SSD1306_WriteCMD(data, 3);
	// Set page address
	data[0] = SSD1306_CMD_SET_PAGE_ADDRESS;
	data[1] = 0x00; // from 0
#if (SSD1306_PIXEL_Y == 16)
	data[2] = 0x01; // to 1
#elif (SSD1306_PIXEL_Y == 32)
	data[2] = 0x03; // to 3
#elif (SSD1306_PIXEL_Y == 64)
	data[2] = 0x07; // to 7
#else
    data[1] = 0x0F;
#endif
	SSD1306_WriteCMD(data, 3);
	// Set contrast
	data[0] = SSD1306_CMD_SET_CONTRAST;
	data[1] = 0x7F; // 127
	SSD1306_WriteCMD(data, 2);
	// Entire display on
	data[0] = SSD1306_CMD_ALL_PIXELS_ON;
	SSD1306_WriteCMD(data, 1);
	//Set normal display
#ifdef SSD1306_INVERSE_COLOR
    data[0] = SSD1306_CMD_DISPLAY_INVERT; //set inverse color
#else
    data[0] = SSD1306_CMD_DISPLAY_NORMAL; //set normal color
#endif
	SSD1306_WriteCMD(data, 1);
	// Set display on
	data[0] = SSD1306_CMD_DISPLAY_ON;
	SSD1306_WriteCMD(data, 1);
	/* Clear screen */
	SSD1306_FillWindow(SSD1306_COLOR_BLACK);
	/* Return OK */
	return 1;
}
//-------------------------------------------------------------------------
void SSD1306_DisplayOn(void)
{
	uint8_t data[2];
    data[0] = SSD1306_CMD_SET_CHARGE_PUMP;
    data[1] = SSD1306_CMD_CHARGE_PUMP_ENABLE;
	SSD1306_WriteCMD(data, sizeof(data));
	data[0] = SSD1306_CMD_DISPLAY_ON;
	SSD1306_WriteCMD(data, sizeof(data)/2);
}
//-------------------------------------------------------------------------
void SSD1306_DisplayOff(void)
{
	uint8_t data[2];
    data[0] = SSD1306_CMD_SET_CHARGE_PUMP;
    data[1] = SSD1306_CMD_CHARGE_PUMP_DISABLE;
	SSD1306_WriteCMD(data, sizeof(data));
	data[0] = SSD1306_CMD_DISPLAY_OFF;
	SSD1306_WriteCMD(data, sizeof(data)/2);
}
//-------------------------------------------------------------------------
void SSD1306_SetContrast(uint8_t contrast)
{
	uint8_t data[2];
	SSD1306_DisplayOff();
	data[0] = SSD1306_CMD_SET_CONTRAST;
	data[1] = contrast;
    SSD1306_WriteCMD(data, sizeof(data));
	SSD1306_DisplayOn();
}
//-------------------------------------------------------------------------
void SSD1306_SetInvert(SSD1306_INVERT_t state)
{
	SSD1306_DisplayOff();;
	state |= SSD1306_CMD_DISPLAY_NORMAL;
	SSD1306_WriteCMD(&state, 1);
	SSD1306_DisplayOn();
}
//-------------------------------------------------------------------------
void SSD1306_SetPos(uint8_t x, uint8_t y)
{
	uint8_t data[3];
	data[0] = y | SSD1306_CMD_SET_PAGE;              // Page
	data[1] = (x & 0x0F) | SSD1306_CMD_SET_ADDR_LOW; // Column address LSB
	data[2] = (x >> 4) | SSD1306_CMD_SET_ADDR_HIGH;  // Column address MSB
	SSD1306_WriteCMD(data, sizeof(data));
}
//-------------------------------------------------------------------------
void SSD1306_FillRange(uint8_t x, uint8_t y, uint8_t size, SSD1306_COLOR_t color)
{
	if ((x >= SSD1306_PIXEL_X) || (y >= SSD1306_PAGES))
	{
		return;
	}
	if((x + size) > SSD1306_PIXEL_X)
	{
		size = SSD1306_PIXEL_X - x;
	}
	uint8_t colour = 0x00;
	if(color != SSD1306_COLOR_BLACK)
	{
		colour = 0xFF;
	}
	SSD1306_SetPos(x, y);
    for(uint8_t x = 0; x < size; x++)
    {
    	SSD1306_WriteData(&colour, 1);
    }
}
//-------------------------------------------------------------------------
void SSD1306_FillWindow(SSD1306_COLOR_t color)
{
	uint8_t x, y;
	uint8_t colour = 0x00;
	if(color != SSD1306_COLOR_BLACK)
	{
		colour = 0xFF;
	}
	for(y = 0; y < SSD1306_PAGES; y++)
	{
		SSD1306_SetPos(0, y);
	    for(x = 0; x < SSD1306_PIXEL_X; x++)
	    {
	    	SSD1306_WriteData(&colour, 1);
	    }
    }
}
//-------------------------------------------------------------------------
#define FONT_START_CHAR     0x20
#define FONT_MAX_CHAR       0xC1
#define FONT_FLAG_BLANK_AFTER
//-------------------------------------------------------------------------
static inline char SSD1306_AsciiToFont(char ch)
{
	if (ch == 0xB0)
	{
		return 0x7F; // '°'
	}
	else if (ch == 0xA8)
	{
		return 0xC0; // '¨'
	}
	else if (ch == 0xB8)
	{
		return 0xC1; // '¸'
	}
	else if ((ch >= 0xC0)&&(ch <= 0xFF)) // 'À'-'ÿ'
	{
		return ch - 0x40;
	}
	return 0;
}
//-------------------------------------------------------------------------
uint8_t SSD1306_PrintChar(uint8_t x, uint8_t y, char ch, SSD1306_XORMASK_t xorMask)
{
//===========================================
	if ((x >= SSD1306_PIXEL_X)||(y >= SSD1306_PAGES))
	{
		return x;
	}
//===========================================
	SSD1306_SetPos(x, y);
//===========================================
	if ((ch >= FONT_START_CHAR) && (ch <= FONT_MAX_CHAR))
	{
		uint16_t fw = ((uint16_t*)font_normal)[ch - FONT_START_CHAR];
		const uint8_t* font = font_normal + (fw & 0xFFF);
		uint16_t w = fw >> 12;
		if (w > (SSD1306_PIXEL_X - x))
		{
			w = SSD1306_PIXEL_X - x;
		}
		x += w;
		while (w--)
		{
			uint8_t idx = *font ^ xorMask;
			SSD1306_WriteData(&idx, 1);
			font++;
		}
		if (x >= SSD1306_PIXEL_X)
		{
			return x;
		}
#ifdef FONT_FLAG_BLANK_AFTER
		SSD1306_WriteData(&xorMask, 1);
		x++;
#endif
	}
//===========================================
	return x;
}
//-------------------------------------------------------------------------
uint8_t SSD1306_PrintText(uint8_t x, uint8_t y, char* string, SSD1306_XORMASK_t xorMask)
{
//===========================================
	if ((x >= SSD1306_PIXEL_X)||(y >= SSD1306_PAGES))
	{
		return x;
	}
//===========================================
	do
	{
		char ch = *string++;
		if (!ch)
		{
			break;
		}
		if (ch < FONT_START_CHAR)
		{
			continue;
		}
		if (ch > 0x7E)
		{
			ch = SSD1306_AsciiToFont(ch);
			if (!ch)
			{
				continue;
			}
		}
		x = SSD1306_PrintChar(x, y, ch, xorMask);
	} while (x < SSD1306_PIXEL_X);
//===========================================
	return x;
}
//-------------------------------------------------------------------------
uint8_t SSD1306_Print2XChar(uint8_t x, uint8_t y, char ch, SSD1306_XORMASK_t xorMask)
{
//===========================================
	uint8_t i, j, k, l;
	uint8_t var1 = 0;
	uint8_t var2 = 1;
	uint16_t sym, sym2;
	uint8_t idx[2];
//===========================================
	SSD1306_SetPos(x, y);
//===========================================
	if ((ch >= FONT_START_CHAR) && (ch <= FONT_MAX_CHAR))
	{
		uint16_t fw = ((uint16_t*)font_normal)[ch - FONT_START_CHAR];
		const uint8_t* font = font_normal + (fw & 0xFFF);
		uint16_t w = fw >> 12;
//===========================================
		for(i = 1; i <= 2; i++)
		{
			for(j = 0; j < w; j++)
			{
				sym = 0;
				l = 1;
				for(k = var1; k < var1 + 4; k++)
				{
					sym += (font[j] & (1 << k)) * l;
					l <<= 1;
				}
				sym /= var2;
				sym2 = 0;
				sym2 += sym;
				sym <<= 1;
				sym2 += sym;
				sym <<= 1;
				idx[0] = idx[1] = (uint8_t)sym2 ^ xorMask;
				SSD1306_WriteData(idx, sizeof(idx));
			}
			#ifdef FONT_FLAG_BLANK_AFTER
			idx[0] = idx[1] = xorMask;
			SSD1306_WriteData(idx, sizeof(idx));
			#endif
			SSD1306_SetPos(x, y + i);
			var1 += 4;
			var2 <<= 4;
		}
		x += w << 1;
		#ifdef FONT_FLAG_BLANK_AFTER
		x += 2;
		#endif
	}
	return x;
}
//-------------------------------------------------------------------------
uint8_t SSD1306_Print2XText(uint8_t x, uint8_t y, char* string, SSD1306_XORMASK_t xorMask)
{
//===========================================
	if ((x >= SSD1306_PIXEL_X)||(y >= SSD1306_PAGES))
	{
		return x;
	}
//===========================================
	do
	{
		char ch = *string++;
		if (!ch)
		{
			break;
		}
		if (ch < FONT_START_CHAR)
		{
			continue;
		}
		if (ch > 0x7E)
		{
			ch = SSD1306_AsciiToFont(ch);
			if (!ch)
			{
				continue;
			}
		}
		x = SSD1306_Print2XChar(x, y, ch, xorMask);
	} while (x < SSD1306_PIXEL_X);
//===========================================
	return x;
}
//-------------------------------------------------------------------------
