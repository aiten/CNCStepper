////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/
////////////////////////////////////////////////////////

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

void CConfigEeprom::Init(uint16_t eepromSize, const void* defaultEeprom, uint32_t eepromID)
{
	CHAL::InitEeprom();

	_eepromSize    = eepromSize;
	_defaultEeprom = defaultEeprom;

	if (CHAL::HaveEeprom())
	{
		_eepromValid = true;
		_eepromValid = GetConfigU32(offsetof(SCNCEeprom, Signature)) == eepromID;
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

inline const void* AddAdr(const void* adr, eepromofs_t ofs)
{
	return ((uint8_t*)adr) + ofs;
}

uint32_t CConfigEeprom::GetConfig32(eepromofs_t ofs) const
{
	if (_eepromValid)
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
