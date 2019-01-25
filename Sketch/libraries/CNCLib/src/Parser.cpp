/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "ConfigurationCNCLib.h"
#include "Parser.h"
#include "Stepper.h"
#include "MotionControlBase.h"

////////////////////////////////////////////////////////////

void CParser::ParseCommand()
{
	if (InitParse())
	{
		Parse();
	}
	CleanupParse();
}

////////////////////////////////////////////////////////////

bool CParser::CheckError()
{
	if (IsError())
	{
		return true;
	}
	if (_reader->IsError())
	{
		_error = _reader->GetError();
		return true;
	}
	if (CStepper::GetInstance()->IsError())
	{
		_error = CStepper::GetInstance()->GetError();
		return true;
	}
	if (CMotionControlBase::GetInstance()->IsError())
	{
		_error = CMotionControlBase::GetInstance()->GetError();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////

bool CParser::ExpectEndOfCommand()
{
	// return true => not char left on input => OK
	// return false  => error or still char available

	if (IsError())
	{
		return false;
	}

	if (_reader->IsEOC(SkipSpacesOrComment()))
	{
		return true;
	}

	Error(MESSAGE(MESSAGE_PARSER_EndOfCommandExpected));
	return false;
}

////////////////////////////////////////////////////////////

uint8_t  CParser::GetUInt8() { return GetUInt<uint8_t>(); }
uint16_t CParser::GetUInt16() { return GetUInt<uint16_t>(); }
uint32_t CParser::GetUInt32() { return GetUInt<uint32_t>(); }

char    CParser::GetInt8() { return GetInt<char>(); }
int16_t CParser::GetInt16() { return GetInt<int16_t>(); }
int32_t CParser::GetInt32() { return GetInt<int32_t>(); }
sdist_t CParser::GetSDist() { return GetInt<sdist_t>(); }

////////////////////////////////////////////////////////////

int32_t CParser::GetInt32Scale(int32_t minvalue, int32_t maxvalue, uint8_t scale, uint8_t maxScale)
{
	// ignore digits between scale and maxScale (first digit after scale is used for round)
	// 1.2345 with scale=3 and maxScale=5 is ok => return 1235 (calculated with scale - round)

	bool    negativ;
	int32_t value     = 0;
	uint8_t thisscale = 0;
	uint8_t ch        = _reader->GetChar();

	if ((negativ = CStreamReader::IsMinus(ch)) != 0)
	{
		ch = _reader->GetNextChar();
	}

	if (!CStreamReader::IsDigit(ch) && !CStreamReader::IsDot(ch))
	{
		ErrorNotANumber();
		return 0;
	}

	while (CStreamReader::IsDigit(ch))
	{
		value *= 10l;
		value += ch - '0';
		ch = _reader->GetNextChar();
	}

	if (CStreamReader::IsDot(ch))
	{
		ch = _reader->GetNextChar();

		if (!CStreamReader::IsDigit(ch))
		{
			Error(MESSAGE(MESSAGE_PARSER_NotANumber_MissingScale));
			return 0;
		}

		while (CStreamReader::IsDigit(ch))
		{
			if (thisscale < scale)
			{
				value *= 10l;
				value += ch - '0';
			}
			else if (thisscale == scale)
			{
				// check round
				if (ch >= '5')
				{
					value++;
				}
			}

			ch = _reader->GetNextChar();
			thisscale++;
		}
	}

	if (thisscale > maxScale)
	{
		Error(MESSAGE( MESSAGE_PARSER_NotANumber_MaxScaleExceeded));
		return 0;
	}
	else if (thisscale < scale)
	{
		for (; thisscale != scale; thisscale++)
		{
			value *= 10l;
		}
	}

	if (negativ)
	{
		value = -(value);
	}

	if (value < minvalue)
	{
		Error(MESSAGE(MESSAGE_PARSER_ValueLessThanMin));
	}
	else if (value > maxvalue)
	{
		Error(MESSAGE(MESSAGE_PARSER_ValueGreaterThanMax));
	}

	return value;
}

////////////////////////////////////////////////////////////

expr_t CParser::GetDouble()
{
	uint8_t ch    = _reader->GetChar();
	auto    start = const_cast<char*>(_reader->GetBuffer());

	while (CStreamReader::IsDigitDot(ch))
	{
		ch = _reader->GetNextChar();
	}

	// check for scientific notation like "2.3e-4" or "1.23e50"
	if (toupper(ch) == 'E')
	{
		ch = _reader->GetNextChar();

		if (ch == '+' || ch == '-')
		{
			ch = _reader->GetNextChar();
		}

		while (CStreamReader::IsDigit(ch))
		{
			ch = _reader->GetNextChar();
		}
	}

	// temporary terminate buffer with '\00'
	CStreamReader::CSetTemporary terminate(_reader->GetBuffer());
	return expr_t(strtod(start, nullptr));
}

////////////////////////////////////////////////////////////

bool CParser::IsToken(FLSTR b, bool expectDel, bool ignoreCase)
{
	const char* tmp;
	if (!TryToken(_reader->GetBuffer(), b, expectDel, ignoreCase, tmp))
	{
		return false;
	}

	_reader->ResetBuffer(tmp);

	_reader->SkipSpaces();
	return true;
}

////////////////////////////////////////////////////////////

bool CParser::TryToken(const char* buffer, FLSTR b, bool expectDel, bool ignoreCase)
{
	const char* dummy;
	return TryToken(buffer, b, expectDel, ignoreCase, dummy);
}

////////////////////////////////////////////////////////////

bool CParser::TryToken(const char* buffer, FLSTR b, bool ignoreCase)
{
	auto p = (const char*)b;

	//const char* tmp = _reader->GetBuffer();
	char c = pgm_read_byte(p);

	while (c && ConvertChar(*buffer, ignoreCase) == ConvertChar(c, ignoreCase))
	{
		p++;
		buffer++;
		c = pgm_read_byte(p);
	}

	return c == 0 && *buffer == 0;
}

////////////////////////////////////////////////////////////

bool CParser::TryToken(const char* buffer, FLSTR b, bool expectDel, bool ignoreCase, const char*& nextchar)
{
	auto p = (const char*)b;

	//const char* tmp = _reader->GetBuffer();
	char c = pgm_read_byte(p);

	while (c && ConvertChar(*buffer, ignoreCase) == ConvertChar(c, ignoreCase))
	{
		p++;
		buffer++;
		c = pgm_read_byte(p);
	}

	switch (c)
	{
		case '\001':
		{
			if (!CStreamReader::IsDigit(*buffer))
			{
				return false;
			}
			c = 0;
			break;
		}
		case '\002':
		{
			if (*buffer != 0 && !CStreamReader::IsSpace(*buffer) && CStreamReader::IsDigit(*buffer))
			{
				return false;
			}
			c = 0;
			break;
		}
		case '\003':
		{
			if (*buffer != 0)
			{
				return false;
			}
			c = 0;
			break;
		}
		default: break;
	}

	if (c)
	{
		return false;
	}

	// check if line-end or blank

	if (expectDel)
	{
		if (!CStreamReader::IsSpaceOrEnd(*buffer) && !CStreamReader::IsEOC(*buffer))
		{
			return false;
		}
	}

	nextchar = buffer;

	return true;
}
