#ifndef SSD1306_H
#define SSD1306_H
//----------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
//----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_i2c.h"
#include "font.h"
//----------------------------------------------------------------------------
#define SSD1306_I2C_ADDR                 0x78 // 0x3C << 1
//----------------------------------------------------------------------------
/* SSD1306 settings */
#define SSD1306_PIXEL_X                  128  // Number of pixels in X-direction
#define SSD1306_PIXEL_Y                  32   // Number of pixels in Y-direction
#define SSD1306_PAGES                    SSD1306_PIXEL_Y / 8 // Number of pages
//----------------------------------------------------------------------------
#define SSD1306_BUFFER_SIZE              (SSD1306_PIXEL_X *  SSD1306_PIXEL_Y) / 8
//----------------------------------------------------------------------------
#define SSD1306_CONTROL_CMD              0x00
#define SSD1306_CONTROL_DATA             0x40
//----------------------------------------------------------------------------
// Set Column Address. Value range: 0~131.
#define SSD1306_CMD_SET_ADDR_LOW         0x00 // set column address bits 0..3
#define SSD1306_CMD_SET_ADDR_HIGH        0x10 // set column address bits 4..7
// Set Page Address. Possible value = 0~8.
#define SSD1306_CMD_SET_PAGE             0xB0
// Set Memory Addressing Mode
#define SSD1306_CMD_SET_ADDRESS_MODE     0x20
// Set Address
#define SSD1306_CMD_SET_COLUMN_ADDRESS   0x21 // set column address
#define SSD1306_CMD_SET_PAGE_ADDRESS     0x22 // set page address
// Set Contrast. Range: 0~255.
#define SSD1306_CMD_SET_CONTRAST         0x81
// Set Display Start Line. Range: 0~63.
#define SSD1306_CMD_SET_START_LINE       0x40
// Set All Pixel ON. Default: OFF
#define SSD1306_CMD_ALL_PIXELS_ON        0xA4 // all display pixels are on
#define SSD1306_CMD_ALL_PIXELS_OFF       0xA5 // all display pixels are off
// Set Display Enable
#define SSD1306_CMD_DISPLAY_OFF          0xAE // turn display off
#define SSD1306_CMD_DISPLAY_ON           0xAF // turn display on
// Set Inverse Display
#define SSD1306_CMD_DISPLAY_NORMAL       0xA6 // set normal display mode
#define SSD1306_CMD_DISPLAY_INVERT       0xA7 // set invert display mode
// Set SEG Direction
#define SSD1306_CMD_SEG_DIR_NORMAL       0xA1 // set relationship between RAM column address and display driver: normal (MX=0)
#define SSD1306_CMD_SEG_DIR_INVERT       0xA0 // set relationship between RAM column address and display driver: invert (MX=1)
// Set Multiplex Ratio
#define SSD1306_CMD_SET_MULTIPLEX_RATIO  0xA8
// Enable Charge Pump Regulator
#define SSD1306_CMD_SET_CHARGE_PUMP      0x8D
#define SSD1306_CMD_CHARGE_PUMP_ENABLE   0x14
#define SSD1306_CMD_CHARGE_PUMP_DISABLE  0x10
// Set COM Direction
#define SSD1306_CMD_COM_DIR_NORMAL       0xC8 // set the COM output scan direction 0->63 (MY=0)
#define SSD1306_CMD_COM_DIR_INVERT       0xC0 // set the COM output scan direction 63->0 (MY=1)
// Set Display Offset
#define SSD1306_CMD_DISPLAY_OFFSET       0xD3
// Set Display Clock Divide Ratio/ Oscillator Frequency
#define SSD1306_CMD_RATIO_FREQUENCY      0xD5 //A[3:0] --> Range 1 to 16, A[7:4] -->  default setting is 1000b
// Set Pre-charge Period
#define SSD1306_CMD_PRECHARGE_PERIOD     0xD9
// Set COM Pins Hardware Configuration
#define SSD1306_CMD_COM_CONFIG           0xDA
//----------------------------------------------------------------------------
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;
//----------------------------------------------------------------------------
typedef enum
{
	SSD1306_NORMAL = 0x00,
	SSD1306_INVERT = 0x01
} SSD1306_INVERT_t;
//----------------------------------------------------------------------------
typedef enum
{
	SSD1306_BLACK_MASK = 0x00,
	SSD1306_WHITE_MASK = 0xFF
} SSD1306_XORMASK_t;
//----------------------------------------------------------------------------

