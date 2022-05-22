#include "stm32f03x_twi.h"
// ---------------------------------------------------------------------------
static TwoWire_t TwoWireStruct;
// ---------------------------------------------------------------------------
void TwoWire_Init(GPIO_TypeDef* SCL_GPIOx, uint16_t SCL_GPIO_Pin, GPIO_TypeDef* SDA_GPIOx, uint16_t SDA_GPIO_Pin)
{
	/* Init GPIO pin SCL */
	GPIO_Init(SCL_GPIOx, SCL_GPIO_Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
	/* Init GPIO pin SDA */
	GPIO_Init(SDA_GPIOx, SDA_GPIO_Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
	/* Save settings */
	TwoWireStruct.SCL_GPIOx = SCL_GPIOx;
	TwoWireStruct.SCL_GPIO_Pin = SCL_GPIO_Pin;
	TwoWireStruct.SDA_GPIOx = SDA_GPIOx;
	TwoWireStruct.SDA_GPIO_Pin = SDA_GPIO_Pin;
}
// ---------------------------------------------------------------------------
void TwoWire_Start(void)
{
	TwoWire_t* pTwoWire = &TwoWireStruct;
    TWOWIRE_SDA_OUTPUT(pTwoWire);
	TWOWIRE_SDA_HIGH(pTwoWire);
	TWOWIRE_DELAY(TWOWIRE_TIME_US);
	TWOWIRE_SCL_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SDA_LOW(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SCL_LOW(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SDA_INPUT(pTwoWire);
}
// ---------------------------------------------------------------------------
void TwoWire_Stop(void)
{
	TwoWire_t* pTwoWire = &TwoWireStruct;
    TWOWIRE_SDA_OUTPUT(pTwoWire);
    TWOWIRE_SCL_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SDA_LOW(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SDA_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SCL_LOW(pTwoWire);
    TWOWIRE_SDA_INPUT(pTwoWire);
}
// ---------------------------------------------------------------------------
uint8_t TwoWire_WriteByte(uint8_t data)
{
	TwoWire_t* pTwoWire = &TwoWireStruct;
    TWOWIRE_SDA_OUTPUT(pTwoWire);
    for (uint8_t i = 0; i < 8; i++)
    {
        if(data & 0x80)
        	TWOWIRE_SDA_HIGH(pTwoWire);
	    else
	    	TWOWIRE_SDA_LOW(pTwoWire);
	    TWOWIRE_DELAY(TWOWIRE_TIME_US);
        TWOWIRE_SCL_HIGH(pTwoWire);
        TWOWIRE_DELAY(TWOWIRE_TIME_US);
        TWOWIRE_SCL_LOW(pTwoWire);
        data<<=1;
    }
    TWOWIRE_SDA_INPUT(pTwoWire);
    TWOWIRE_SDA_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SCL_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    //uint8_t res = (((pTwoWire-SDA_GPIOx)->IDR & (pTwoWire->SDA_GPIO_Pin)) == 0 ? 1 : 0);
    uint8_t res = !GPIO_GetInputPinValue(pTwoWire->SDA_GPIOx, pTwoWire->SDA_GPIO_Pin);
    TWOWIRE_SCL_LOW(pTwoWire);
	TWOWIRE_SDA_LOW(pTwoWire);
	TWOWIRE_SDA_INPUT(pTwoWire);
    return res;
}
// ---------------------------------------------------------------------------
uint8_t TwoWire_ReadByte(void)
{
    uint8_t data = 0;
    TwoWire_t* pTwoWire = &TwoWireStruct;
    TWOWIRE_SDA_OUTPUT(pTwoWire);
	TWOWIRE_SDA_INPUT(pTwoWire);
    TWOWIRE_SDA_HIGH(pTwoWire);
    for (uint8_t i = 0; i < 8; i++)
    {
        data <<= 1;
		TWOWIRE_SCL_HIGH(pTwoWire);
        TWOWIRE_DELAY(TWOWIRE_TIME_US);
        if ((pTwoWire->SDA_GPIOx)->IDR & (pTwoWire->SDA_GPIO_Pin))
        {
        	data |= 1;
        }
        TWOWIRE_SCL_LOW(pTwoWire);
        TWOWIRE_DELAY(TWOWIRE_TIME_US);
    }
    TWOWIRE_SDA_LOW(pTwoWire);
    TWOWIRE_SDA_OUTPUT(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SCL_HIGH(pTwoWire);
    TWOWIRE_DELAY(TWOWIRE_TIME_US);
    TWOWIRE_SCL_LOW(pTwoWire);
    return data;
}
// ---------------------------------------------------------------------------
uint8_t TwoWire_IsDeviceConnected(uint8_t addr)
{
	TwoWire_Start();
	return TwoWire_WriteByte(addr);
}
// ---------------------------------------------------------------------------
uint8_t TwoWire_Read(uint8_t addr, uint8_t reg, void* data, uint8_t count)
{
	uint8_t err = 0;
	TwoWire_Start();
	err = TwoWire_WriteByte(addr);
	if (err)
	{
		err = TwoWire_WriteByte(reg);
		if (err)
		{
			TwoWire_Start();
			err = TwoWire_WriteByte(addr + 1);
			if (err)
			{
				uint8_t i = 0;
				while (i < count)
				{
					((uint8_t*)data)[i++] = TwoWire_ReadByte();
				}
				TwoWire_Stop();
			}
		}
	}
	return err;
}
// ---------------------------------------------------------------------------
uint8_t TwoWire_Write(uint8_t addr, uint8_t reg, void* data, uint8_t count)
{
	uint8_t err = 0;
	TwoWire_Start();
	err = TwoWire_WriteByte(addr);
	if (err)
	{
		err = TwoWire_WriteByte(reg);
		if (err)
		{
			uint8_t i = 0;
			while (i < count)
			{
				err = TwoWire_WriteByte(((uint8_t*)data)[i++]);
			}
			TwoWire_Stop();
		}
	}
	return err;
}
// ---------------------------------------------------------------------------
