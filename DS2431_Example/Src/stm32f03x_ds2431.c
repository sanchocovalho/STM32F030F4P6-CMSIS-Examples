#include "stm32f03x_ds2431.h"
//*************************************************************************************
uint8_t Is_DS2431(uint8_t *ROM)
{
	/* Checks if first byte is equal to DS2431's family code */
	if (*ROM == DS2431_FAMILY_CODE)
	{
		return 1;
	}
	/* Return error value, 1 = OK, 0 = ERROR */
	return 0;
}
//*************************************************************************************
uint8_t DS2431_Start(OneWire_t* OneWireStruct, uint8_t *ROM)
{
	uint8_t err = 0;
	/* Check if device is DS2431 */
	if (!Is_DS2431(ROM))
	{
		return err;
	}
	err = !OneWire_Reset(OneWireStruct);
	if (err)
	{
		OneWire_Select(OneWireStruct, ROM);
	}
	/* Return error value, 1 = OK, 0 = ERROR */
	return err;
}
//*************************************************************************************
uint8_t DS2431_Read(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf, uint16_t count)
{
	uint8_t err = 0;
	if (address + count > DS2431_EEPROM_SIZE)
	{
		count = DS2431_EEPROM_SIZE - address;
	}
	err = DS2431_Start(OneWireStruct, ROM);
	if (err)
	{
		OneWire_WriteByte(OneWireStruct, DS2431_READ_MEMORY);
		OneWire_WriteByte(OneWireStruct, address & 0xFF);
		OneWire_WriteByte(OneWireStruct, address >> 8);
		for (uint16_t i = 0; i < count; i++)
		{
			buf[i] = OneWire_ReadByte(OneWireStruct);
		}
	}
	/* Return error value, 1 = OK, 0 = ERROR */
	return err;
}
//*************************************************************************************
uint8_t DS2431_WriteAlignedRow(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf)
{
	uint8_t verify = 0;
	uint8_t error_count = 0;
	uint8_t buffer[DS2431_BUFFER_SIZE_8];
	uint8_t crc16[DS2431_CRC_SIZE];
	/* Check if device is DS2431 */
	if (!Is_DS2431(ROM))
	{
		return 0;
	}
	/* Address + 8-byte have not to be exceed the eeprom size.
	   Address has to be aligned on an 8-byte boundary. */
	if ((address + DS2431_ROW_SIZE > DS2431_EEPROM_SIZE) || (address & 0xFFF8))
	{
		return 0;
	}
	/* Prepare buffer data */
	buffer[0] = DS2431_WRITE_SCRATCHPAD;
	buffer[1] = (uint8_t)(address & 0xFF);
	buffer[2] = (uint8_t)(address >> 4);
	for(uint8_t i = 0; i < DS2431_ROW_SIZE; i++)
	{
		buffer[i + DS2431_CMD_SIZE] = buf[i];
	}
	/*  Write scratchpad with CRC check */
	if(!(DS2431_Start(OneWireStruct, ROM)))
	{
		return 0;
	}
	OneWire_WriteBytes(OneWireStruct, buffer, DS2431_CMD_SIZE + DS2431_ROW_SIZE); // Write CMD + LSB Adr + MSB Adr
	OneWire_ReadBytes(OneWireStruct, crc16, DS2431_CRC_SIZE);           //Read CRC-16
	if (!OneWire_CheckCRC16(buffer, DS2431_CMD_SIZE + DS2431_ROW_SIZE, crc16, 0))
	{
		verify = 1; // CRC not matching, try to read again
	}
	/*  Read verification. Prepare buffer data */
	buffer[0] = DS2431_READ_SCRATCHPAD;
	do
	{
		/* Read scratchpad to compare with the data sent */
		if(!(DS2431_Start(OneWireStruct, ROM)))
		{
			return 0;
		}
		OneWire_WriteByte(OneWireStruct, buffer[0]);                   // Write CMD
		OneWire_ReadBytes(OneWireStruct, &buffer[1], DS2431_CMD_SIZE); //Read TA1, TA2, E/S, scratchpad
		if (buffer[3] != DS2431_PF_MASK)
		{
			verify = 1;
		}
		if(verify)
		{
			OneWire_ReadBytes(OneWireStruct, &buffer[4], DS2431_ROW_SIZE);       //Read scratchpad
			OneWire_ReadBytes(OneWireStruct, crc16, DS2431_CRC_SIZE);  //Read CRC-16
			if (!OneWire_CheckCRC16(buffer, DS2431_CMD_SIZE + DS2431_ROW_SIZE + 1, crc16, 0))
			{
				error_count++; //CRC not matching.
				continue;
			}
			if (address != ((buffer[2] << 8) + buffer[1]))
			{
				return 0; //Address not matching
			}
			if (buffer[3] != DS2431_PF_MASK)
			{
				return 0; //Invalid transfer or data already copied (wrong value for E/S).
			}
			for(uint8_t i = 0; i < DS2431_ROW_SIZE; i++)
			{
				if(buffer[i + DS2431_CMD_SIZE + 1] != buf[i])
				{
					return 0; //Data in the scratchpad is invalid.
				}
			}
		}
		break;
	} while(error_count < DS2431_READ_RETRY);
	/*  Prepare buffer data */
	buffer[0] = DS2431_COPY_SCRATCHPAD;
	//Copy scratchpad
	if(!(DS2431_Start(OneWireStruct, ROM)))
	{
		return 0;
	}
	/* Send authorization code (TA1, TA2, E/S) */
	OneWire_WriteBytes(OneWireStruct, buffer, DS2431_CMD_SIZE + 1);
	DS2431_DELAY(15); // t_PROG = 12.5ms worst case.
	uint8_t res = OneWire_ReadByte(OneWireStruct);
	if (res != DS2431_WRITE_MASK)
	{
		return 0;
	}
	/* Return error value, 1 = OK, 0 = ERROR */
	return 1;
}
//*************************************************************************************
uint8_t DS2431_Write(OneWire_t* OneWireStruct, uint8_t *ROM, uint16_t address, uint8_t *buf, uint16_t count)
{
	uint8_t buffer[DS2431_ROW_SIZE];
	uint16_t aligned_addr;
	uint16_t offset;
	uint16_t size;
	if (address + count > DS2431_EEPROM_SIZE)
	{
		count = DS2431_EEPROM_SIZE - address;
	}
	uint16_t k = 0;
	while(count)
	{
		aligned_addr = address & 0xFFF8;
		if(!DS2431_Read(OneWireStruct, ROM, aligned_addr, buffer, DS2431_ROW_SIZE))
		{
			return 0;
		}
		offset = address - aligned_addr;
		size = count + offset;
		if(count > DS2431_ROW_SIZE || size > DS2431_ROW_SIZE)
		{
			size = (aligned_addr + DS2431_ROW_SIZE) - address;
		}
		else
		{
			size = count;
		}
		for(uint16_t j = 0; j < size; j++)
		{
			buffer[j + offset] = buf[k++];
		}
		if(!DS2431_WriteAlignedRow(OneWireStruct, ROM, aligned_addr, buffer))
		{
			return 0;
		}
		address += size;
		count -= size;
	}
	return 1;
}
//*************************************************************************************
uint8_t DS2431_EraseFullMemory(OneWire_t* OneWireStruct, uint8_t *ROM)
{
	uint8_t err = 0;
	uint8_t buffer[DS2431_ROW_SIZE];
	for(uint8_t i = 0 ; i < DS2431_ROW_SIZE; i++)
	{
		buffer[i] = 0xFF;
	}
    for(uint8_t j = 0 ; j < 16; j++)
    {
    	err = DS2431_WriteAlignedRow(OneWireStruct, ROM, (uint16_t)(j * DS2431_ROW_SIZE), buffer);
    }
    return err;
}
//*************************************************************************************
