#ifndef GPIO_H
#define GPIO_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
// ---------------------------------------------------------------------------
#ifndef GPIO_PIN_0
#define GPIO_PIN_0		((uint16_t)0x0001)
#define GPIO_PIN_1		((uint16_t)0x0002)
#define GPIO_PIN_2		((uint16_t)0x0004)
#define GPIO_PIN_3		((uint16_t)0x0008)
#define GPIO_PIN_4		((uint16_t)0x0010)
#define GPIO_PIN_5		((uint16_t)0x0020)
#define GPIO_PIN_6		((uint16_t)0x0040)
#define GPIO_PIN_7		((uint16_t)0x0080)
#define GPIO_PIN_8		((uint16_t)0x0100)
#define GPIO_PIN_9		((uint16_t)0x0200)
#define GPIO_PIN_10		((uint16_t)0x0400)
#define GPIO_PIN_11		((uint16_t)0x0800)
#define GPIO_PIN_12		((uint16_t)0x1000)
#define GPIO_PIN_13		((uint16_t)0x2000)
#define GPIO_PIN_14		((uint16_t)0x4000)
#define GPIO_PIN_15		((uint16_t)0x8000)
#define GPIO_PIN_ALL	((uint16_t)0xFFFF)
#endif

#ifndef GPIO_Pin_0
#define GPIO_Pin_0		((uint16_t)0x0001)
#define GPIO_Pin_1		((uint16_t)0x0002)
#define GPIO_Pin_2		((uint16_t)0x0004)
#define GPIO_Pin_3		((uint16_t)0x0008)
#define GPIO_Pin_4		((uint16_t)0x0010)
#define GPIO_Pin_5		((uint16_t)0x0020)
#define GPIO_Pin_6		((uint16_t)0x0040)
#define GPIO_Pin_7		((uint16_t)0x0080)
#define GPIO_Pin_8		((uint16_t)0x0100)
#define GPIO_Pin_9		((uint16_t)0x0200)
#define GPIO_Pin_10		((uint16_t)0x0400)
#define GPIO_Pin_11		((uint16_t)0x0800)
#define GPIO_Pin_12		((uint16_t)0x1000)
#define GPIO_Pin_13		((uint16_t)0x2000)
#define GPIO_Pin_14		((uint16_t)0x4000)
#define GPIO_Pin_15		((uint16_t)0x8000)
#define GPIO_Pin_All	((uint16_t)0xFFFF)
#endif
// ---------------------------------------------------------------------------
typedef enum {
	GPIO_Mode_IN = 0x00,  /*!< GPIO Pin as General Purpose Input */
	GPIO_Mode_OUT = 0x01, /*!< GPIO Pin as General Purpose Output */
	GPIO_Mode_AF = 0x02,  /*!< GPIO Pin as Alternate Function */
	GPIO_Mode_AN = 0x03,  /*!< GPIO Pin as Analog */
} GPIO_Mode_t;

/**
 * @brief GPIO Output type enumeration
 */
typedef enum {
	GPIO_OType_PP = 0x00, /*!< GPIO Output Type Push-Pull */
	GPIO_OType_OD = 0x01  /*!< GPIO Output Type Open-Drain */
} GPIO_OType_t;

/**
 * @brief  GPIO Speed enumeration
 */
typedef enum {
	GPIO_Speed_Low = 0x00,    /*!< GPIO Speed Low: 2 MHz */
	GPIO_Speed_Medium = 0x01, /*!< GPIO Speed Medium: 10 MHz */
	GPIO_Speed_High = 0x02    /*!< GPIO Speed High: 50 MHz */
} GPIO_Speed_t;

/**
 * @brief GPIO pull resistors enumeration
 */
typedef enum {
	GPIO_PuPd_NOPULL = 0x00, /*!< No pull resistor */
	GPIO_PuPd_UP = 0x01,     /*!< Pull up resistor enabled */
	GPIO_PuPd_DOWN = 0x02    /*!< Pull down resistor enabled */
} GPIO_PuPd_t;
// ---------------------------------------------------------------------------
/**
 * @brief  Initializes GPIO pins(s)
 * @note   This function also enables clock for GPIO port
 * @param  GPIOx: Pointer to GPIOx port you will use for initialization
 * @param  GPIO_Pin: GPIO pin(s) you will use for initialization
 * @param  GPIO_Mode: Select GPIO mode. This parameter can be a value of @ref GPIO_Mode_t enumeration
 * @param  GPIO_OType: Select GPIO Output type. This parameter can be a value of @ref GPIO_OType_t enumeration
 * @param  GPIO_PuPd: Select GPIO pull resistor. This parameter can be a value of @ref GPIO_PuPd_t enumeration
 * @param  GPIO_Speed: Select GPIO speed. This parameter can be a value of @ref GPIO_Speed_t enumeration
 * @retval None
 */
void GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_Mode_t GPIO_Mode, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed);

/**
 * @brief  Initializes GPIO pins(s) as alternate function
 * @note   This function also enables clock for GPIO port
 * @param  GPIOx: Pointer to GPIOx port you will use for initialization
 * @param  GPIO_Pin: GPIO pin(s) you will use for initialization
 * @param  GPIO_OType: Select GPIO Output type. This parameter can be a value of @ref GPIO_OType_t enumeration
 * @param  GPIO_PuPd: Select GPIO pull resistor. This parameter can be a value of @ref GPIO_PuPd_t enumeration
 * @param  GPIO_Speed: Select GPIO speed. This parameter can be a value of @ref GPIO_Speed_t enumeration
 * @param  Alternate: Alternate function you will use
 * @retval None
 */
