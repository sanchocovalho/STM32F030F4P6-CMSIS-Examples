#ifndef ONEWIRE_H
#define ONEWIRE_H
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
/* OneWire delay */
#define ONEWIRE_DELAY(x)				DelayUs(x)

/* Pin settings */
#define ONEWIRE_LOW(structure)			GPIO_SetPinLow((structure)->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_HIGH(structure)			GPIO_SetPinHigh((structure)->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_INPUT(structure)		GPIO_SetPinAsInput(structure->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_OUTPUT(structure)		GPIO_SetPinAsOutput(structure->GPIOx, (structure)->GPIO_Pin)

/* OneWire commands */
#define ONEWIRE_CMD_RSCRATCHPAD			0xBE
#define ONEWIRE_CMD_WSCRATCHPAD			0x4E
#define ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define ONEWIRE_CMD_RECEEPROM			0xB8
#define ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define ONEWIRE_CMD_SEARCHROM			0xF0
#define ONEWIRE_CMD_READROM				0x33
#define ONEWIRE_CMD_MATCHROM			0x55
#define ONEWIRE_CMD_SKIPROM				0xCC
// ---------------------------------------------------------------------------
/**
 * @brief  OneWire working struct
 * @note   Except ROM_NO member, everything is fully private and should not be touched by user
 */
typedef struct {
	GPIO_TypeDef* GPIOx;           /*!< GPIOx port to be used for I/O functions */
	uint16_t GPIO_Pin;             /*!< GPIO Pin to be used for I/O functions */
	uint8_t LastDiscrepancy;       /*!< Search private */
	uint8_t LastFamilyDiscrepancy; /*!< Search private */
	uint8_t LastDeviceFlag;        /*!< Search private */
	uint8_t ROM_NO[8];             /*!< 8-bytes address of last search device */
} OneWire_t;
// ---------------------------------------------------------------------------
/**
 * @brief  Initializes OneWire bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t empty working onewire structure
 * @param  *Pointer to GPIO port used for onewire channel
 * @param  GPIO_Pin: GPIO Pin on specific GPIOx to be used for onewire channel
 * @retval None
 */
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
 * @brief  Resets OneWire bus
 * 
 * @note   Sends reset command for OneWire
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @retval None
 */
uint8_t OneWire_Reset(OneWire_t* OneWireStruct);

/**
 * @brief  Reads byte from one wire bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @retval Byte from read operation
 */
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct);

/**
 * @brief  Writes byte to bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @param  byte: 8-bit value to write over OneWire protocol
 * @retval None
 */
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte);

/**
 * @brief  Writes single bit to onewire bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @param  bit: Bit value to send, 1 or 0
 * @retval None
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit);

/**
 * @brief  Writes bytes to bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @param  *buf: Pointer to buffer which will be written
 * @param  count: Number of bytes  which will be written
 * @retval None
 */
void OneWire_WriteBytes(OneWire_t* OneWireStruct, uint8_t *buf, uint16_t count);

/**
 * @brief  Reads bytes from one wire bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @param  *buf: Pointer to buffer in which will be read
 * @param  count: Number of bytes which will be read
 * @retval None
 */
void OneWire_ReadBytes(OneWire_t* OneWireStruct, uint8_t *buf, uint16_t count);

/**
 * @brief  Reads bit from one wire bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure
 * @retval Read bit
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct);

/**
 * @brief  Searches for OneWire devices on specific Onewire port
 * @note   Not meant for public use. Use @ref OneWire_First and @ref OneWire_Next for this.
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire structure where to search
 * @param  Device status:
 *            - 0: No devices detected
 *            - > 0: Device detected
 */
uint8_t OneWire_Search(OneWire_t* OneWireStruct);

/**
 * @brief  Resets search states
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire where to reset search values
 * @retval None
 */
void OneWire_ResetSearch(OneWire_t* OneWireStruct);

/**
 * @brief  Starts search, reset states first
 * @note   When you want to search for ALL devices on one onewire port, you should first use this function.
@verbatim
/...Initialization before
status = OneWire_First(&OneWireStruct);
while (status) {
	//Save ROM number from device
	OneWire_GetFullROM(ROM_Array_Pointer);
	//Check for new device
	status = OneWire_Next(&OneWireStruct);
}
@endverbatim
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire where to reset search values
 * @param  Device status:
 *            - 0: No devices detected
 *            - > 0: Device detected
 */
uint8_t OneWire_First(OneWire_t* OneWireStruct);

/**
 * @brief  Reads next device
 * @note   Use @ref OneWire_First to start searching
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire
 * @param  Device status:
 *            - 0: No devices detected any more
 *            - > 0: New device detected
 */
uint8_t OneWire_Next(OneWire_t* OneWireStruct);

/**
 * @brief  Gets ROM number from device from search
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire
 * @param  index: Because each device has 8-bytes long ROm address, you have to call this 8 times, to get ROM bytes from 0 to 7
 * @reetval ROM byte for index (0 to 7) at current found device
 */
uint8_t OneWire_GetROM(OneWire_t* OneWireStruct, uint8_t index);

/**
 * @brief  Gets all 8 bytes ROM value from device from search
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire
 * @param  *firstIndex: Pointer to first location for first byte, other bytes are automatically incremented
 * @retval None
 */
void OneWire_GetFullROM(OneWire_t* OneWireStruct, uint8_t *firstIndex);

/**
 * @brief  Selects specific slave on bus
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire
 * @param  *addr: Pointer to first location of 8-bytes long ROM address
 * @retval None
 */
void OneWire_Select(OneWire_t* OneWireStruct, uint8_t* addr);

/**
 * @brief  Selects specific slave on bus with pointer address
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working onewire
 * @param  *ROM: Pointer to first byte of ROM address
 * @retval None
 */
void OneWire_SelectWithPointer(OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Calculates 8-bit CRC for 1-wire devices
 * @param  *addr: Pointer to 8-bit array of data to calculate CRC
 * @param  len: Number of bytes to check
 *
 * @retval Calculated CRC from input data
 */
uint8_t OneWire_CRC8(uint8_t* addr, uint8_t len);
/**
 * @brief  Calculates 16-bit CRC for 1-wire devices
 * @param  *input: Pointer to input data
 * @param  len: Number of bytes to check
 * @param  crc: Input CRC which will be read
 *
 * @retval Calculated CRC from input data
 */
uint16_t OneWire_CRC16(uint8_t* input, uint16_t len, uint16_t crc);

/**
 * @brief  Calculates 16-bit CRC for 1-wire devices
 * @param  *input: Pointer to input data
 * @param  len: Number of bytes to check
 * @param  len: Input CRC which will be checked
 * @param  crc: Input CRC which will be read
 *
 * @retval CRC checking status: 1 is correct, 0 is wrong
 */
uint8_t OneWire_CheckCRC16(uint8_t* input, uint16_t len, uint8_t* inverted_crc, uint16_t crc);
// ---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif

