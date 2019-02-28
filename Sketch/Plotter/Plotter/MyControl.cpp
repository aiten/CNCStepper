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

////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <StepperLib.h>
#include <CNCLib.h>
#include "MyControl.h"
#include "HPGLParser.h"
#include "PlotterControl.h"

////////////////////////////////////////////////////////////

CMotionControl  MotionControl;
CConfigEeprom   Eprom;
HardwareSerial& StepperSerial = Serial;

////////////////////////////////////////////////////////////

#ifndef MYNUM_AXIS
#error Please define MYNUM_AXIS
#endif

////////////////////////////////////////////////////////////

const CMyControl::SMyCNCEeprom CMyControl::_eepromFlash PROGMEM =
{
	{
		EPROM_SIGNATURE_PLOTTER,
		NUM_AXIS, MYNUM_AXIS, offsetof(CConfigEeprom::SCNCEeprom,Axis), sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions),
		COMMANDSYNTAX_CLEAR(GetInfo1a()) | COMMANDSYNTAX_VALUE(CConfigEeprom::HPGL), GetInfo1b(),
		0,
		STEPPERDIRECTION, 0, 0,SPINDLE_FADETIMEDELAY,
		SPINDLE_MAXSPEED,
		CNC_JERKSPEED,
		CNC_MAXSPEED,
		CNC_ACC,
		CNC_DEC,
		STEPRATE_REFMOVE,
		MOVEAWAYFROMREF_MM1000,
		X_STEPSPERMM / 1000.0,
		{
			{ AXISPARAMETER(X_MAXSIZE, X_USEREFERENCE, REFMOVE_1_AXIS, X_REFERENCEHITVALUE_MIN, X_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
			{ AXISPARAMETER(Y_MAXSIZE, Y_USEREFERENCE, REFMOVE_2_AXIS, Y_REFERENCEHITVALUE_MIN, Y_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
			{ AXISPARAMETER(Z_MAXSIZE, Z_USEREFERENCE, REFMOVE_3_AXIS, Z_REFERENCEHITVALUE_MIN, Z_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#if NUM_AXIS > 3
		{ AXISPARAMETER(A_MAXSIZE,     A_USEREFERENCE, REFMOVE_4_AXIS,  A_REFERENCEHITVALUE_MIN, A_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
#if NUM_AXIS > 4
		{ AXISPARAMETER(B_MAXSIZE,     B_USEREFERENCE, REFMOVE_5_AXIS,  B_REFERENCEHITVALUE_MIN, B_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
#if NUM_AXIS > 5
		{ AXISPARAMETER(C_MAXSIZE,     C_USEREFERENCE, REFMOVE_6_AXIS,  C_REFERENCEHITVALUE_MIN, C_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
		}
	},
	// Plotter EEprom Extension

	PLOTTER_DEFAULT_PENDOWN_FEEDRATE,
	PLOTTER_DEFAULT_PENUP_FEEDRATE,

	PLOTTER_DEFAULT_Z_PENDOWN_FEEDRATE,
	PLOTTER_DEFAULT_Z_PENUP_FEEDRATE,
	PLOTTER_DEFAULT_Z_PENCHANGE_FEEDRATE,

	PLOTTER_PENDOWNPOS_Z,
	PLOTTER_PENUPPOS_Z,

	PLOTTER_PENCHANGEPOS_X,
	PLOTTER_PENCHANGEPOS_Y,
	PLOTTER_PENCHANGEPOS_Z,

	PLOTTER_PENCHANGEPOS_X_OFS,
	PLOTTER_PENCHANGEPOS_Y_OFS,

	SERVO1_CLAMPOPEN,SERVO1_CLAMPCLOSE,
	SERVO1_CLAMPOPENDELAY,SERVO1_CLAMPCLOSEDELAY
};

////////////////////////////////////////////////////////////

void CMyControl::Init()
{
	CSingleton<CConfigEeprom>::GetInstance()->Init(sizeof(CMyControl::SMyCNCEeprom), &_eepromFlash, EPROM_SIGNATURE_PLOTTER);

#ifdef DISABLELEDBLINK
	DisableBlinkLed();
#endif

	super::Init();

	InitFromEeprom();

	_data.Init();

	CHPGLParser::Init();
	Plotter.Init();

	_servo1.attach(SERVO1_PIN);

#if PENTYPE == PENTYPE_SERVO
	_servo2.attach(SERVO2_PIN);
#endif

	InitSD(SD_ENABLE_PIN);
}

////////////////////////////////////////////////////////////

void CMyControl::PrintVersion()
{
  super::PrintVersion();
  StepperSerial.println(MESSAGE_MYCONTROL_VERSION);
}

////////////////////////////////////////////////////////////

void CMyControl::IOControl(uint8_t tool, uint16_t level)
{
	switch (tool)
	{
		// @formatter:off � disable formatter after this line
		case Servo1: _servo1.write(level);	return;
		case Servo2: _servo2.write(level);	return;
			// @formatter:on � enable formatter after this line
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
#ifndef REDUCED_SIZE
		case SpindleCW:
		case SpindleCCW: { return _data._spindle.IsOn(); }
		case Coolant: { return _data._coolant.IsOn(); }
		case ControllerFan: { return _data._controllerfan.GetLevel(); }
#endif
		case Probe: { return _data._probe.IsOn(); }

		case Servo1: return uint16_t(_servo1.readMicroseconds());
		case Servo2: return uint16_t(_servo2.readMicroseconds());
	}

	return super::IOControl(tool);
}

////////////////////////////////////////////////////////////

void CMyControl::Kill()
{
	super::Kill();
	_data.Kill();
}

////////////////////////////////////////////////////////////

bool CMyControl::IsKill()
{
	if (_data.IsKill())
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
	_servo1.write(SERVO1_CLAMPOPEN);
	Plotter.Initialized();
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
	return super::OnEvent(eventType, addInfo);
}

////////////////////////////////////////////////////////////

void CMyControl::Idle(unsigned int idleTime)
{
	Plotter.Idle(idleTime);
	super::Idle(idleTime);
}

////////////////////////////////////////////////////////////

bool CMyControl::Parse(CStreamReader* reader, Stream* output)
{
	if (reader->GetCharToUpper() == 0x1b)			// escape
	{
		reader->GetNextChar();
		return super::Parse(reader, output);
	}

	switch (reader->GetChar())
	{
		case '$':
		case '?':
		case '&': return super::Parse(reader, output);
		default: break;
	}

	CHPGLParser hpgl(reader, output);
	return ParseAndPrintResult(&hpgl, output);
}

////////////////////////////////////////////////////////////
