#ifndef DS18B20_H
#define DS18B20_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_onewire.h"
// ---------------------------------------------------------------------------
/* Every onewire chip has different ROM code, but all the same chips has same family code */
/* in case of DS18B20 this is 0x28 and this is first byte of ROM address */
#define DS18B20_FAMILY_CODE				0x28
#define DS18B20_CMD_ALARMSEARCH			0xEC

/* DS18B20 read temperature command */
#define DS18B20_CMD_CONVERTTEMP			0x44 	/* Convert temperature */
#define DS18B20_DECIMAL_STEPS_12BIT		0.0625
#define DS18B20_DECIMAL_STEPS_11BIT		0.125
#define DS18B20_DECIMAL_STEPS_10BIT		0.25
#define DS18B20_DECIMAL_STEPS_9BIT		0.5

/* Bits locations for resolution */
#define DS18B20_RESOLUTION_R1			6
#define DS18B20_RESOLUTION_R0			5

/* CRC enabled */
#ifdef DS18B20_USE_CRC	
#define DS18B20_DATA_LEN				9
#else
#define DS18B20_DATA_LEN				2
#endif
// ---------------------------------------------------------------------------
typedef enum {
	DS18B20_Resolution_9bits = 9,   /*!< DS18B20 9 bits resolution */
	DS18B20_Resolution_10bits = 10, /*!< DS18B20 10 bits resolution */
	DS18B20_Resolution_11bits = 11, /*!< DS18B20 11 bits resolution */
	DS18B20_Resolution_12bits = 12  /*!< DS18B20 12 bits resolution */
} DS18B20_Resolution_t;
// ---------------------------------------------------------------------------
/**
 * @brief  Starts temperature conversion for specific DS18B20 on specific onewire channel
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval 1 if device is DS18B20 or 0 if not
 */
uint8_t DS18B20_Start(OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Starts temperature conversion for all DS18B20 devices on specific onewire channel
 * @note   This mode will skip ROM addressing
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @retval None
 */
void DS18B20_StartAll(OneWire_t* OneWireStruct);

/**
 * @brief  Reads temperature from DS18B20
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  *destination: Pointer to float variable to store temperature
 * @retval Temperature status:
 *            - 0: Device is not DS18B20 or conversion is not done yet or CRC failed
 *            - > 0: Temperature is read OK
 */
uint8_t DS18B20_Read(OneWire_t* OneWireStruct, uint8_t* ROM, float* destination);

/**
 * @brief  Gets resolution for temperature conversion from DS18B20 device
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Resolution:
 *            - 0: Device is not DS18B20
 *            - 9 - 12: Resolution of DS18B20
 */
uint8_t DS18B20_GetResolution(OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Sets resolution for specific DS18B20 device
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  resolution: Resolution for DS18B20 device. This parameter can be a value of @ref DS18B20_Resolution_t enumeration.
 * @retval Success status:
 *            - 0: Device is not DS18B20
 *            - > 0: Resolution set OK
 */
uint8_t DS18B20_SetResolution(OneWire_t* OneWireStruct, uint8_t* ROM, DS18B20_Resolution_t resolution);

/**
 * @brief  Checks if device with specific ROM number is DS18B20
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Device status
 *            - 0: Device is not DS18B20
 *            - > 0: Device is DS18B20
 */
uint8_t Is_DS18B20(uint8_t* ROM);

/**
 * @brief  Sets high alarm temperature to specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  temp: integer value for temperature between -55 to 125 degrees
 * @retval Success status:
 *            - 0: Device is not DS18B20
 *            - > 0: High alarm set OK
 */
uint8_t DS18B20_SetAlarmHighTemperature(OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * @brief  Sets low alarm temperature to specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @param  temp: integer value for temperature between -55 to 125 degrees
 * @retval Success status:
 *            - 0: Device is not DS18B20
 *            - > 0: Low alarm set OK
 */
uint8_t DS18B20_SetAlarmLowTemperature(OneWire_t* OneWireStruct, uint8_t* ROM, int8_t temp);

/**
 * @brief  Disables alarm temperature for specific DS18B20 sensor
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @param  *ROM: Pointer to first byte of ROM address for desired DS12B80 device.
 *         Entire ROM address is 8-bytes long
 * @retval Success status:
 *            - 0: Device is not DS18B20
 *            - > 0: Alarm disabled OK
 */
uint8_t DS18B20_DisableAlarmTemperature(OneWire_t* OneWireStruct, uint8_t* ROM);

/**
 * @brief  Searches for devices with alarm flag set
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @retval Alarm search status
 *            - 0: No device found with alarm flag set
 *            - > 0: Device is found with alarm flag
 * @note   To get all devices on one onewire channel with alarm flag set, you can do this:
@verbatim
while (DS18B20_AlarmSearch(&OneWireStruct)) {
	//Read device ID here 
	//Print to user device by device
}
@endverbatim 
 * @retval 1 if any device has flag, otherwise 0
 */
uint8_t DS18B20_AlarmSearch(OneWire_t* OneWireStruct);

/**
 * @brief  Checks if all DS18B20 sensors are done with temperature conversion
 * @param  *OneWireStruct: Pointer to @ref OneWire_t working structure (OneWire channel)
 * @retval Conversion status
 *            - 0: Not all devices are done
 *            - > 0: All devices are done with conversion
 */
uint8_t DS18B20_AllDone(OneWire_t* OneWireStruct);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif

