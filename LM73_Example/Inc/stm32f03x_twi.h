#ifndef TWOWIRE_H
#define TWOWIRE_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_delay.h"
#include "stm32f03x_gpio.h"
// ---------------------------------------------------------------------------
typedef struct {
	GPIO_TypeDef* SCL_GPIOx;           /*!< GPIOx port to be used for SCL */
	GPIO_TypeDef* SDA_GPIOx;           /*!< GPIOx port to be used for SDA */
	uint16_t SCL_GPIO_Pin;             /*!< GPIO Pin to be used SCL */
	uint16_t SDA_GPIO_Pin;             /*!< GPIO Pin to be used SDA */
} TwoWire_t;
// ---------------------------------------------------------------------------
/* TwoWire delay */
#define TWOWIRE_TIME_US                     5          // for 400 kHz I2C frequency
//#define TWOWIRE_TIME_US                     20         // for 100 kHz I2C frequency
#define TWOWIRE_DELAY(x)                    Delay500ns(x)
/* Pin settings */
#define TWOWIRE_SCL_LOW(structure)			GPIO_SetPinLow((structure)->SCL_GPIOx, (structure)->SCL_GPIO_Pin)
#define TWOWIRE_SCL_HIGH(structure)			GPIO_SetPinHigh((structure)->SCL_GPIOx, (structure)->SCL_GPIO_Pin)
#define TWOWIRE_SDA_LOW(structure)			GPIO_SetPinLow((structure)->SDA_GPIOx, (structure)->SDA_GPIO_Pin)
#define TWOWIRE_SDA_HIGH(structure)			GPIO_SetPinHigh((structure)->SDA_GPIOx, (structure)->SDA_GPIO_Pin)
#define TWOWIRE_SDA_INPUT(structure)		GPIO_SetPinAsInput(structure->SDA_GPIOx, (structure)->SDA_GPIO_Pin)
#define TWOWIRE_SDA_OUTPUT(structure)		GPIO_SetPinAsOutput(structure->SDA_GPIOx, (structure)->SDA_GPIO_Pin)
// ---------------------------------------------------------------------------
void TwoWire_Init(GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin, GPIO_TypeDef* SDA_GPIOx, uint16_t SDA_GPIO_Pin);
uint8_t TwoWire_IsDeviceConnected(uint8_t addr);
uint8_t TwoWire_Read(uint8_t addr, uint8_t reg, void* data, uint8_t count);
uint8_t TwoWire_Write(uint8_t addr, uint8_t reg, void* data, uint8_t count);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
