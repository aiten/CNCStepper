/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) Herbert Aitenbichler

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#pragma once

////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////

class CHAL_I2C_EEprom24C256
{
public:

	static void Init()
	{
		Wire.begin();
	}

#define I2C24C256ADR 0x50

	static byte i2c_eeprom_read_byte(unsigned int eeaddress)
	{
		return i2c_eeprom_read_byte(I2C24C256ADR, eeaddress);
	}

	static uint32_t i2c_eeprom_read_dword(unsigned int eeaddress)
	{
		uint32_t data;
		i2c_eeprom_read_buffer(I2C24C256ADR, eeaddress, (byte*)&data, sizeof(data));
		return data;
	}

	static void i2c_eeprom_write_dword(unsigned int eeaddress, uint32_t value)
	{
		i2c_eeprom_write_page(I2C24C256ADR, eeaddress, (byte*)&value, sizeof(value));
	}

	////////////////////////////////
	// see: http://playground.arduino.cc/Code/I2CEEPROM

	static void i2c_eeprom_write_byte(int deviceaddress, unsigned int eeaddress, byte data)
	{
		int rdata = data;
		Wire.beginTransmission(deviceaddress);
		Wire.write(int(eeaddress >> 8));   // MSB
		Wire.write(int(eeaddress & 0xFF)); // LSB
		Wire.write(rdata);
		Wire.endTransmission();
	}

	// WARNING: address is a page address, 6-bit end will wrap around
	// also, data can be maximum of about 30 bytes, because the Wire library has a buffer of 32 bytes
	static void i2c_eeprom_write_page(int deviceaddress, unsigned int eeaddresspage, byte* data, byte length)
	{
		Wire.beginTransmission(deviceaddress);
		Wire.write(int(eeaddresspage >> 8));   // MSB
		Wire.write(int(eeaddresspage & 0xFF)); // LSB
		for (byte c = 0; c < length; c++)
		{
			Wire.write(data[c]);
		}
		Wire.endTransmission();
	}

	static byte i2c_eeprom_read_byte(int deviceaddress, unsigned int eeaddress)
	{
		byte rdata = 0xFF;
		Wire.beginTransmission(deviceaddress);
		Wire.write(int(eeaddress >> 8));   // MSB
		Wire.write(int(eeaddress & 0xFF)); // LSB
		Wire.endTransmission();
		Wire.requestFrom(deviceaddress, 1);
		if (Wire.available())
		{
			rdata = Wire.read();
		}
		return rdata;
	}

	// maybe let's not read more than 30 or 32 bytes at a time!
	static void i2c_eeprom_read_buffer(int deviceaddress, unsigned int eeaddress, byte* buffer, int length)
	{
		Wire.beginTransmission(deviceaddress);
		Wire.write(int(eeaddress >> 8));   // MSB
		Wire.write(int(eeaddress & 0xFF)); // LSB
		Wire.endTransmission();
		Wire.requestFrom(deviceaddress, length);
		for (int c = 0; c < length; c++)
		{
			if (Wire.available())
			{
				buffer[c] = Wire.read();
			}
		}
	}
};
