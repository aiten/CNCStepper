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
////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "HAL.h"
#include "UtilitiesStepperLib.h"

////////////////////////////////////////////////////////

uint32_t    CHAL::_eepromBuffer[2048] = { 0 };
const char* CHAL::_eepromFileName     = nullptr;

////////////////////////////////////////////////////////

bool CHAL::HaveEeprom()
{
	return true;
}

////////////////////////////////////////////////////////

void CHAL::InitEeprom()
{
	if (_eepromFileName)
	{
		FILE* f = nullptr;
		fopen_s(&f, _eepromFileName, "rb+");

		if (f)
		{
			fread(_eepromBuffer, 1, sizeof(_eepromBuffer), f);
			fclose(f);
		}
	}
}

void CHAL::FlushEeprom()
{
	if (_eepromFileName)
	{
		FILE* f = nullptr;
		fopen_s(&f, _eepromFileName, "wb+");
		if (f)
		{
			fwrite(_eepromBuffer, sizeof(_eepromBuffer), 1, f);
			fclose(f);
		}
	}
}


////////////////////////////////////////////////////////

#endif		// _MSC_VER
