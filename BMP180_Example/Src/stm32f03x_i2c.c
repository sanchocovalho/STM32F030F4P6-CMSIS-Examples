#include "stm32f03x_i2c.h"
// ---------------------------------------------------------------------------
void I2C_Init(GPIO_TypeDef* GPIOx, uint16_t SCL_GPIO_Pin, uint16_t SDA_GPIO_Pin)
{
	GPIO_InitAlternate(GPIOx, SCL_GPIO_Pin, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_Medium, GPIO_AF_I2C1);
	GPIO_InitAlternate(GPIOx, SDA_GPIO_Pin, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_Medium, GPIO_AF_I2C1);
	// Enable the peripheral clock I2C1
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	// 7-bit addressing mode
	I2C_BUS->CR2 &= ~(I2C_CR2_ADD10 | I2C_CR2_HEAD10R);
#ifdef I2C_400KHZ
	// I2C has 48MHz clocking frequency
	// and 400kHz bus frequency
	// Set timings
	I2C_BUS->TIMINGR |=(0x0 << I2C_TIMINGR_PRESC_Pos); 	//400 kHz - I2C bus speed
	I2C_BUS->TIMINGR |=(0x9 << I2C_TIMINGR_SCLL_Pos);
	I2C_BUS->TIMINGR |=(0x3	<< I2C_TIMINGR_SCLH_Pos);
	I2C_BUS->TIMINGR |=(0x1	<< I2C_TIMINGR_SDADEL_Pos);
	I2C_BUS->TIMINGR |=(0x3	<< I2C_TIMINGR_SCLDEL_Pos);
#else
	// I2C has 48MHz clocking frequency
	// and 100kHz bus frequency
	// Set timings
	I2C_BUS->TIMINGR |=(0x1	<< I2C_TIMINGR_PRESC_Pos); 	//100 kHz - I2C bus speed
	I2C_BUS->TIMINGR |=(0x13<< I2C_TIMINGR_SCLL_Pos);
	I2C_BUS->TIMINGR |=(0xF	<< I2C_TIMINGR_SCLH_Pos);
	I2C_BUS->TIMINGR |=(0x2	<< I2C_TIMINGR_SDADEL_Pos);
	I2C_BUS->TIMINGR |=(0x4	<< I2C_TIMINGR_SCLDEL_Pos);
#endif
	// Enable I2C
	I2C_BUS->CR1 |= I2C_CR1_PE;
	// Wait enabling I2C
	while(!(I2C_BUS->CR1 & I2C_CR1_PE));
}
// ---------------------------------------------------------------------------
uint8_t I2C_IsDeviceConnected(uint8_t addr)
{
	uint8_t err = 0;
	//-------------------------------------------------
	// �����
	// ����� ��������
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// ���������� ������ ������
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// �������� ����� �������� ����������
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// ���������� ������ ������
	I2C_BUS->CR2 |= 0x80 << I2C_CR2_NBYTES_Pos;
	// ���������� ����� �������� ����������
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// ������ ����� �� ����
	I2C_BUS->CR2 |= I2C_CR2_START;
	// ������� ������ ������
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// ���� �� ������� NACK-���� � TXIS-����,
	// �� ���������� �������� � ������ ��������� ������.
 	if(!(I2C_BUS->ISR & I2C_ISR_NACKF) && !(I2C_BUS->ISR & I2C_ISR_TXIS))
 	{
 		err = 1;
 	}
	//-------------------------------------------------
	// ������ ���� �� ����
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// ������� ������ �����
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	// ������ ����� - ���������� ��� ���������� ������ ����
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP ����
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK ����
	//-------------------------------------------------
	return err;
}
// ---------------------------------------------------------------------------
uint8_t I2C_Read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t count)
{
	uint8_t i = 0; // ������� ������� �������� ����
	//-------------------------------------------------
	// �����
	// ����� ��������
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// ���������� ������ ������
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// �������� ����� �������� ����������
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// ���������� ������ ������
	I2C_BUS->CR2 |= 1 << I2C_CR2_NBYTES_Pos;
	// ���������� ����� �������� ����������
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// ������ ����� �� ����
	I2C_BUS->CR2 |= I2C_CR2_START;
	// ������� ������ ������
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// ������ ���� I2C �������� ������ ���� ��� ��������,
	// ���� ������� NACK-����, ��������� � ���, ��� ���������� �� ��������.
	// ���� ������� NACK-����, �������� ����������.
	while(!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if(I2C_BUS->ISR & I2C_ISR_TXIS)
		{
			I2C_BUS->TXDR = reg;	                // ��������� ����� ��������
		}
	}
	//-------------------------------------------------
	// ������ ���� �� ����
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// ������� ������ �����
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP ����
	//-------------------------------------------------;
	// �������� ������ ������
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// �������� ����� �������� ����������
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// ����� ��������
	I2C_BUS->CR2 |= I2C_CR2_RD_WRN;
	// ���������� ������ ������
	I2C_BUS->CR2 |= count << I2C_CR2_NBYTES_Pos;
	// ���������� ����� �������� ����������
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// ������ ����� �� ����
	I2C_BUS->CR2 |= I2C_CR2_START;
	// ������� ������ ������
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// ���������� ����� �� ��� ���, ���� �� ������� TC-����.
	// ���� ������� NACK-����, �������� ����������.
	while(!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if (I2C_BUS->ISR & I2C_ISR_RXNE)
		{
			data[i++] = I2C_BUS->RXDR;	// �������� ������
		}
	}
	//-------------------------------------------------
	// ������ ���� �� ����
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// ������� ������ �����
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP ����
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK ����
	// ���� ���� ������ �� ���� - ������ �����
	if (I2C_BUS->ISR & (I2C_ISR_ARLO | I2C_ISR_BERR))
	{
		I2C_BUS->ICR |= I2C_ICR_ARLOCF;
		I2C_BUS->ICR |= I2C_ICR_BERRCF;
	}
	I2C_BUS->CR2 &=(~I2C_CR2_RD_WRN) &(~I2C_CR2_NACK);
	//-------------------------------------------------
	if(i == count)
		return 1;
	return 0;
}
// ---------------------------------------------------------------------------
uint8_t I2C_Write(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t count)
{
	uint8_t i = 0;	// ������� ������� ���������� ����
	//-------------------------------------------------
	// �����
	// ����� ��������
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// ���������� ������ ������
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// �������� ����� �������� ����������
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// ���������� ������ ������
	I2C_BUS->CR2 |= (count + 1) << I2C_CR2_NBYTES_Pos;
	// ���������� ����� �������� ����������
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// ������ ����� �� ����
	I2C_BUS->CR2 |= I2C_CR2_START;
	// ������� ������ ������
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// ������ ���� I2C �������� ������ ���� ��� ��������,
	// ���� ������� NACK-����, ��������� � ���, ��� ���������� �� ��������.
	// ���� ������� NACK-����, �������� ����������.
	while (!(I2C_BUS->ISR & I2C_ISR_TXIS) && !(I2C_BUS->ISR & I2C_ISR_NACKF));
	if (I2C_BUS->ISR & I2C_ISR_TXIS)
	{
		I2C_BUS->TXDR = reg;	        // ��������� ����� ��������
	}
	//-------------------------------------------------
	// ���������� ����� �� ��� ���, ���� �� ������� TC-����.
	// ���� ������� NACK-����, �������� ����������.
	while (!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if (I2C_BUS->ISR & I2C_ISR_TXIS)
		{
			I2C_BUS->TXDR = data[i++];	// ��������� ������
		}
	}
	//-------------------------------------------------
	// ������ ���� �� ����
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// ������� ������ �����
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	// ������ ����� - ���������� ��� ���������� ������ ����
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP ����
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK ����
	// ���� ���� ������ �� ���� - ������ �����
	if (I2C_BUS->ISR & (I2C_ISR_ARLO | I2C_ISR_BERR))
	{
		I2C_BUS->ICR |= I2C_ICR_ARLOCF;
		I2C_BUS->ICR |= I2C_ICR_BERRCF;
	}
	//-------------------------------------------------
	if(i == count)
		return 1;
	return 0;
}
// ---------------------------------------------------------------------------
