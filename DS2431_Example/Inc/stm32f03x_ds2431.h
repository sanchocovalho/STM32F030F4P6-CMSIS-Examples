#ifndef _DS2431_H
#define _DS2431_H
//*************************************************************************************
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
//*************************************************************************************
//#include <string.h>
#include "stm32f03x_delay.h"
#include "stm32f03x_onewire.h"
//*************************************************************************************
#define DS2431_FAMILY_CODE          0x2D
#define DS2431_PF_MASK              0x07
#define DS2431_WRITE_MASK           0xAA
//*************************************************************************************
#define DS2431_EEPROM_SIZE          128
#define DS2431_ROW_SIZE             8
#define DS2431_CMD_SIZE             3
#define DS2431_CRC_SIZE             2
#define DS2431_READ_RETRY           2
#define DS2431_BUFFER_SIZE_8        (DS2431_ROW_SIZE + DS2431_CMD_SIZE + DS2431_CRC_SIZE)
//*************************************************************************************
#define DS2431_DELAY(x)             DelayMs(x)
//*************************************************************************************
#define DS2431_WRITE_SCRATCHPAD     0x0F
#define DS2431_READ_SCRATCHPAD      0xAA
#define DS2431_COPY_SCRATCHPAD      0x55
#define DS2431_READ_MEMORY          0xF0
//*************************************************************************************
/**
 * @brief  Check device ROM address to match DS2431 device
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @retval Writing status:
 *            - 0: Fail
 *            - 1: Success
 */
uint8_t Is_DS2431(uint8_t *ROM);
//*************************************************************************************
/**
 * @brief  Starts communication with DS2431 on specific onewire channel
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @retval 1 if device is DS2431 or 0 if not
 */
uint8_t DS2431_Start(OneWire_t* OneWireStruct, uint8_t *ROM);
//*************************************************************************************
/**
 * @brief  Reads bytes from DS2431
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @param  address: 1st byte address which desired to read
 * @param  *buf: Pointer to buffer which will bytes stored
 * @param  count: Byte length
 * @retval Reading status:
 *            - 0: Fail
 *            - 1: Success
 */
uint8_t DS2431_Read(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf, uint16_t count);
//*************************************************************************************
/**
 * @brief  Write 8 bytes by aligned address on DS2431
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @param  address: 1st byte aligned address which desired to write
 * @param  *buf: Pointer to buffer which stores 8 bytes
 * @retval Writing status:
 *            - 0: Fail
 *            - 1: Success
 */
uint8_t DS2431_WriteAlignedRow(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf);
//*************************************************************************************
/**
 * @brief  Write bytes on DS2431
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @param  address: 1st byte address which desired to write
 * @param  *buf: Pointer to buffer which stores the bytes
 * @param  count: Byte length
 * @retval Writing status:
 *            - 0: Fail
 *            - 1: Success
 */
uint8_t DS2431_Write(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf, uint16_t count);
//*************************************************************************************
/**
 * @brief  Erase full memory on DS2431
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS2431 device.
 *         Entire ROM address is 8-bytes long
 * @retval Writing status:
 *            - 0: Fail
 *            - 1: Success
 */
uint8_t DS2431_EraseFullMemory(OneWire_t* OneWireStruct, uint8_t *ROM);
//*************************************************************************************
#ifdef __cplusplus
}
#endif
//*************************************************************************************
#endif // _DS2431_H
