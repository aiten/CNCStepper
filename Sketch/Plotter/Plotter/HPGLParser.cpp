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
#include <ctype.h>
#include <Arduino.h>

#include <CNCLib.h>
#include <HelpParser.h>

#include "MyControl.h"
#include "HPGLParser.h"
#include "PlotterControl.h"

////////////////////////////////////////////////////////////

enum EPenMove
{
	PD,
	PU,
	PA,
	PR
};

////////////////////////////////////////////////////////////

CHPGLParser::SState CHPGLParser::_state;

////////////////////////////////////////////////////////////

mm1000_t CHPGLParser::HPGLToMM1000X(int32_t xx)
{
	// HPGL unit is 1/40 mm => 0.025mm
	return (xx + _state._HPOffsetX) * 25;
}

////////////////////////////////////////////////////////////

mm1000_t CHPGLParser::HPGLToMM1000Y(int32_t yy)
{
	return (yy + _state._HPOffsetY) * 25;
}

////////////////////////////////////////////////////////////

void CHPGLParser::ReadAndSkipSemicolon()
{
	char ch = _reader->SkipSpaces();
	if (ch == ';')
	{
		_reader->GetNextChar();
	}
}

////////////////////////////////////////////////////////////

void CHPGLParser::Parse()
{
	_reader->IsNextChar('\r');
	if (_reader->GetChar())
	{
		_reader->SkipSpaces();
		if (IsToken(F("SP"), false, false))
		{
			SelectPenCommand();
			return;
		}
		if (IsToken(F("VS"), false, false))
		{
			PenVelocityCommand();
			return;
		}
		if (IsToken(F("VN"), false, false))
		{
			PenVelocityNormalCommand();
			return;
		}
		if (IsToken(F("IN"), false, false))
		{
			InitCommand();
			return;
		}
		if (IsToken(F("PD"), false, false))
		{
			PenMoveCommand(PD);
			return;
		}
		if (IsToken(F("PU"), false, false))
		{
			PenMoveCommand(PU);
			return;
		}
		if (IsToken(F("PA"), false, false))
		{
			PenMoveCommand(PA);
			return;
		}
		if (IsToken(F("PR"), false, false))
		{
			PenMoveCommand(PR);
			return;
		}
		if (IsToken(F("LT"), false, false))
		{
			IgnoreCommand();
			return;
		}
		if (IsToken(F("WU"), false, false))
		{
			IgnoreCommand();
			return;
		}

		Error(MESSAGE_GCODE_IllegalCommand);
	}
}

////////////////////////////////////////////////////////////

void CHPGLParser::IgnoreCommand()
{
	_reader->MoveToEnd();
}

////////////////////////////////////////////////////////////

void CHPGLParser::InitCommand()
{
	Init();
	ReadAndSkipSemicolon();
}

////////////////////////////////////////////////////////////

void CHPGLParser::PenMoveCommand(uint8_t cmdIdx)
{
	Plotter.Resume(cmdIdx != PU);

	switch (cmdIdx)
	{
		case PU: Plotter.DelayPenUp();
			_state.FeedRate = _state.FeedRateUp;
			break;
		case PD: Plotter.PenDown();
			_state.FeedRate = _state.FeedRateDown;
			break;
		case PA: _state._HPGLIsAbsolute = true;
			break;
		case PR: _state._HPGLIsAbsolute = false;
			break;
	}

	if (IsToken(F("PD"), false, false))
	{
		PenMoveCommand(PD);
		return;
	}
	if (IsToken(F("PU"), false, false))
	{
		PenMoveCommand(PU);
		return;
	}
	if (IsToken(F("PA"), false, false))
	{
		PenMoveCommand(PA);
		return;
	}
	if (IsToken(F("PR"), false, false))
	{
		PenMoveCommand(PR);
		return;
	}

	while (IsInt(_reader->GetChar()))
	{
		int32_t xIn = GetInt32();

		//all blank or colon

		if (IsInt(_reader->SkipSpaces()))
		{
			// only blank as separator
		}
		else if (_reader->GetChar() == ',' && IsInt(_reader->GetNextCharSkipScaces()))
		{
			// Colon
		}
		else
		{
		ERROR_MISSINGARGUMENT:
			Error(F("Missing or invalid parameter"));
			return;
		}

		int32_t yIn = GetInt32();

		if (_reader->IsError()) goto ERROR_MISSINGARGUMENT;

		mm1000_t x = HPGLToMM1000X(xIn);
		mm1000_t y = HPGLToMM1000Y(yIn);

		if (_state._HPGLIsAbsolute)
		{
			if (x != CMotionControlBase::GetInstance()->GetPosition(X_AXIS) || y != CMotionControlBase::GetInstance()->GetPosition(Y_AXIS))
			{
				Plotter.DelayPenNow();
				CMotionControlBase::GetInstance()->MoveAbsEx(_state.FeedRate, X_AXIS, x, Y_AXIS, y, -1);
			}
		}
		else
		{
			if (x != 0 || y != 0)
			{
				Plotter.DelayPenNow();
				CMotionControlBase::GetInstance()->MoveRelEx(_state.FeedRate, X_AXIS, x, Y_AXIS, y, -1);
			}
		}
		if (_reader->SkipSpaces() != ',')
		{
			break;
		}

		_reader->GetNextCharSkipScaces();
	}
	ReadAndSkipSemicolon();
}

////////////////////////////////////////////////////////////

void CHPGLParser::SelectPenCommand()
{
	uint8_t newPen = GetUInt8();
	if (!Plotter.SetPen(newPen))
	{
		Error(F("Select Pen failed"));
	}

	ReadAndSkipSemicolon();
}

////////////////////////////////////////////////////////////

void CHPGLParser::PenVelocityNormalCommand()
{
	_state.SetFeedRates();
	ReadAndSkipSemicolon();
}

////////////////////////////////////////////////////////////

void CHPGLParser::PenVelocityCommand()
{
	int32_t velocityCmPerSec = GetInt32Scale(10, 1000000, 3, 255);

	if (IsError())
	{
		return;
	}

	// feedrate is => mm_1000 / min
	_state.FeedRateDown = CMotionControlBase::GetInstance()->GetMaxFeedRate(X_AXIS, velocityCmPerSec * 60l * 10l);

	ReadAndSkipSemicolon();
}
