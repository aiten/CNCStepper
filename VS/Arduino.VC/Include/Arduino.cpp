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
http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

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
	char tmpname[_MAX_PATH];
	char fileName[_MAX_PATH];
	::GetTempPathA(_MAX_PATH, tmpname);
	sprintf_s(fileName, "%s\\CNCLib_digitalReadFor_%i.txt", tmpname, int(pin));

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
