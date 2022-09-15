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

#include <CNCLib.h>
#include <GCodeParser.h>

#include "MyControl.h"
#include "MyLcd.h"

////////////////////////////////////////////////////////////

CMyControl      Control;
CMotionControl  MotionControl;
CConfigEeprom   Eprom;
HardwareSerial& StepperSerial = Serial;

////////////////////////////////////////////////////////////

#ifndef MYNUM_AXIS
#error Please define MYNUM_AXIS
#endif

////////////////////////////////////////////////////////////

const CConfigEeprom::SCNCEeprom CMyControl::_eepromFlash PROGMEM =
{
	EPROM_SIGNATURE,
	NUM_AXIS, MYNUM_AXIS, offsetof(CConfigEeprom::SCNCEeprom,Axis), sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions),
	GetInfo1a() | CConfigEeprom::IS_LASER | CConfigEeprom::HAVE_SPINDLE | CConfigEeprom::HAVE_SPINDLE_ANALOG, GetInfo1b(),
	0,
	STEPPERDIRECTION, 0, 0,SPINDLE_FADETIMEDELAY,
	SPINDLE_MAXSPEED,
	CNC_JERKSPEED,
	CNC_MAXSPEED,
	CNC_ACC,
	CNC_DEC,
	STEPRATE_REFMOVE,
	MOVEAWAYFROMREF_MM1000,
	STEPSPERMM / 1000.0,
	TIMEOUTSETIDLE_DEFAULT,
	0,
	0,
	0,
	0,
	0,
	{
		{ AXISPARAMETER(X_MAXSIZE, X_USEREFERENCE, REFMOVE_1_AXIS, X_REFERENCEHITVALUE_MIN, X_REFERENCEHITVALUE_MAX, 0, X_MAXSPEED, X_ACC, X_DEC, X_STEPRATE_REFMOVE, X_STEPSPERMM, X_PROBESIZE) },
		{ AXISPARAMETER(Y_MAXSIZE, Y_USEREFERENCE, REFMOVE_2_AXIS, Y_REFERENCEHITVALUE_MIN, Y_REFERENCEHITVALUE_MAX, 0, Y_MAXSPEED, Y_ACC, Y_DEC, Y_STEPRATE_REFMOVE, Y_STEPSPERMM, Y_PROBESIZE) },
		{ AXISPARAMETER(Z_MAXSIZE, Z_USEREFERENCE, REFMOVE_3_AXIS, Z_REFERENCEHITVALUE_MIN, Z_REFERENCEHITVALUE_MAX, 0, Z_MAXSPEED, Z_ACC, Z_DEC, Z_STEPRATE_REFMOVE, Z_STEPSPERMM, Z_PROBESIZE) },
#if NUM_AXIS > 3
		{ AXISPARAMETER(A_MAXSIZE,     A_USEREFERENCE, REFMOVE_4_AXIS,  A_REFERENCEHITVALUE_MIN, A_REFERENCEHITVALUE_MAX,	0,	A_MAXSPEED, A_ACC, A_DEC,	A_STEPRATE_REFMOVE,  A_STEPSPERMM, A_PROBESIZE) },
#endif
#if NUM_AXIS > 4
		{ AXISPARAMETER(B_MAXSIZE,     B_USEREFERENCE, REFMOVE_5_AXIS,  B_REFERENCEHITVALUE_MIN, B_REFERENCEHITVALUE_MAX,	0,	B_MAXSPEED,	B_ACC, B_DEC,	B_STEPRATE_REFMOVE,  B_STEPSPERMM, B_PROBESIZE) },
#endif
#if NUM_AXIS > 5
		{ AXISPARAMETER(C_MAXSIZE,     C_USEREFERENCE, REFMOVE_6_AXIS,  C_REFERENCEHITVALUE_MIN, C_REFERENCEHITVALUE_MAX,	0,	C_MAXSPEED,	C_ACC, C_DEC,	C_STEPRATE_REFMOVE,  C_STEPSPERMM, C_PROBESIZE) },
#endif
	}
};

////////////////////////////////////////////////////////////

