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

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "MenuNavigator.h"

////////////////////////////////////////////////////////////

void CMenuNavigator::AdjustOffset(menupos_t menuEntries, menupos_t firstline, menupos_t lastline)
{
	menupos_t pos = GetPosition();

	if (pos == 0)
	{
		SetOffset(0); // first menuitem selected => move to first line
	}
	else if (pos - 1 < GetOffset())
	{
		SubOffset(GetOffset() - (pos - 1));
	}

	if (menuEntries >= lastline)
	{
		if (pos == menuEntries - 1)
		{
			AddOffset(pos + firstline - GetOffset() - lastline); // last menuitem selected => move to last line
		}
		else if (((pos + 1) + firstline - GetOffset()) > lastline)
		{
			AddOffset((pos + 1) + firstline - GetOffset() - lastline);
		}
	}
}

////////////////////////////////////////////////////////////

uint8_t CMenuNavigator::ToPrintLine(menupos_t firstline, menupos_t lastline, menupos_t i)
{
	// return 255 if not to print

	uint8_t printtorow = i + firstline - GetOffset(); // may overrun => no need to check for minus
	if (printtorow >= firstline && printtorow <= lastline)
		return printtorow;

	return 255;
}
