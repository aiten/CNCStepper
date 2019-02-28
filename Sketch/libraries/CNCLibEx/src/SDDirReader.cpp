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
