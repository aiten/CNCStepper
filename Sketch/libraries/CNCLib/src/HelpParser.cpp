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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "ConfigurationCNCLib.h"
#include "Control.h"
#include "HelpParser.h"
#include "Stepper.h"

////////////////////////////////////////////////////////////

void CHelpParser::Parse()
{
	_reader->SkipSpaces();

	// @formatter:off — disable formatter after this line

	if (IsToken(F("s"), true, false)) { SetSpeed(); return; }

	if (IsToken(F("r1"), true, false)) { MoveRel(X_AXIS); return; }
	if (IsToken(F("r2"), true, false)) { MoveRel(Y_AXIS); return; }
	if (IsToken(F("r3"), true, false)) { MoveRel(Z_AXIS); return; }
#if NUM_AXIS > 3
	if (IsToken(F("r4"), true, false)) { MoveRel(A_AXIS); return; }
	if (IsToken(F("r5"), true, false)) { MoveRel(B_AXIS); return; }
	if (IsToken(F("r6"), true, false)) { MoveRel(C_AXIS); return; }
#endif
	if (IsToken(F("r"), true, false))  { MoveRel(); return; }

	if (IsToken(F("a1"), true, false)) { MoveAbs(X_AXIS); return; }
	if (IsToken(F("a2"), true, false)) { MoveAbs(Y_AXIS); return; }
	if (IsToken(F("a3"), true, false)) { MoveAbs(Z_AXIS); return; }
#if NUM_AXIS > 3
	if (IsToken(F("a4"), true, false)) { MoveAbs(A_AXIS); return; }
	if (IsToken(F("a5"), true, false)) { MoveAbs(B_AXIS); return; }
	if (IsToken(F("a6"), true, false)) { MoveAbs(C_AXIS); return; }
#endif
	if (IsToken(F("a"), true, false))  { MoveAbs(); return; }

#ifndef REDUCED_SIZE

	if (IsToken(F("p1"), true, false)) { SetPosition(X_AXIS); return; }
	if (IsToken(F("p2"), true, false)) { SetPosition(Y_AXIS); return; }
	if (IsToken(F("p3"), true, false)) { SetPosition(Z_AXIS); return; }
#if NUM_AXIS > 3
	if (IsToken(F("p4"), true, false)) { SetPosition(A_AXIS); return; }
	if (IsToken(F("p5"), true, false)) { SetPosition(B_AXIS); return; }
	if (IsToken(F("p6"), true, false)) { SetPosition(C_AXIS); return; }
#endif
#endif
	/*
		if (IsToken(F("ix"), true, false)) { MyGoToReference(X_AXIS); return; }
		if (IsToken(F("iy"), true, false)) { MyGoToReference(Y_AXIS); return; }
		if (IsToken(F("iz"), true, false)) { MyGoToReference(Z_AXIS); return; }
		if (IsToken(F("ia"), true, false)) { MyGoToReference(A_AXIS); return; }
		if (IsToken(F("ib"), true, false)) { MyGoToReference(B_AXIS); return; }
		if (IsToken(F("ic"), true, false)) { MyGoToReference(C_AXIS); return; }
		if (IsToken(F("i!"), true, false)) { if (CheckEOC())	::GoToReference();	return; }
		*/
	if (IsToken(F("i!"), true, false))	{ if (CheckEOC())	CControl::GetInstance()->GoToReference();	return; }
	if (IsToken(F("!"), true, false))	{ if (CheckEOC()) { CControl::GetInstance()->Kill(); } return; }
	if (IsToken(F("?"), true, false))	{ if (CheckEOC()) { CStepper::GetInstance()->Dump(CStepper::DumpAll); }	return; }

	// @formatter:on — enable formatter after this line

#ifndef REDUCED_SIZE

	if (IsToken(F("-"), true, false))
	{
		if (CheckEOC())
		{
			for (uint8_t i = 0; i < NUM_AXIS - 1; i++)
			{
				StepperSerial.print(CStepper::GetInstance()->GetCurrentPosition(i));
				StepperSerial.print(F(":"));
			}
			StepperSerial.print(CStepper::GetInstance()->GetCurrentPosition(NUM_AXIS - 1));
			StepperSerial.println();
		}
		return;
	}
	
	// @formatter:off — disable formatter after this line
	if (IsToken(F("w"), true, false))	{ if (CheckEOC()) {	CStepper::GetInstance()->WaitBusy();} return; }
	// @formatter:on — enable formatter after this line

#endif

	Error(F("Illegal command"));
}

////////////////////////////////////////////////////////////

bool CHelpParser::SetSpeed()
{
	steprate_t max = 4500;
	steprate_t acc = 400;
	steprate_t dec = 400;

	if (IsUInt(_reader->GetChar()))
	{
		max = steprate_t(GetUInt32());
		if (IsUInt(_reader->SkipSpaces()))
		{
			acc = steprate_t(GetUInt32());
			if (IsUInt(_reader->SkipSpaces()))
			{
				dec = steprate_t(GetUInt32());
			}
		}
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->SetDefaultMaxSpeed(max, acc, dec);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::MoveAbs()
{
	udist_t position[NUM_AXIS] = { 0 };

	for (uint32_t& pos : position)
	{
		if (IsUInt(_reader->SkipSpaces()))
		{
			pos = GetUInt32();
		}
		else
		{
			break;
		}
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->MoveAbs(position);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::MoveAbs(axis_t axis)
{
	if (axis > NUM_AXIS)
	{
		return true;
	}

	udist_t dist = 0;
	char    ch   = _reader->SkipSpaces();

	if (IsUInt(ch))
	{
		dist = GetUInt32();
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->MoveAbs(axis, dist, 0);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::MoveRel()
{
	sdist_t position[NUM_AXIS] = { 0 };

	for (int32_t& pos : position)
	{
		if (IsInt(_reader->SkipSpaces()))
		{
			pos = GetInt32();
		}
		else
		{
			break;
		}
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->MoveRel(position);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::MoveRel(axis_t axis)
{
	if (axis > NUM_AXIS)
	{
		return true;
	}

	sdist_t dist = 0;
	char    ch   = _reader->SkipSpaces();

	if (IsInt(ch))
	{
		dist = GetInt32();
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->MoveRel(axis, dist, 0);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::SetPosition(axis_t axis)
{
	if (axis > NUM_AXIS)
	{
		return true;
	}

	udist_t dist = 0;
	char    ch   = _reader->SkipSpaces();

	if (IsUInt(ch))
	{
		dist = GetUInt32();
	}

	if (CheckEOC())
	{
		CStepper::GetInstance()->SetPosition(axis, dist);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::MyGoToReference(axis_t axis)
{
	if (axis > NUM_AXIS)
	{
		return true;
	}

	if (CheckEOC())
	{
		CControl::GetInstance()->GoToReference(axis, 0, axis != Z_AXIS);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CHelpParser::CheckEOC()
{
	if (_reader->IsError())
	{
		return false;
	}

	char ch = _reader->SkipSpaces();
	if (!_reader->IsEOC(ch))
	{
		Error(F("Illegal characters in command"));
		return false;
	}

	if (ch != 0)
	{
		_reader->GetNextChar();
		_reader->SkipSpaces();
	}

	return true;
}
