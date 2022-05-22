#include "stm32f03x_onewire.h"
//*************************************************************************************
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	/* Init GPIO pin */
	GPIO_Init(GPIOx, GPIO_Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
	/* Save settings */
	OneWireStruct->GPIOx = GPIOx;
	OneWireStruct->GPIO_Pin = GPIO_Pin;
}
//*************************************************************************************
uint8_t OneWire_Reset(OneWire_t* OneWireStruct)
{
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(500);
	/* Release line and wait for 70us */
	ONEWIRE_HIGH(OneWireStruct);
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(70);
	/* Check bit value */
	//uint8_t res = (((OneWireStruct->GPIOx)->IDR & (OneWireStruct->GPIO_Pin)) == 0 ? 0 : 1);
	uint8_t res = GPIO_GetInputPinValue(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin);
	/* Delay for 410 us */
	ONEWIRE_DELAY(410);
	/* Return value of presence pulse, 0 = OK, 1 = ERROR */
	return res;
}
//*************************************************************************************
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit)
{
	if (bit)
	{
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(10);
		/* Bit high */
		ONEWIRE_HIGH(OneWireStruct);
		/* Wait for 55 us and release the line */
		ONEWIRE_DELAY(55);
		ONEWIRE_INPUT(OneWireStruct);
	}
	else
	{
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(65);
		/* Bit high */
		ONEWIRE_HIGH(OneWireStruct);
		/* Wait for 5 us and release the line */
		ONEWIRE_DELAY(5);
		ONEWIRE_INPUT(OneWireStruct);
	}
}
//*************************************************************************************
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct)
{
	//uint8_t bit = 0;
	/* Line low */
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(2);
	/* Release line */
	ONEWIRE_HIGH(OneWireStruct);
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(10);
	/* Read line value */
	//uint8_t bit = (((OneWireStruct->GPIOx)->IDR & (OneWireStruct->GPIO_Pin)) == 0 ? 0 : 1);
	uint8_t bit = GPIO_GetInputPinValue(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin);
	/* Wait 50us to complete 60us period */
	ONEWIRE_DELAY(50);
	/* Return bit value */
	return bit;
}
//*************************************************************************************
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte)
{
	uint8_t i = 8;
	/* Write 8 bits */
	while (i--)
	{
		/* LSB bit is first */
		OneWire_WriteBit(OneWireStruct, byte & 0x01);
		byte >>= 1;
	}
}
//*************************************************************************************
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct)
{
	uint8_t i = 8, byte = 0;
	while (i--)
	{
		byte >>= 1;
		byte |= (OneWire_ReadBit(OneWireStruct) << 7);
	}
	return byte;
}
//*************************************************************************************
void OneWire_WriteBytes(OneWire_t* OneWireStruct, uint8_t *buf, uint16_t count)
{
	for (uint16_t i = 0; i < count; i++)
		OneWire_WriteByte(OneWireStruct, buf[i]);
}
//*************************************************************************************
void OneWire_ReadBytes(OneWire_t* OneWireStruct, uint8_t *buf, uint16_t count)
{
	for (uint16_t i = 0; i < count; i++)
		buf[i] = OneWire_ReadByte(OneWireStruct);
}
//*************************************************************************************
uint8_t OneWire_First(OneWire_t* OneWireStruct)
{
	/* Reset search values */
	OneWire_ResetSearch(OneWireStruct);
	/* Start with searching */
	return OneWire_Search(OneWireStruct);
}
//*************************************************************************************
uint8_t OneWire_Next(OneWire_t* OneWireStruct)
{
   /* Leave the search state alone */
   return OneWire_Search(OneWireStruct);
}
//*************************************************************************************
void OneWire_ResetSearch(OneWire_t* OneWireStruct)
{
	/* Reset the search state */
	OneWireStruct->LastDiscrepancy = 0;
	OneWireStruct->LastDeviceFlag = 0;
	OneWireStruct->LastFamilyDiscrepancy = 0;
	for(int8_t i = 0; i < 8; i++)
	{
		OneWireStruct->ROM_NO[i] = 0;
	}
}
//*************************************************************************************
uint8_t OneWire_Search(OneWire_t* OneWireStruct)
{
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit;
	uint8_t rom_byte_mask, search_direction;
	/* Initialize for search */
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;
	// if the last call was not the last one
	if (!OneWireStruct->LastDeviceFlag)
	{
		// 1-Wire reset
		if (OneWire_Reset(OneWireStruct))
		{
			/* Reset the search */
			OneWireStruct->LastDiscrepancy = 0;
			OneWireStruct->LastDeviceFlag = 0;
			OneWireStruct->LastFamilyDiscrepancy = 0;
			return 0;
		}
		// issue the search command
		OneWire_WriteByte(OneWireStruct, ONEWIRE_CMD_SEARCHROM);
		// loop to do the search
		do
		{
			// read a bit and its complement
			id_bit = OneWire_ReadBit(OneWireStruct);
			cmp_id_bit = OneWire_ReadBit(OneWireStruct);
			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1))
			{
				break;
			}
			else
			{
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit)
				{
					search_direction = id_bit;  // bit write value for search
				}
				else
				{
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (id_bit_number < OneWireStruct->LastDiscrepancy)
					{
						search_direction = ((OneWireStruct->ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					}
					else
					{
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == OneWireStruct->LastDiscrepancy);
					}
					
					// if 0 was picked then record its position in LastZero
					if (search_direction == 0)
					{
						last_zero = id_bit_number;
						// check for Last discrepancy in family
						if (last_zero < 9)
						{
							OneWireStruct->LastFamilyDiscrepancy = last_zero;
						}
					}
				}
				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction == 1)
				{
					OneWireStruct->ROM_NO[rom_byte_number] |= rom_byte_mask;
				}
				else
				{
					OneWireStruct->ROM_NO[rom_byte_number] &= ~rom_byte_mask;
				}
				// serial number search direction write bit
				OneWire_WriteBit(OneWireStruct, search_direction);
				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;
				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0)
				{
					//docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		}while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7
		// if the search was successful then
		if (!(id_bit_number < 65))
		{
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			OneWireStruct->LastDiscrepancy = last_zero;
			// check for last device
			if (OneWireStruct->LastDiscrepancy == 0)
			{
				OneWireStruct->LastDeviceFlag = 1;
			}
			search_result = 1;
		}
	}
	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !OneWireStruct->ROM_NO[0])
	{
		OneWireStruct->LastDiscrepancy = 0;
		OneWireStruct->LastDeviceFlag = 0;
		OneWireStruct->LastFamilyDiscrepancy = 0;
		search_result = 0;
	}
	return search_result;
}
//*************************************************************************************
int OneWire_Verify(OneWire_t* OneWireStruct)
{
	unsigned char rom_backup[8];
	int i,rslt,ld_backup,ldf_backup,lfd_backup;
	// keep a backup copy of the current state
	for (i = 0; i < 8; i++)
	rom_backup[i] = OneWireStruct->ROM_NO[i];
	ld_backup = OneWireStruct->LastDiscrepancy;
	ldf_backup = OneWireStruct->LastDeviceFlag;
	lfd_backup = OneWireStruct->LastFamilyDiscrepancy;
	// set search to find the same device
	OneWireStruct->LastDiscrepancy = 64;
	OneWireStruct->LastDeviceFlag = 0;
	if (OneWire_Search(OneWireStruct))
	{
		// check if same device found
		rslt = 1;
		for (i = 0; i < 8; i++)
		{
			if (rom_backup[i] != OneWireStruct->ROM_NO[i])
			{
				rslt = 1;
				break;
			}
		}
	}
	else
	{
		rslt = 0;
	}
	// restore the search state 
	for (i = 0; i < 8; i++)
	{
		OneWireStruct->ROM_NO[i] = rom_backup[i];
	}
	OneWireStruct->LastDiscrepancy = ld_backup;
	OneWireStruct->LastDeviceFlag = ldf_backup;
	OneWireStruct->LastFamilyDiscrepancy = lfd_backup;
	// return the result of the verify
	return rslt;
}
//*************************************************************************************
void OneWire_TargetSetup(OneWire_t* OneWireStruct, uint8_t family_code)
{
   uint8_t i;
	// set the search state to find SearchFamily type devices
	OneWireStruct->ROM_NO[0] = family_code;
	for (i = 1; i < 8; i++)
	{
		OneWireStruct->ROM_NO[i] = 0;
	}
	OneWireStruct->LastDiscrepancy = 64;
	OneWireStruct->LastFamilyDiscrepancy = 0;
	OneWireStruct->LastDeviceFlag = 0;
}
//*************************************************************************************
void OneWire_FamilySkipSetup(OneWire_t* OneWireStruct)
{
	// set the Last discrepancy to last family discrepancy
	OneWireStruct->LastDiscrepancy = OneWireStruct->LastFamilyDiscrepancy;
	OneWireStruct->LastFamilyDiscrepancy = 0;
	// check for end of list
	if (OneWireStruct->LastDiscrepancy == 0)
	{
		OneWireStruct->LastDeviceFlag = 1;
	}
}
//*************************************************************************************
uint8_t OneWire_GetROM(OneWire_t* OneWireStruct, uint8_t index)
{
	return OneWireStruct->ROM_NO[index];
}
//*************************************************************************************
void OneWire_Select(OneWire_t* OneWireStruct, uint8_t* addr)
{
	uint8_t i;
	OneWire_WriteByte(OneWireStruct, ONEWIRE_CMD_MATCHROM);
	for (i = 0; i < 8; i++)
	{
		OneWire_WriteByte(OneWireStruct, addr[i]);
	}
}
//*************************************************************************************
void OneWire_SelectWithPointer(OneWire_t* OneWireStruct, uint8_t *ROM)
{
	uint8_t i;
	OneWire_WriteByte(OneWireStruct, ONEWIRE_CMD_MATCHROM);
	for (i = 0; i < 8; i++)
	{
		OneWire_WriteByte(OneWireStruct, ROM[i]);
	}
}
//*************************************************************************************
void OneWire_GetFullROM(OneWire_t* OneWireStruct, uint8_t *firstIndex)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		firstIndex[i] = OneWireStruct->ROM_NO[i];
	}
}
//*************************************************************************************
uint8_t OneWire_CRC8(uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;
	while (len--)
	{
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--)
		{
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
			{
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}
	/* Return calculated CRC */
	return crc;
}
//*************************************************************************************
uint16_t OneWire_CRC16(uint8_t* input, uint16_t len, uint16_t crc)
{
    static const uint8_t oddparity[16] =
        { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
    for (uint16_t i = 0 ; i < len ; i++) {
      uint16_t cdata = input[i];
      cdata = (cdata ^ crc) & 0xff;
      crc >>= 8;
      if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
          crc ^= 0xC001;
      cdata <<= 6;
      crc ^= cdata;
      cdata <<= 1;
      crc ^= cdata;
    }
    return crc;
}
//*************************************************************************************
uint8_t OneWire_CheckCRC16(uint8_t* input, uint16_t len, uint8_t* inverted_crc, uint16_t crc)
{
    crc = ~OneWire_CRC16(input, len, crc);
    return (uint8_t)((crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1]);
}
//*************************************************************************************
