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
	// Старт
	// Режим передачи
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// Установить размер данных
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// Очистить адрес ведомого устройства
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// Установить размер данных
	I2C_BUS->CR2 |= 0x80 << I2C_CR2_NBYTES_Pos;
	// Установить адрес ведомого устройства
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// Выдать старт на шину
	I2C_BUS->CR2 |= I2C_CR2_START;
	// Ожидать выдачу старта
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// Если не взлетел NACK-флаг и TXIS-флаг,
	// то микросхема отвечает и готова принимать данные.
 	if(!(I2C_BUS->ISR & I2C_ISR_NACKF) && !(I2C_BUS->ISR & I2C_ISR_TXIS))
 	{
 		err = 1;
 	}
	//-------------------------------------------------
	// Выдать стоп на шину
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// Ожидать выдачу стопа
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	// Очищаю флаги - необходимо для дальнейшей работы шины
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK флаг
	//-------------------------------------------------
	return err;
}
// ---------------------------------------------------------------------------
uint8_t I2C_Read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t count)
{
	uint8_t i = 0; // Счётчик успешно принятых байт
	//-------------------------------------------------
	// Старт
	// Режим передачи
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// Установить размер данных
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// Очистить адрес ведомого устройства
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// Установить размер данных
	I2C_BUS->CR2 |= 1 << I2C_CR2_NBYTES_Pos;
	// Установить адрес ведомого устройства
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// Выдать старт на шину
	I2C_BUS->CR2 |= I2C_CR2_START;
	// Ожидать выдачу старта
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// Сейчас либо I2C запросит первый байт для отправки,
	// Либо взлетит NACK-флаг, говорящий о том, что микросхема не отвечает.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while(!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if(I2C_BUS->ISR & I2C_ISR_TXIS)
		{
			I2C_BUS->TXDR = reg;	                // Отправляю адрес регистра
		}
	}
	//-------------------------------------------------
	// Выдать стоп на шину
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// Ожидать выдачу стопа
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	//-------------------------------------------------;
	// Очистить размер данных
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// Очистить адрес ведомого устройства
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// Режим передачи
	I2C_BUS->CR2 |= I2C_CR2_RD_WRN;
	// Установить размер данных
	I2C_BUS->CR2 |= count << I2C_CR2_NBYTES_Pos;
	// Установить адрес ведомого устройства
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// Выдать старт на шину
	I2C_BUS->CR2 |= I2C_CR2_START;
	// Ожидать выдачу старта
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// Отправляем байты до тех пор, пока не взлетит TC-флаг.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while(!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if (I2C_BUS->ISR & I2C_ISR_RXNE)
		{
			data[i++] = I2C_BUS->RXDR;	// Принимаю данные
		}
	}
	//-------------------------------------------------
	// Выдать стоп на шину
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// Ожидать выдачу стопа
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK флаг
	// Если есть ошибки на шине - очищаю флаги
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
	uint8_t i = 0;	// Счётчик успешно переданных байт
	//-------------------------------------------------
	// Старт
	// Режим передачи
	I2C_BUS->CR2 &= ~I2C_CR2_RD_WRN;
	// Установить размер данных
	I2C_BUS->CR2 &= ~I2C_CR2_NBYTES;
	// Очистить адрес ведомого устройства
	I2C_BUS->CR2 &= ~I2C_CR2_SADD;
	// Установить размер данных
	I2C_BUS->CR2 |= (count + 1) << I2C_CR2_NBYTES_Pos;
	// Установить адрес ведомого устройства
	I2C_BUS->CR2 |= addr << I2C_CR2_SADD_Pos;
	// Выдать старт на шину
	I2C_BUS->CR2 |= I2C_CR2_START;
	// Ожидать выдачу старта
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	//-------------------------------------------------
	// Сейчас либо I2C запросит первый байт для отправки,
	// Либо взлетит NACK-флаг, говорящий о том, что микросхема не отвечает.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while (!(I2C_BUS->ISR & I2C_ISR_TXIS) && !(I2C_BUS->ISR & I2C_ISR_NACKF));
	if (I2C_BUS->ISR & I2C_ISR_TXIS)
	{
		I2C_BUS->TXDR = reg;	        // Отправляю адрес регистра
	}
	//-------------------------------------------------
	// Отправляем байты до тех пор, пока не взлетит TC-флаг.
	// Если взлетит NACK-флаг, отправку прекращаем.
	while (!(I2C_BUS->ISR & I2C_ISR_TC) && !(I2C_BUS->ISR & I2C_ISR_NACKF))
	{
		if (I2C_BUS->ISR & I2C_ISR_TXIS)
		{
			I2C_BUS->TXDR = data[i++];	// Отправляю данные
		}
	}
	//-------------------------------------------------
	// Выдать стоп на шину
	I2C_BUS->CR2 |= I2C_CR2_STOP;
	// Ожидать выдачу стопа
	while(I2C_BUS->ISR & I2C_ISR_BUSY);
	// Очищаю флаги - необходимо для дальнейшей работы шины
	I2C_BUS->ICR |= I2C_ICR_STOPCF;		// STOP флаг
	I2C_BUS->ICR |= I2C_ICR_NACKCF;		// NACK флаг
	// Если есть ошибки на шине - очищаю флаги
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
