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
#include <Arduino.h>

#include "SDDirReader.h"

////////////////////////////////////////////////////////////

CSDDirReader::CSDDirReader(bool (*skip)(File*))
{
	_rootDir = SD.open(PSTR("/"));
	_skip    = skip;
}

////////////////////////////////////////////////////////////

CSDDirReader::CSDDirReader(const char* dir, bool (*skip)(File*))
{
	_rootDir = SD.open(dir);
	_skip    = skip;
}

////////////////////////////////////////////////////////////

CSDDirReader::~CSDDirReader()
{
	if (_rootDir)
	{
		_rootDir.rewindDirectory();
		_rootDir.close();
	}

	if (Current)
	{
		Current.close();
	}
}

////////////////////////////////////////////////////////////

void CSDDirReader::Reset()
{
	if (_rootDir)
	{
		_rootDir.rewindDirectory();
	}
}

////////////////////////////////////////////////////////////

bool CSDDirReader::MoveNext()
{
	if (!_rootDir)
	{
		return false;
	}

	do
	{
		if (Current)
		{
			Current.close();
		}

		Current = _rootDir.openNextFile();
	}
	while (Current && _skip != nullptr && _skip(&Current));

	return Current;
}

////////////////////////////////////////////////////////////
