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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <HelpParser.h>

#include "MyMotionControl.h"
#include "MyParser.h"

////////////////////////////////////////////////////////////

bool CMyParser::MCommand(mcode_t mcode)
{
	if (super::MCommand(mcode))
	{
		return true;
	}

	switch (mcode)
	{
		// @formatter:off — disable formatter after this line
		case 116: _OkMessage = PrintInfo;	return true;
		case 117: M117Command();			return true;
		case 118: M118Command();			return true;
			// @formatter:on — enable formatter after this line
		default: break;
	}

	return false;
}

////////////////////////////////////////////////////////////

void CMyParser::PrintInfo()
{
	static_cast<CMyMotionControl*>(CMotionControlBase::GetInstance())->PrintInfo();
}

////////////////////////////////////////////////////////////

bool CMyParser::GetAxisAbs(SAxisMove& move)
{
	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if (GetAxisNo(ch, NUM_AXIS,axis))
		{
			GetAxis(axis, move, AbsolutWithZeroShiftPosition);
		}
		else
		{
			break;
		}

		if (CheckError())
		{
			return false;
		}
	}

	return move.axes != 0;
}

////////////////////////////////////////////////////////

void CMyParser::M117Command()
{
	SAxisMove move(false);

	if (GetAxisAbs(move))
	{
		auto pMC = static_cast<CMyMotionControl*>(CMotionControlBase::GetInstance());
		pMC->MoveAngle(move.newpos);
	}
}

////////////////////////////////////////////////////////////

void CMyParser::M118Command()
{
	SAxisMove move(false);

	if (GetAxisAbs(move))
	{
		auto pMC = static_cast<CMyMotionControl*>(CMotionControlBase::GetInstance());
		pMC->MoveAngleLog(move.newpos);
	}
}
