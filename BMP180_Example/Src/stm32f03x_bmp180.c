#include "stm32f03x_bmp180.h"
// ---------------------------------------------------------------------------
uint8_t BMP180_StartSensor(BMP180_t* BMP180_Data)
{
	uint8_t err = 0;
	/* Test if device is connected */
	err = I2C_IsDeviceConnected(BMP180_I2C_ADDRESS);
	if(err)
	{
		uint8_t chipid;
		err = I2C_Read(BMP180_I2C_ADDRESS, BMP180_REG_CHIPID, &chipid, sizeof(chipid));
		if(err)
		{
			if(chipid != BMP180_CHIP_ID)
			{
				return 0;
			}
			uint8_t calib_data[22];
			/* Get default values from EEPROM */
			/* EEPROM starts at 0xAA address, read 22 bytes */
			err = I2C_Read(BMP180_I2C_ADDRESS, BMP180_REG_EEPROM, calib_data, sizeof(calib_data));
			if(err)
			{
				/* Set calibration values */
				for(uint8_t i = 0; i < sizeof(calib_data); i += 2)
				{
					((uint16_t*)BMP180_Data)[i >> 1] = (calib_data[i] << 8 | calib_data[i + 1]);
				}
			}
		}
	}
	/* Return 1 if OK or 0 if Error */
	return err;
}
// ---------------------------------------------------------------------------
uint8_t BMP180_StartTemperature(BMP180_t* BMP180_Data)
{
	uint8_t err = 0;
	uint8_t cmd = BMP180_CMD_TEMPERATURE;
	/* Send to device */
	err = I2C_Write(BMP180_I2C_ADDRESS, BMP180_REG_CONTROL, &cmd, sizeof(cmd));
	/* Set minimum delay */
	BMP180_Data->delay = BMP180_TEMPERATURE_DELAY;
	/* Return 1 if OK or 0 if Error */
	return err;
}
// ---------------------------------------------------------------------------
uint8_t BMP180_ReadTemperature(BMP180_t* BMP180_Data)
{
	uint8_t err = 0;
	uint8_t temp_data[2];
	/* Read 2 bytes from I2C */
	err = I2C_Read(BMP180_I2C_ADDRESS, BMP180_REG_RESULT, temp_data, sizeof(temp_data));
	if(err)
	{
		/* Get uncompensated temperature */
		uint16_t untemp = temp_data[0] << 8 | temp_data[1];
		/* Calculate true temperature */
		int32_t x1 = ((untemp - BMP180_Data->ac6) * BMP180_Data->ac5) >> 15;
		int32_t x2 = (BMP180_Data->mc << 11) / (x1 + BMP180_Data->md);
		BMP180_Data->b5 = x1 + x2;
		/* Get temperature in degrees */
		BMP180_Data->temperature = ((float)(BMP180_Data->b5) + 8.0f) / 160.0f;
	}
	/* Return 1 if OK or 0 if Error */
	return err;
}
// ---------------------------------------------------------------------------
uint8_t BMP180_StartPressure(BMP180_t* BMP180_Data, BMP180_MODE_t oss)
{
	uint8_t err = 0;
	uint8_t cmd;
	switch (oss)
	{
		case BMP180_MODE_ULTRA_LOW_POWER :
			cmd = BMP180_CMD_PRESSURE_0;
			BMP180_Data->delay = BMP180_PRESSURE_0_DELAY;
			break;
		case BMP180_MODE_STANDARD:
			cmd = BMP180_CMD_PRESSURE_1;
			BMP180_Data->delay = BMP180_PRESSURE_1_DELAY;
			break;
		case BMP180_MODE_HIGH_RESOLUTION:
			cmd = BMP180_CMD_PRESSURE_2;
			BMP180_Data->delay = BMP180_PRESSURE_2_DELAY;
			break;
		case BMP180_MODE_ULTRA_HIGH_RESOLUTION:
			cmd = BMP180_CMD_PRESSURE_3;
			BMP180_Data->delay = BMP180_PRESSURE_3_DELAY;
			break;
		default:
			cmd = BMP180_CMD_PRESSURE_0;
			BMP180_Data->delay = BMP180_PRESSURE_0_DELAY;
			break;
	}
	/* Send to device */
	err = I2C_Write(BMP180_I2C_ADDRESS, BMP180_REG_CONTROL, &cmd, sizeof(cmd));
	/* Save selected oversampling */
	BMP180_Data->oss = oss;
	/* Return 1 if OK or 0 if Error */
	return err;
}
// ---------------------------------------------------------------------------
uint8_t BMP180_ReadPressure(BMP180_t* BMP180_Data)
{
	uint8_t err = 0;
	uint8_t press_data[3];
	/* Read 3 bytes from I2C */
	err = I2C_Read(BMP180_I2C_ADDRESS, BMP180_REG_RESULT, press_data, sizeof(press_data));
	if(err)
	{
		/* Get uncompensated pressure */
		int32_t unpressure = (press_data[0] << 16 | press_data[1] << 8 | press_data[2]) >> (8 - (uint8_t)BMP180_Data->oss);
		int32_t pressure;
		/* Calculate true pressure */
		int32_t b6 = BMP180_Data->b5 - 4000;
		int32_t x1 = (BMP180_Data->b2 * ((b6 * b6) >> 12)) >> 11;
		int32_t x2 = (BMP180_Data->ac2 * b6) >> 11;
		int32_t x3 = x1 + x2;
		int32_t b3 = ((((int32_t)(BMP180_Data->ac1) * 4 + x3) << (uint8_t)BMP180_Data->oss) + 2) >> 2;
		x1 = ((int32_t)(BMP180_Data->ac3) * b6) >> 13;
		x2 = ((int32_t)(BMP180_Data->b1) * ((b6 * b6) >> 12)) >> 16;
		x3 = ((x1 + x2) + 2) >> 2;
		uint32_t b4 = ((uint32_t)(BMP180_Data->ac4) * (uint32_t)(x3 + 32768)) >> 15;
		uint32_t b7 = ((uint32_t)unpressure - b3) * (50000 >> (uint8_t)BMP180_Data->oss);
		if (b7 < 0x80000000)
		{
			pressure = (b7 * 2) / b4;
		}
		else
		{
			pressure = (b7 / b4) * 2;
		}
		x1 = (pressure >> 8) * (pressure >> 8);
		x1 = (x1 * 3038) >> 16;
		x2 = (-7357 * pressure) >> 16;
		/* Save pressure */
		BMP180_Data->pressure = (float)(pressure + ((x1 + x2 + 3791) >> 4));
		/* Calculate altitude */
		//BMP180_Data->altitude = 44330.0f * (1.0f - powf((BMP180_Data->pressure) * 0.00000986923266726f, 0.1902949571836346f));
		BMP180_Data->altitude = 44330.0f * (1.0f - expf(0.1902949571836346f * logf(BMP180_Data->pressure * 0.00000986923266726f)));
	}
	return err;
}
// ---------------------------------------------------------------------------
float BMP180_ConvertPressureToMmHg(float pressure)
{
	return pressure * 0.007500616827f;
}
// ---------------------------------------------------------------------------
float BMP180_GetPressureAtSeaLevel(float pressure, float altitude)
{
	//return pressure / (powf(1.0f - altitude / 44330.0f, 5.255f));
	return pressure / (expf(5.255f * logf(1.0f - altitude * 0.000022558087074216f)));
}
// ---------------------------------------------------------------------------
