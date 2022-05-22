#ifndef I2C_H
#define I2C_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_gpio.h"
#include "stm32f03x_defs.h"
// ---------------------------------------------------------------------------
#define I2C_TIMEOUT				20000
// ---------------------------------------------------------------------------
#define I2C_BUS					I2C1
#define GPIO_AF_I2C1            4
// ---------------------------------------------------------------------------
void I2C_Init(GPIO_TypeDef* GPIOx, uint16_t SCL_GPIO_Pin, uint16_t SDA_GPIO_Pin);
uint8_t I2C_Read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t count);
uint8_t I2C_Write(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t count);
uint8_t I2C_IsDeviceConnected(uint8_t addr);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
