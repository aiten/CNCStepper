////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

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

#pragma once

#include "CNCLibTypes.h"

////////////////////////////////////////////////////////

class CMenuHelper
{
public:

	typedef uint8_t menupos_t;

public:

	menupos_t GetPosition()											{ return _position; }
	void SetPosition(menupos_t position)							{ _position = position; }

	menupos_t GetOffset()											{ return _offset; }

	void AdjustOffset(menupos_t menuEntries, menupos_t firstline, menupos_t lastline);
	uint8_t ToPrintLine(menupos_t firstline, menupos_t lastline, menupos_t i);		// return 255 if not to print

	void Clear()													{ _offset = 0;  _position = 0; }

protected:

	//////////////////////////////////////////

	void SetOffset(menupos_t offset)								{ _offset = offset; }
	void AddOffset(menupos_t offset)								{ _offset += offset; }
	void SubOffset(menupos_t offset)								{ _offset -= offset; }

private:

	menupos_t			_position;									// current selected menu
	menupos_t			_offset;									// start of menuitem to draw  

};

////////////////////////////////////////////////////////
