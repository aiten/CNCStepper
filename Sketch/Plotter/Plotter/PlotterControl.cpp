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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <StepperLib.h>
#include <CNCLib.h>
#include "PlotterControl.h"
#include "MyControl.h"
#include "MyLcd.h"
#include "HPGLParser.h"

////////////////////////////////////////////////////////////

CPlotter::CPlotter()
{
	_isPenDown        = false;
	_isPenDownTimeout = false;
	_isDelayPen       = false;
	_pen              = 0;
	_havePen          = false;
}

////////////////////////////////////////////////////////////

void CPlotter::Init() {}

////////////////////////////////////////////////////////////

void CPlotter::Initialized()
{
	PenUpNow();
}

////////////////////////////////////////////////////////////

void CPlotter::Idle(unsigned int /* idleTime */)
{
	if (!CStepper::GetInstance()->IsBusy() && (millis() - CStepper::GetInstance()->IdleTime()) > PLOTTER_PENUP_TIMEOUT)
	{
		if (_isDelayPen)
		{
			// abort delay but set position
			if (_isDelayPenDown)
			{
				_isPenDownTimeout = true;
			}
			PenUp();
		}
		if (_isPenDown)
		{
			_isPenDownTimeout = true;
			PenUp();
		}
	}
}

////////////////////////////////////////////////////////////

void CPlotter::Resume(bool restorePenDown)
{
	if (IsResumePenDown())
	{
		_isPenDownTimeout = false;
		if (restorePenDown)
		{
			PenDown();
		}
	}
}

////////////////////////////////////////////////////////////

void CPlotter::PenUp()
{
	_isDelayPen = false;
	if (_isPenDown)
	{
		PenUpNow();
	}
}

////////////////////////////////////////////////////////////

void CPlotter::PenUpNow()
{
	CStepper::GetInstance()->Wait(1);
	_isPenDown = false;
	MoveToPenPosition(
		CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, MovePenUpFeedRate)),
		ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenUpPos)), Z_AXIS));

#ifdef MYUSE_LCD
	// Lcd.DrawRequest(true,CLcd::DrawAll); => delay off movementBuffer
#endif
}


////////////////////////////////////////////////////////////

void CPlotter::PenDown()
{
	_isDelayPen = false;
	if (!_isPenDown)
	{
		_isPenDown = true;
		MoveToPenPosition(
			CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, MovePenDownFeedRate)),
			ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenDownPos)), Z_AXIS));

		CStepper::GetInstance()->Wait(1);
#ifdef MYUSE_LCD
		// Lcd.DrawRequest(true,CLcd::DrawAll); => delay off movementBuffer
		Lcd.DrawRequest(CLcd::DrawForceAll);
#endif
	}
}

////////////////////////////////////////////////////////////

void CPlotter::DelayPenNow()
{
	if (_isDelayPen)
	{
		_isDelayPen = false;
		if (_isDelayPenDown)
		{
			PenDown();
		}
		else
		{
			PenUp();
		}
	}
}

////////////////////////////////////////////////////////////

bool CPlotter::MoveToPenPosition(feedrate_t feedRate, mm1000_t pos)
{
#if PENTYPE == PENTYPE_ZAXIS      // Z-AXIS

  CMotionControlBase::GetInstance()->MoveAbsEx(
    feedRate,
    Z_AXIS, pos,
    -1);

  return !CStepper::GetInstance()->IsError();

#elif PENTYPE == PENTYPE_SERVO    // servo

	CStepper::GetInstance()->IoControl(CControl::Servo2, int16_t(pos));
	CStepper::GetInstance()->Wait(feedRate / 10);

	return true;

#else

error

#endif
}

////////////////////////////////////////////////////////////

bool CPlotter::ToPenChangePos(uint8_t pen)
{
	mm1000_t ofs_x = pen * CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenChangePos_x_ofs));
	mm1000_t ofs_y = pen * CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenChangePos_y_ofs));;

	CMotionControlBase::GetInstance()->MoveAbsEx(
		CHPGLParser::_state.FeedRateUp,
		X_AXIS, ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenChangePos_x)), X_AXIS) + ofs_x,
		Y_AXIS, ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenChangePos_y)), Y_AXIS) + ofs_y,
		-1);

	if (CStepper::GetInstance()->IsError())
	{
		return false;
	}

	return MoveToPenPosition(
		CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, MovePenChangeFeedRate)),
		ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenChangePos_z)), Z_AXIS));
}

////////////////////////////////////////////////////////////

bool CPlotter::OffPenChangePos(uint8_t /* pen */)
{
	return MoveToPenPosition(
		CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, MovePenChangeFeedRate)),
		ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, PenUpPos)), Z_AXIS));
}

////////////////////////////////////////////////////////////

bool CPlotter::SetPen(uint8_t pen)
{
	if (_pen == pen && _havePen)
	{
		return true;
	}

	if (!PenToDepot())
	{
		return false;
	}

	return PenFromDepot(pen);
}

////////////////////////////////////////////////////////////

bool CPlotter::PenToDepot()
{
	if (!_havePen)
	{
		return true;
	}

	PenUp();
	CStepper::GetInstance()->WaitBusy();

	/////////////////////////////////////
	// TODO: 

	if (!ToPenChangePos(_pen))
	{
		return false;
	}

	CStepper::GetInstance()->IoControl(CControl::Servo1, CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, PenChangeServoClampOpenPos)));
	CStepper::GetInstance()->Wait(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, PenChangeServoClampOpenDelay)) / 10);

	OffPenChangePos(_pen);

	////////////////////////////////////

	_havePen = false;
	return true;
}

////////////////////////////////////////////////////////////

bool CPlotter::PenFromDepot(uint8_t pen)
{

	/////////////////////////////////////
	// TODO: 

	if (!ToPenChangePos(pen))
	{
		return false;
	}

	CStepper::GetInstance()->IoControl(CControl::Servo1, CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, PenChangeServoClampClosePos)));
	CStepper::GetInstance()->Wait(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, PenChangeServoClampClosePos)) / 10);

	OffPenChangePos(pen);

	////////////////////////////////////

	CStepper::GetInstance()->WaitBusy();

	_pen     = pen;
	_havePen = true;
	return true;
}

////////////////////////////////////////////////////////////

mm1000_t CPlotter::ConvertConfigPos(mm1000_t pos, axis_t axis)
{
#if PENTYPE == PENTYPE_SERVO    // servo
	if (axis == Z_AXIS)
	{
		return pos;
	}
#endif

	if (pos >= 0x7fff000l)
	{
		eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * axis;

		pos = CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, Axis[0].Size) + ofs);
	}
	return pos;
}
