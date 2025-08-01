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

#pragma once

////////////////////////////////////////////////////////

#include <Control3D.h>

#include <OnOffIOControl.h>
#include <Analog8IOControl.h>
#include <Analog8InvertIOControl.h>
#include <ReadPinIOControl.h>
#include <ReadPinIOTriggerControl.h>
#include <PushButtonLow.h>
#include <PushButton.h>
#include <DummyIOControl.h>
#include <ConfigEeprom.h>

#include "Configuration.h"

// must be after "Configuration.h" because of defines
#include <ControlImplementation.h>

////////////////////////////////////////////////////////

class CMyControl : public CControl3D
{
private:

	typedef CControl3D super;

public:

	CMyControl()
	{
	}

	virtual void Kill() override;

	virtual void     IOControl(uint8_t tool, uint16_t level) override;
	virtual uint16_t IOControl(uint8_t tool) override;

protected:

	virtual void Init() override;
	virtual void Initialized() override;

	virtual void TimerInterrupt() override;

	virtual bool IsKill() override;
	virtual void Poll() override;
	virtual bool GoToReference(axis_t axis, steprate_t stepRate, bool toMinRef) override;

	virtual bool OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo) override;

	virtual void PrintVersion() override;
	
private:

#ifdef BLINK_LED
	uint32_t _timeBlink=0;
#endif

	static const CConfigEeprom::SCNCEeprom _eepromFlash;

	ControlData _data;
};

////////////////////////////////////////////////////////

extern CMyControl Control;