/**
 * @brief  Initializes SSD1306 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - 1: LCD initialized OK and ready to use
 */
uint8_t SSD1306_Init(void);
/**
 * @brief  Turns on SSD1306 LCD
 * @param  None
 * @retval None
 */
void SSD1306_DisplayOn(void);
/**
 * @brief  Turns off SSD1306 LCD
 * @param  None
 * @retval None
 */
void SSD1306_DisplayOff(void);
/**
 * @brief  Sets contrast for SSD1306 LCD
 * @param  contrast: value in range from 0x00 to 0xFF
 * @retval None
 */
void SSD1306_SetContrast(uint8_t contrast);
/**
 * @brief  Invert pixels for SSD1306 LCD
 * @param  state: SSD1306_NORMAL or SSD1306_INVERT
 * @retval None
 */
void SSD1306_SetInvert(SSD1306_INVERT_t state);
/**
 * @brief  Sets caret position for SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @retval None
 */
void SSD1306_SetPos(uint8_t x, uint8_t y);
/**
 * @brief  Fills range of page for SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @param  size: number of pixels
 * @param  color: color of pixels (SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE)
 * @retval None
 */
void SSD1306_FillRange(uint8_t x, uint8_t y, uint8_t size, SSD1306_COLOR_t color);
/**
 * @brief  Fills all window for SSD1306 LCD
 * @param  color: color of pixels (SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE)
 * @retval None
 */
void SSD1306_FillWindow(SSD1306_COLOR_t color);
//----------------------------------------------------------------------------
/**
 * @brief  Prints character on SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @param  ch: Character to be written
 * @param  xorMask: mask for printing normal (SSD1306_BLACK_MASK) and inverting (SSD1306_WHITE_MASK) pixels
 * @retval pixel offset for next character
 */
uint8_t SSD1306_PrintChar(uint8_t x, uint8_t y, char ch, SSD1306_XORMASK_t xorMask);
/**
 * @brief  Prints string on SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @param  string: String to be written
 * @param  xorMask: mask for printing normal (SSD1306_BLACK_MASK) and inverting (SSD1306_WHITE_MASK) pixels
 * @retval pixel offset for next string
 */
uint8_t SSD1306_PrintText(uint8_t x, uint8_t y, char* string, SSD1306_XORMASK_t xorMask);
/**
 * @brief  Prints big size (2X) character on SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @param  ch: Character to be written
 * @param  xorMask: mask for printing normal (SSD1306_BLACK_MASK) and inverting (SSD1306_WHITE_MASK) pixels
 * @retval pixel offset for next character
 */
uint8_t SSD1306_Print2XChar(uint8_t x, uint8_t y, char ch, SSD1306_XORMASK_t xorMask);
/**
 * @brief  Prints big size (2X) string on SSD1306 LCD
 * @param  x: pixel offset (from 0 to SSD1306_PIXEL_X - 1)
 * @param  y: number of page (from 0 to SSD1306_PAGES - 1)
 * @param  string: String to be written
 * @param  xorMask: mask for printing normal (SSD1306_BLACK_MASK) and inverting (SSD1306_WHITE_MASK) pixels
 * @retval pixel offset for next string
 */
uint8_t SSD1306_Print2XText(uint8_t x, uint8_t y, char* string, SSD1306_XORMASK_t xorMask);
//----------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
//----------------------------------------------------------------------------
#endif
