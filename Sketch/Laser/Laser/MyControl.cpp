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

////////////////////////////////////////////////////////////

CMyControl            Control;
CMotionControlDefault MotionControl;
CConfigEeprom         Eprom;
#if defined (USEHARDWARESERIAL)
HardwareSerial&       StepperSerial = Serial;
#endif

////////////////////////////////////////////////////////////

#ifndef MYNUM_AXIS
#error Please define MYNUM_AXIS
#endif

////////////////////////////////////////////////////////////

const CConfigEeprom::SCNCEeprom CMyControl::_eepromFlash PROGMEM =
{
	EPROM_SIGNATURE,
	NUM_AXIS, MYNUM_AXIS, offsetof(CConfigEeprom::SCNCEeprom,Axis), sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions),
	GetInfo1a() | CConfigEeprom::IS_LASER, GetInfo1b(),
	0,
	STEPPERDIRECTION, 0, 0,SPINDLE_FADETIMEDELAY,
	SPINDLE_MAXSPEED,
	CNC_JERKSPEED,
	CNC_MAXSPEED,
	CNC_ACC,
	CNC_DEC,
	STEPRATE_REFMOVE,
	MOVEAWAYFROMREF_MM1000,
	TIMEOUTSETIDLE_DEFAULT,
	0,
	0,
	0,
	0,
	0,
  STEPSPERMM / 1000.0,
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

#ifdef BLINK_LED
	CHAL::pinModeOutput(BLINK_LED);
#endif

	super::Init();

	InitFromEeprom();

	_data.Init();

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
		default: break;
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
#ifdef BLINK_LED

	uint32_t time = millis();
	if (_timeBlink < time)
	{
		HALFastdigitalWrite(BLINK_LED, CHAL::digitalRead(BLINK_LED) == HIGH ? LOW : HIGH);
		_timeBlink = time + BLINK_TIMEOUT;
		 //CStepper::GetInstance()->Dump(CStepper::DumpAll);	
	}
#endif
}

////////////////////////////////////////////////////////////

bool CMyControl::OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo)
{
	_data.OnEvent(eventType, addInfo);
	return super::OnEvent(eventType, addInfo);
}
