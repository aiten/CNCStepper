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

#include "Arduino.h"

#define MAXDIGITALREADPINS 256

uint8_t digitalReadValues[MAXDIGITALREADPINS] = { LOW };

uint8_t digitalRead(int16_t pin)
{
	uint8_t value = digitalReadFromFile(pin);

	if (value == DIGITALREADNOVALUE && digitalReadEvent != nullptr)
	{
		value = digitalReadEvent(pin);
	}

	if (value == DIGITALREADNOVALUE && pin < MAXDIGITALREADPINS)
	{
		value = digitalReadValues[pin];
	}

	if (value == DIGITALREADNOVALUE)
	{
		value = LOW;
	}

	if (pin < MAXDIGITALREADPINS)
	{
		// remember last value
		digitalReadValues[pin] = value;
	}

	return value;
};

uint8_t digitalReadFromFile(int16_t pin)
{
	char tmpName[_MAX_PATH];
	char fileName[_MAX_PATH];
	::GetTempPathA(_MAX_PATH, tmpName);
	sprintf_s(fileName, "%s\\CNCLib_digitalReadFor_%i.txt", tmpName, int(pin));

	FILE* fin;
	fopen_s(&fin, fileName, "rt");
	if (fin)
	{
		char buffer[512];
		fgets(buffer, sizeof(buffer), fin);
		fclose(fin);
		remove(fileName);
		return atoi(buffer) == 0 ? LOW : HIGH;
	}
	return DIGITALREADNOVALUE;
}
