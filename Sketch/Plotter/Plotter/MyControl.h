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

#pragma once

////////////////////////////////////////////////////////

#include <Servo.h>

#include <Control3D.h>
#include <OnOffIOControl.h>
#include <Analog8IOControl.h>
#include <Analog8IOControlSmooth.h>
#include <ReadPinIOControl.h>
#include <PushButtonLow.h>
#include <DummyIOControl.h>
#include <ConfigEeprom.h>

#include "Configuration.h"
#include "MyLcd.h"

// must be after "Configuration.h" because of defines
#include <ControlImplementation.h>

////////////////////////////////////////////////////////

class CMyControl : public CControl3D
{
private:

	typedef CControl3D super;

public:

	CMyControl() { }

	virtual void Kill() override;

	virtual void IOControl(uint8_t tool, uint16_t level) override;
#ifndef REDUCED_SIZE
	virtual uint16_t IOControl(uint8_t tool) override;
#endif

protected:

	virtual void Init() override;
	virtual void Initialized() override;

	virtual void TimerInterrupt() override;
	virtual bool Parse(CStreamReader* reader, Stream* output) override;
	virtual void Idle(unsigned int    idleTime) override;

	virtual bool IsKill() override;
	virtual void Poll() override;

	virtual bool OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo) override;

  virtual void PrintVersion() override;

public:

	struct SMyCNCEeprom
	{
		CConfigEeprom::SCNCEeprom _std;
		feedrate_t                PenDownFeedRate;
		feedrate_t                PenUpFeedRate;

		feedrate_t MovePenDownFeedRate;
		feedrate_t MovePenUpFeedRate;
		feedrate_t MovePenChangeFeedRate;

		mm1000_t PenDownPos;
		mm1000_t PenUpPos;

		mm1000_t PenChangePos_x;
		mm1000_t PenChangePos_y;
		mm1000_t PenChangePos_z;

		mm1000_t PenChangePos_x_ofs;
		mm1000_t PenChangePos_y_ofs;

		uint16_t PenChangeServoClampOpenPos;
		uint16_t PenChangeServoClampClosePos;

		uint16_t PenChangeServoClampOpenDelay;
		uint16_t PenChangeServoClampCloseDelay;
	};

private:

	static const SMyCNCEeprom _eepromFlash;

	ControlData _data;

	Servo _servo1;
	Servo _servo2;
};

////////////////////////////////////////////////////////

extern CMyControl Control;
