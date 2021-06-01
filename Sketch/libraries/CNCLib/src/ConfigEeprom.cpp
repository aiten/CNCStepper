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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <StepperLib.h>

#include "Parser.h"
#include "ConfigEeprom.h"

////////////////////////////////////////////////////////////

template <> CConfigEeprom* CSingleton<CConfigEeprom>::_instance = nullptr;

////////////////////////////////////////////////////////////

inline const void* AddAdr(const void* adr, eepromofs_t ofs)
{
	return ((uint8_t*)adr) + ofs;
}

////////////////////////////////////////////////////////////

void CConfigEeprom::Init(uint16_t eepromSize, const void* defaultEeprom, uint32_t eepromID)
{
	CHAL::InitEeprom();

	_eepromSize    = eepromSize;
	_defaultEeprom = defaultEeprom;

	if (CHAL::HaveEeprom())
	{
		_eepromValid = CHAL::eeprom_read_dword((uint32_t*)AddAdr(CHAL::GetEepromBaseAdr(), offsetof(SCNCEeprom, Signature))) == eepromID;
	}
	else
	{
		_eepromValid = false;;
	}
}

////////////////////////////////////////////////////////////

void CConfigEeprom::Flush()
{
	CHAL::FlushEeprom();
}

////////////////////////////////////////////////////////////

float CConfigEeprom::GetConfigFloat(eepromofs_t ofs)
{
	union
	{
		uint32_t u;
		float    f;
	}            v;

	v.u = GetInstance()->GetConfig32(ofs);
	return v.f;
}

uint32_t CConfigEeprom::GetConfigU32(eepromofs_t ofs)
{
	return GetInstance()->GetConfig32(ofs);
};

////////////////////////////////////////////////////////////

uint32_t CConfigEeprom::GetConfig32(eepromofs_t ofs) const
{
	// read info1 & info2 from FLASH, never from EEPROM
	
	if (_eepromValid && ofs >= offsetof(SCNCEeprom, StepperDirections))
	{
		return CHAL::eeprom_read_dword((uint32_t*)AddAdr(CHAL::GetEepromBaseAdr(), ofs));
	}
	return pgm_read_dword((uint32_t*)AddAdr(_defaultEeprom, ofs));
}

////////////////////////////////////////////////////////////

void CConfigEeprom::SetConfig32(eepromofs_t ofs, uint32_t value)
{
	CHAL::eeprom_write_dword((uint32_t*)AddAdr(CHAL::GetEepromBaseAdr(), ofs), value);
}

////////////////////////////////////////////////////////////

void CConfigEeprom::FlushConfig()
{
	for (eepromofs_t ofs = 0; ofs < _eepromSize; ofs += sizeof(uint32_t))
	{
		SetConfig32(ofs, GetConfig32(ofs));
	}
	_eepromValid = true;
}

////////////////////////////////////////////////////////////

void CConfigEeprom::PrintConfig()
{
	for (eepromofs_t ofs = 0; ofs < _eepromSize; ofs += sizeof(uint32_t))
	{
		uint32_t val = GetConfig32(ofs);
		StepperSerial.print('$');
		StepperSerial.print(ofs / sizeof(uint32_t));
		StepperSerial.print('=');
		StepperSerial.print(val);
#ifndef REDUCED_SIZE
		StepperSerial.print('(');
		StepperSerial.print(val, HEX);
		StepperSerial.print(')');
#endif
		StepperSerial.println();
	}
}

////////////////////////////////////////////////////////////

bool CConfigEeprom::ParseConfig(CParser* parser)
{
	switch (parser->GetReader()->SkipSpaces())
	{
		case '?':
		{
			PrintConfig();
			parser->GetReader()->GetNextChar();
			return true;
		}
		case '!':
		{
			if (!CHAL::HaveEeprom())
			{
				return false;
			}
			_eepromCanWrite = true;
			parser->GetReader()->GetNextChar();
			return true;
		}
		case 'w':
		{
			if (!CHAL::HaveEeprom() || !CHAL::NeedFlushEeprom() || !_eepromCanWrite)
			{
				return false;
			}
			_eepromCanWrite = false;
			Flush();
			parser->GetReader()->GetNextChar();
			return true;
		}
		default: break;
	}

	uint8_t slot = parser->GetUInt8();
	if (parser->GetReader()->SkipSpaces() != '=')
	{
		return false;
	}

	parser->GetReader()->GetNextChar();
	uint32_t varValue = parser->GetUInt32();

	if (parser->IsError() || slot >= _eepromSize / sizeof(uint32_t) || !_eepromCanWrite)
	{
		return false;
	}

	if (!_eepromValid)
	{
		FlushConfig();
	}

	SetConfig32(slot * sizeof(uint32_t), varValue);

	return true;
}
