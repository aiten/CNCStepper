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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "CNCLib.h"
#include "GCodeBuilder.h"

////////////////////////////////////////////////////////////

CGCodeBuilder& CGCodeBuilder::Add(FLSTR cmd)
{
	strcat_P(_commandString, (const char*) cmd);
	return *this;
}

////////////////////////////////////////////////////////////

CGCodeBuilder& CGCodeBuilder::Add(char* cmd)
{
	strcat(_commandString, cmd);
	return *this;
}

////////////////////////////////////////////////////////////

CGCodeBuilder& CGCodeBuilder::Add(mm1000_t mm1000)
{
	char tmp[16];
	Add(CMm1000::ToString(mm1000, tmp, 3));
	return *this;
}

////////////////////////////////////////////////////////////

CGCodeBuilder& CGCodeBuilder::AddAxisName(axis_t axis)
{
	auto len                = uint8_t(strlen(_commandString));
	_commandString[len]     = AxisToChar(axis);
	_commandString[len + 1] = 0;
	return *this;
}

////////////////////////////////////////////////////////////

char CGCodeBuilder::AxisToChar(axis_t axis)
{
	return pgm_read_byte(PSTR("XYZABCUVW") + axis);
}

////////////////////////////////////////////////////////////

CGCodeBuilder& CGCodeBuilder::InitCommand()
{
	_commandString[0] = 0;
	return *this;
}
