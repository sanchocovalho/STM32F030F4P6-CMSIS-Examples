//----------------------------------------------------------------------------
// ���������� �������������� ������� LM73: lm73.c
//----------------------------------------------------------------------------
// ---------------------------������������ �����------------------------------
#include "stm32f03x_lm73.h"
//-------------------------- ���������� ����������: --------------------------
static LM73_twiaddr_t LM73_twiAddress = 0;
//----------------------------------------------------------------------------
//---------------------------- ������ �������: -------------------------------
uint8_t LM73_Start(LM73_resolution_t resolution)
{
	LM73_twiaddr_t twiAddress;
	uint8_t err = 0;
	LM73_twiaddr_t LM73_device_addr[6] = {LM73_0_I2C_FLOAT,LM73_0_I2C_GND,LM73_0_I2C_VDD,LM73_1_I2C_FLOAT,LM73_1_I2C_GND,LM73_1_I2C_VDD};
	// ���� ����� ������� �� ���� I2C
	for (int8_t i = 0; i < 6; i++)
	{
		twiAddress = LM73_device_addr[i];
		err = TwoWire_IsDeviceConnected(twiAddress);
		if (err)
			break;
	}
	if (err)
	{
		// ��������� ����� ���������� �������
		LM73_twiAddress = twiAddress;
		// �������� ����� �������� ������� �������;
		err = LM73_SetPowerMode(LM73_POWER_OFF);
		if (err)
		{
			if(resolution == 0 ||
			   resolution != LM73_RESOLUTION_11BIT ||
			   resolution != LM73_RESOLUTION_12BIT ||
			   resolution != LM73_RESOLUTION_13BIT ||
			   resolution != LM73_RESOLUTION_14BIT)
			{
				resolution = LM73_RESOLUTION_12BIT;
			}
			// ������������� ���������� ��� �����������
			err = LM73_SetResolution(resolution);
			if (err)
			{
				// �������� ���������� ����� ������
				err = LM73_SetPowerMode(LM73_POWER_ON);
			}
		}
	}
	return err;
}
//---------------------- �������� ����� ����������: --------------------------
LM73_twiaddr_t LM73_GetAddress(void)
{
	return LM73_twiAddress;
}
//------------------- ������ ������ �� �������� �������: ---------------------
uint8_t LM73_Read(uint8_t reg, void* data, uint8_t count)
{
	return TwoWire_Read(LM73_twiAddress, reg, data, count);
}
//-------------------- ������ ������ � ������� �������: ----------------------
uint8_t LM73_Write(uint8_t reg, void* data, uint8_t count)
{
	return TwoWire_Write(LM73_twiAddress, reg, data, count);
}
//---------------- ��������� ������ ����������� �� �������: ------------------
/*
 * ��� ��������� ������� �����������: reg = LM73_REG_TEMPERATURE;
 * ��� ��������� �������� ������ �����������: reg = LM73_REG_THIGH;
 * ��� ��������� ������� ������ �����������: reg = LM73_REG_TLOW.
*/
uint8_t LM73_GetTemperature(double* temperature, uint8_t reg)
{
	uint8_t err;
	uint8_t data[2];
	err = LM73_Read(reg, data, sizeof(data));
	if (err)
		*temperature = ((double)((data[0] << 8) | data[1])) * 0.0078125;
	return err;
}
//------------------- ���������� ����� ������� �������: ----------------------
uint8_t LM73_SetPowerMode(LM73_power_t pwrmode)
{
	uint8_t err;
	uint8_t reg;
	err = LM73_Read(LM73_REG_CONFIG, &reg, sizeof(reg));
	if (err)
	{
		reg = (reg & LM73_MASK_POWER) | pwrmode;
		err = LM73_Write(LM73_REG_CONFIG, &reg, sizeof(reg));
	}
	return err;
}
//---------------- ��������� ���������� ������ �����������: ------------------
uint8_t LM73_SetResolution(LM73_resolution_t resolution)
{
	uint8_t err;
	uint8_t reg;
	err = LM73_Read(LM73_REG_CTRLSTATUS, &reg, sizeof(reg));
	if (err)
	{
		reg = (reg & LM73_MASK_RESOLUTION) | resolution;
		err = LM73_Write(LM73_REG_CTRLSTATUS, &reg, sizeof(reg));
	}
	return err;
}
//----------- ��������� �������� � ������� �������������� ������� ------------
//--------------------------- ������������ �������: --------------------------
// ����������: ������ ��� 11-������ ����������
uint8_t LM73_SetAlert(double* high, double* low)
{
	uint8_t err;
	uint8_t templimit[2];
	*high *= 128.0;
	templimit[0] = ((uint16_t)*high) >> 8;
	templimit[1] = ((uint16_t)*high) & 0xE0;
	err = LM73_Write(LM73_REG_THIGH, templimit, sizeof(templimit));
	*low *= 128.0;
	templimit[0] = ((uint16_t)*low) >> 8;
	templimit[1] = ((uint16_t)*low) & 0xE0;
	err = LM73_Write(LM73_REG_TLOW, templimit, sizeof(templimit));
	return err;
}
//------------------------ ��������� ������ �������: -------------------------
uint8_t LM73_GetAlertStatus(TwoWire_t* TwoWireStruct)
{
	uint8_t err;
	uint8_t reg;
	err = LM73_Read(LM73_REG_CTRLSTATUS, &reg, sizeof(reg));
	if (err)
	{
		if (reg & LM73_BIT_TLOW_FLAG)
			return 1; // �������: ����������� ���������� ���� ������� ������
		else if (reg & LM73_BIT_THIGH_FLAG)
			return 2; // �������: ����������� ��������� ���� �������� ������
		else
			return 3; // ���������� �����������
	}
	return err; // ������
}
//------------------------ ��������� ��������������: -------------------------
uint8_t LM73_GetIdCode(uint8_t* mancode)
{
	return LM73_Read(LM73_REG_ID, &mancode, sizeof(uint16_t));
}
//----------------------------------------------------------------------------