void GPIO_InitAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed, uint8_t Alternate);

/**
 * @brief  Deinitializes pin(s)
 * @note   Pins(s) will be set as analog mode to get low power consumption
 * @param  GPIOx: GPIOx PORT where you want to set pin as input
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as input
 * @retval None
 */
void GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as input 
 * @note   Pins HAVE to be initialized first using @ref GPIO_Init() or @ref GPIO_InitAlternate() function
 * @note   This is just an option for fast input mode
 * @param  GPIOx: GPIOx PORT where you want to set pin as input
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as input
 * @retval None
 */
void GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as output
 * @note   Pins HAVE to be initialized first using @ref GPIO_Init() or @ref GPIO_InitAlternate() function
 * @note   This is just an option for fast output mode 
 * @param  GPIOx: GPIOx PORT where you want to set pin as output
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as output
 * @retval None
 */
void GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pin(s) as analog
 * @note   Pins HAVE to be initialized first using @ref GPIO_Init() or @ref GPIO_InitAlternate() function
 * @note   This is just an option for fast analog mode 
 * @param  GPIOx: GPIOx PORT where you want to set pin as analog
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as analog
 * @retval None
 */
void GPIO_SetPinAsAnalog(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/** 
 * @brief  Sets pin(s) as alternate function
 * @note   For proper alternate function, you should first init pin using @ref GPIO_InitAlternate() function.
 *            This functions is only used for changing GPIO mode
 * @param  GPIOx: GPIOx PORT where you want to set pin as alternate
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as alternate
 * @retval None
 */
void GPIO_SetPinAsAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Sets pull resistor settings to GPIO pin(s)
 * @note   Pins HAVE to be initialized first using @ref GPIO_Init() or @ref GPIO_InitAlternate() function
 * @param  *GPIOx: GPIOx PORT where you want to select pull resistor
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them as output
 * @param  GPIO_PuPd: Pull resistor option. This parameter can be a value of @ref GPIO_PuPd_t enumeration
 * @retval None
 */
void GPIO_SetPullResistor(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PuPd_t GPIO_PuPd);

/**
 * @brief  Sets pin(s) low
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin low
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them low
 * @retval None
 */
#define GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->ODR &= ~(GPIO_Pin))

/**
 * @brief  Sets pin(s) high
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin high
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them high
 * @retval None
 */
#define GPIO_SetPinHigh(GPIOx, GPIO_Pin) 		((GPIOx)->ODR |= (GPIO_Pin))//((GPIOx)->BSRR |= (GPIO_Pin))

/**
 * @brief  Sets pin(s) value
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them value
 * @param  val: If parameter is 0 then pin will be low, otherwise high
 * @retval None
 */
#define GPIO_SetPinValue(GPIOx, GPIO_Pin, val)	((val) ? GPIO_SetPinHigh(GPIOx, GPIO_Pin) : GPIO_SetPinLow(GPIOx, GPIO_Pin))

/**
 * @brief  Toggles pin(s)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to toggle pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to toggle them all at a time
 * @retval None
 */
#define GPIO_TogglePinValue(GPIOx, GPIO_Pin)		((GPIOx)->ODR ^= (GPIO_Pin))

/**
 * @brief  Sets value to entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set value
 * @param  value: Value for GPIO OUTPUT data
 * @retval None
 */
#define GPIO_SetPortValue(GPIOx, value)			((GPIOx)->ODR = (value))

/**
 * @brief  Gets input data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets output data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets input value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input data value
 * @retval Entire PORT INPUT register
 */
#define GPIO_GetPortInputValue(GPIOx)			((GPIOx)->IDR)

/**
 * @brief  Gets output value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output data value
 * @retval Entire PORT OUTPUT register
 */
#define GPIO_GetPortOutputValue(GPIOx)			((GPIOx)->ODR)

/**
 * @brief  Gets port source from desired GPIOx PORT
 * @note   Meant for private use, unless you know what are you doing
 * @param  GPIOx: GPIO PORT for calculating port source
 * @retval Calculated port source for GPIO
 */
uint16_t GPIO_GetPortSource(GPIO_TypeDef* GPIOx);

/**
 * @brief  Gets pin source from desired GPIO pin
 * @note   Meant for private use, unless you know what are you doing
 * @param  GPIO_Pin: GPIO pin for calculating port source
 * @retval Calculated pin source for GPIO pin
 */
uint16_t GPIO_GetPinSource(uint16_t GPIO_Pin);

/**
 * @brief  Locks GPIOx register for future changes
 * @note   You are not able to config GPIO registers until new MCU reset occurs
 * @param  *GPIOx: GPIOx PORT where you want to lock config registers
 * @param  GPIO_Pin: GPIO pin(s) where you want to lock config registers
 * @retval None
 */
void GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/** 
 * @brief  Gets bit separated pins which were used at least once in library and were not deinitialized
 * @param  *GPIOx: Pointer to GPIOx peripheral where to check used GPIO pins
 * @retval Bit values for used pins
 */
uint16_t GPIO_GetUsedPins(GPIO_TypeDef* GPIOx);

/** 
 * @brief  Gets bit separated pins which were not used at in library or were deinitialized
 * @param  *GPIOx: Pointer to GPIOx peripheral where to check used GPIO pins
 * @retval Bit values for free pins
 */
uint16_t GPIO_GetFreePins(GPIO_TypeDef* GPIOx);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
