//----------------------------------------------------------------------------
// Библиотека температурного сенсора LM73: lm73.h
//----------------------------------------------------------------------------
#ifndef LM73_H
#define LM73_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------Подключаемые файлы------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_twi.h"
// ---------------------------------------------------------------------------
// Адреса регистров
#define LM73_REG_TEMPERATURE	0x00 // регистр данных температуры
#define LM73_REG_CONFIG			0x01 // регистр конфигурации
#define LM73_REG_THIGH			0x02 // регистр верхнего предела срабатывания тревоги
#define LM73_REG_TLOW			0x03 // регистр нижнего предела срабатывания тревоги
#define LM73_REG_CTRLSTATUS		0x04 // регистр контроля/статуса
#define LM73_REG_ID				0x07 // регистр идентификационных данных сенсора
// Биты регистра конфигурации
#define LM73_BIT_ONE_SHOT		0x04 // бит режима одноразовой конвертации температуры (активет при LM73_POWER_OFF)
#define LM73_BIT_ALERT_RST		0x08 // бит сброса пина ALERT и бита ALERT в регистре статуса
#define LM73_BIT_ALERT_POL		0x10 // бит установки полярности пина ALERT
#define LM73_BIT_ALERT_EN		0x20 // бит включения пина ALERT
// Биты регистр контроля/статуса
#define LM73_BIT_DAV_FLAG		0x01
#define LM73_BIT_TLOW_FLAG		0x02
#define LM73_BIT_THIGH_FLAG		0x04
#define LM73_BIT_ALRT_STAT		0x08

// Маски регистра конфигурации
#define LM73_MASK_POWER			~(LM73_POWER_ON | LM73_POWER_OFF)
// Маски регистра контроля/статуса
#define LM73_MASK_RESOLUTION	~(LM73_RESOLUTION_11BIT | LM73_RESOLUTION_12BIT | LM73_RESOLUTION_13BIT | LM73_RESOLUTION_14BIT)
//----------------------------------------------------------------------------
typedef enum
{
	// LM73-0
	LM73_0_I2C_FLOAT = 0x90, // Float
	LM73_0_I2C_GND   = 0x92, // Ground
	LM73_0_I2C_VDD   = 0x94, // VDD
	// LM73-1
	LM73_1_I2C_FLOAT = 0x98, // Float
	LM73_1_I2C_GND   = 0x9A, // Ground
	LM73_1_I2C_VDD   = 0x9C  // VDD
} LM73_twiaddr_t;
//----------------------------------------------------------------------------
typedef enum
{
	LM73_RESOLUTION_11BIT = 0b00000000,
	LM73_RESOLUTION_12BIT = 0b00100000,
	LM73_RESOLUTION_13BIT = 0b01000000,
	LM73_RESOLUTION_14BIT = 0b01100000
} LM73_resolution_t;
//----------------------------------------------------------------------------
typedef enum
{
	LM73_POWER_ON  = 0 << 7, // включаем обычный режим сенсора
	LM73_POWER_OFF = 1 << 7  // включаем режим экономии энергии сенсора
} LM73_power_t;
//---------------------------- Прототипы функций: ----------------------------
uint8_t LM73_Start(LM73_resolution_t resolution);
LM73_twiaddr_t LM73_GetAddress(void);
uint8_t LM73_Read(uint8_t reg, void* data, uint8_t count);
uint8_t LM73_Write(uint8_t reg, void* data, uint8_t count);
uint8_t LM73_GetTemperature(double* temperature, uint8_t reg);
uint8_t LM73_SetPowerMode(LM73_power_t pwrmode);
uint8_t LM73_SetResolution(LM73_resolution_t resolution);
uint8_t LM73_SetAlert(double* high, double* low);
uint8_t LM73_GetAlertStatus(TwoWire_t* TwoWireStruct);
uint8_t LM73_GetIdCode(uint8_t* mancode);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif /* LM73_H_ */
