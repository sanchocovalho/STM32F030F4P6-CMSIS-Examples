#ifndef BMP180_H
#define BMP180_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_i2c.h"
#include "math.h"
// ---------------------------------------------------------------------------
/* BMP180 I2C address */
#define BMP180_I2C_ADDRESS			0xEE  // 0x77 << 1
#define BMP180_CHIP_ID				0x55

/* Registers */
#define	BMP180_REG_CONTROL 	        0xF4
#define	BMP180_REG_RESULT 		    0xF6
#define BMP180_REG_EEPROM		    0xAA
#define BMP180_REG_CHIPID           0xD0

/* Commands */
#define	BMP180_CMD_TEMPERATURE 	    0x2E
#define	BMP180_CMD_PRESSURE_0 	    0x34
#define	BMP180_CMD_PRESSURE_1 	    0x74
#define	BMP180_CMD_PRESSURE_2 	    0xB4
#define	BMP180_CMD_PRESSURE_3 	    0xF4

/* Minimum waiting delay, in microseconds */
#define BMP180_TEMPERATURE_DELAY	4500
#define BMP180_PRESSURE_0_DELAY		4500
#define BMP180_PRESSURE_1_DELAY		7500
#define BMP180_PRESSURE_2_DELAY		13000
#define BMP180_PRESSURE_3_DELAY		25000
// ---------------------------------------------------------------------------
/**
 * @brief  Options for oversampling settings
 * @note   This settings differs in samples for one result 
 *         and sample time for one result
 */
typedef enum {
	BMP180_MODE_ULTRA_LOW_POWER = 0x00,      /*!< 1 sample for result */
	BMP180_MODE_STANDARD = 0x01,             /*!< 2 samples for result */
	BMP180_MODE_HIGH_RESOLUTION = 0x02,      /*!< 3 samples for result */
	BMP180_MODE_ULTRA_HIGH_RESOLUTION = 0x03 /*!< 4 samples for result */
} BMP180_MODE_t;
// ---------------------------------------------------------------------------
/**
 * @brief  BMP180 main structure
 */
typedef struct {
	int16_t   ac1;	        /* AC1 EEPROM calibration value */
	int16_t   ac2;	        /* AC2 EEPROM calibration value */
	int16_t   ac3;	        /* AC3 EEPROM calibration value */
	uint16_t  ac4;	        /* AC4 EEPROM calibration value */
	uint16_t  ac5;	        /* AC5 EEPROM calibration value */
	uint16_t  ac6;	        /* AC6 EEPROM calibration value */
	int16_t   b1;           /* B1 EEPROM calibration value */
	int16_t   b2;           /* B2 EEPROM calibration value */
	int16_t   mb;           /* MB EEPROM calibration value */
	int16_t   mc;           /* MC EEPROM calibration value */
	int16_t   md;           /* MD EEPROM calibration value */
	int32_t   b5;           /* B5 temperature compensation value */
	float     temperature;  /* Temperature in degrees */
	float     pressure;     /* Pressure in pascals */
	float     altitude;     /* Calculated altitude at given read pressure */
	uint16_t  delay;        /* Number of microseconds, that sensor needs to calculate data that you request to */
	BMP180_MODE_t oss;      /* Oversampling for pressure calculation */
} BMP180_t;
// ---------------------------------------------------------------------------
/**
 * @brief  Starts BMP180 pressure sensor
 * @param  *BMP180_Data: Pointer to @ref BMP180_t structure
 * @retval Member of @ref BMP180_Result_t
 */
uint8_t BMP180_StartSensor(BMP180_t* BMP180_Data);

/**
 * @brief  Starts temperature sensor on BMP180
 * @param  *BMP180_Data: Pointer to @ref BMP180_t structure
 * @retval Member of @ref BMP180_Result_t
 */
uint8_t BMP180_StartTemperature(BMP180_t* BMP180_Data);

/**
 * @brief  Reads temperature from BMP180 sensor
 * @note   Temperature has 0.1 degrees Celcius resolution
 * @param  *BMP180_Data: Pointer to @ref BMP180_t structure
 * @retval Member of @ref BMP180_Result_t
 */
uint8_t BMP180_ReadTemperature(BMP180_t* BMP180_Data);

/**
 * @brief  Starts pressure measurement on BMP180 sensor
 * @param  *BMP180_Data: Pointer to @ref BMP180_t structure
 * @param  ovs: Oversampling option for pressure calculation.
 *            This parameter can be a value of @ref BMP180_OVS_t enumeration
 * @note   Calculation time depends on selected oversampling
 * @retval Member of @ref BMP180_Result_t
 */
uint8_t BMP180_StartPressure(BMP180_t* BMP180_Data, BMP180_MODE_t oss);

/**
 * @brief  Reads pressure from BMP180 sensor and calculate it
 * @param  *BMP180_Data: Pointer to @ref BMP180_t structure
 * @retval Member of @ref BMP180_Result_t
 */
uint8_t BMP180_ReadPressure(BMP180_t* BMP180_Data);


float BMP180_ConvertPressureToMmHg(float pressure);
/**
 * @brief  Calculates pressure above sea level in pascals
 * 
 * This is good, if you read pressure from sensor at known altitude, not altitude provided from sensor.
 * Altitude from sensor is calculated in fact, that pressure above the sea is 101325 Pascals.
 * So, if you know your pressure, and you use calculated altitude, you will not get real pressure above the sea.
 * 
 * @warning You need calculated pressure from sensor, and known altitude (from other sensor or GPS data, or whatever)
 *          and then you are able to calculate pressure above the sea level.
 * @param  pressure: Pressure at known altitude in units of pascals
 * @param  altitude: Known altitude in units of meters
 * @retval Pressure above the sea in units of pascals
 */
float BMP180_GetPressureAtSeaLevel(float pressure, float altitude);

// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