void CMyControl::Init()
{
	CSingleton<CConfigEeprom>::GetInstance()->Init(sizeof(CConfigEeprom::SCNCEeprom), &_eepromFlash, EPROM_SIGNATURE);

#ifdef DISABLELEDBLINK
	DisableBlinkLed();
#endif

	super::Init();

	InitFromEeprom();

	_data.Init();

	_laserPWM.Init();
	_laserOnOff.Init();

	_laserWater.Init();
	_laserVacuum.Init();

	CGCodeParserDefault::InitAndSetFeedRate(-STEPRATETOFEEDRATE(GO_DEFAULT_STEPRATE), G1_DEFAULT_FEEDPRATE, STEPRATETOFEEDRATE(G1_DEFAULT_MAXSTEPRATE));

#ifdef MYUSE_LCD
	InitSD(SD_ENABLE_PIN);
#endif

#ifdef DISABLE_ISANYREFERENCE

	CStepper::GetInstance()->SetCheckForReference(false);

#endif
}

////////////////////////////////////////////////////////////

void CMyControl::PrintVersion()
{
	super::PrintVersion();
	StepperSerial.print(MESSAGE_MYCONTROL_VERSION);
}

////////////////////////////////////////////////////////////

void CMyControl::IOControl(uint8_t tool, uint16_t level)
{
	switch (tool)
	{
		case SpindleCCW:
		case SpindleCW:

			if (level != 0)
			{
				_laserPWM.On(uint8_t(level));
				_laserOnOff.On();
			}
			else
			{
				_laserOnOff.Off();
			}
			return;

		case Vacuum:
		{
			_laserVacuum.Set(level > 0);
			return;
		}
			//  case Coolant: _laserWater.Set(level > 0); return; do not allow water turn off
		default: break;
	}

	if (!_data.IOControl(tool, level))
	{
		super::IOControl(tool, level);
	}
}

////////////////////////////////////////////////////////////

uint16_t CMyControl::IOControl(uint8_t tool)
{
	switch (tool)
	{
		case SpindleCW:
		case SpindleCCW: { return _laserPWM.IsOn(); }
		case Probe: { return _data._probe.IsOn(); }
		case Coolant: { return _laserWater.IsOn(); }
		case Vacuum: { return _laserVacuum.IsOn(); }
		case ControllerFan: { return _data._controllerfan.GetLevel(); }
		default: break;
	}

	return super::IOControl(tool);
}

////////////////////////////////////////////////////////////

void CMyControl::Kill()
{
	super::Kill();
	_data.Kill();
	_laserOnOff.Off();
}

////////////////////////////////////////////////////////////

bool CMyControl::IsKill()
{
	if (false && _data.IsKill())
	{
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("E-Stop"));
#endif
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////

void CMyControl::TimerInterrupt()
{
	super::TimerInterrupt();
	_data.TimerInterrupt();
}

////////////////////////////////////////////////////////////

void CMyControl::Initialized()
{
	super::Initialized();
	_data.Initialized();
}

////////////////////////////////////////////////////////////

void CMyControl::Poll()
{
	super::Poll();

	if (IsHold())
	{
		if (_data._resume.IsOn() || _data._holdresume.IsOn())
		{
			Resume();
#ifdef MYUSE_LCD
			Lcd.ClearDiagnostic();
#endif
		}
	}
	else if (_data._hold.IsOn() || _data._holdresume.IsOn())
	{
		Hold();
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("LCD Hold"));
#endif
	}
}

////////////////////////////////////////////////////////////

bool CMyControl::OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo)
{
	_data.OnEvent(eventType, addInfo);

	switch (eventType)
	{
		case OnStartCut:
		{
			if (CGCodeParserBase::IsSpindleOn())
			{
				const bool newIsCutMove = addInfo != 0;
				if (CGCodeParserBase::IsCutMove() != newIsCutMove)
				{
					CStepper::GetInstance()->IoControl(CControl::SpindleCW, newIsCutMove ? CGCodeParserBase::GetSpindleSpeed() : 0);
				}
			}
			break;
		}
		case OnStartEvent:
		{
			_laserWater.On();
			_laserVacuum.On();
			break;
		}
		case OnIdleEvent:
		{
			if (_laserOnOff.IsOn() == false)
			{
				if (millis() - CStepper::GetInstance()->IdleTime() > LASERWATER_ONTIME)
				{
					_laserWater.Off();
				}
				if (millis() - CStepper::GetInstance()->IdleTime() > LASERVACUUM_ONTIME)
				{
					_laserVacuum.Off();
				}
			}
			break;
		}
		default: break;
	}

	return super::OnEvent(eventType, addInfo);
}
