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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arduino.h>

#include "MenuNavigator.h"

////////////////////////////////////////////////////////////

void CMenuNavigator::AdjustOffset(menupos_t menuEntries, menupos_t firstline, menupos_t lastline)
{
	menupos_t pos = GetPosition();

	if (pos == 0)
	{
		SetOffset(0);				// first menuitem selected => move to first line
	}
	else if (pos - 1 < GetOffset())
	{
		SubOffset(GetOffset() - (pos - 1));
	}

	if (menuEntries >= lastline)
	{
		if (pos == menuEntries - 1)
		{
			AddOffset(pos + firstline - GetOffset() - lastline);	// last menuitem selected => move to last line
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

	uint8_t printtorow = i + firstline - GetOffset();	// may overrun => no need to check for minus
	if (printtorow >= firstline && printtorow <= lastline)
		return printtorow;

	return 255;
}

