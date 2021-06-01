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

#include "Stepper.h"

////////////////////////////////////////////////////////

#define CNCSHIELD_ENDSTOPCOUNT 3

////////////////////////////////////////////////////////

class CStepperCNCShield : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperCNCShield()
	{
		_numAxes = CNCSHIELD_NUM_AXIS;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

		CHAL::pinModeOutput(CNCSHIELD_ENABLE_PIN);

		CHAL::pinModeOutput(CNCSHIELD_X_STEP_PIN);
		CHAL::pinModeOutput(CNCSHIELD_X_DIR_PIN);
		CHAL::pinModeInputPullUp(CNCSHIELD_X_MIN_PIN);

		CHAL::pinModeOutput(CNCSHIELD_Y_STEP_PIN);
		CHAL::pinModeOutput(CNCSHIELD_Y_DIR_PIN);
		CHAL::pinModeInputPullUp(CNCSHIELD_Y_MIN_PIN);

		CHAL::pinModeOutput(CNCSHIELD_Z_STEP_PIN);
		CHAL::pinModeOutput(CNCSHIELD_Z_DIR_PIN);
		CHAL::pinModeInputPullUp(CNCSHIELD_Z_MIN_PIN);

		HALFastdigitalWrite(CNCSHIELD_X_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
		HALFastdigitalWrite(CNCSHIELD_Y_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
		HALFastdigitalWrite(CNCSHIELD_Z_STEP_PIN, CNCSHIELD_PIN_STEP_ON);

#if CNCSHIELD_NUM_AXIS > 3

		CHAL::pinModeOutput(CNCSHIELD_A_STEP_PIN);
		CHAL::pinModeOutput(CNCSHIELD_A_DIR_PIN);

		HALFastdigitalWrite(CNCSHIELD_A_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
#endif

		SetDirection(0);

#ifdef USESTEPTIMER
		CHAL::InitTimer2(HandleStepPinInterrupt);
#endif
	}

protected:

	////////////////////////////////////////////////////////

	virtual void SetEnable(axis_t /* axis */, uint8_t level, bool /* force */) override
	{
		if (level != LevelOff)
		{
			HALFastdigitalWrite(CNCSHIELD_ENABLE_PIN, CNCSHIELD_PIN_ENABLE_ON);
		}
		else
		{
			HALFastdigitalWrite(CNCSHIELD_ENABLE_PIN, CNCSHIELD_PIN_ENABLE_OFF);
		}
	}

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t /* axis */) override
	{
		return ConvertLevel(HALFastdigitalRead(CNCSHIELD_ENABLE_PIN) == CNCSHIELD_PIN_ENABLE_ON);
	}

	////////////////////////////////////////////////////////

#if defined(CNCLIB_USE_A4998)
#define USE_A4998
#else
#undef USE_A4998
#endif
#include "StepperA4998_DRV8825.h"

	////////////////////////////////////////////////////////

	static void SetDirection(axisArray_t directionUp)
	{
		// @formatter:off — disable formatter after this line
		if ((directionUp&(1 << X_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_X_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_X_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_Y_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_Y_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_Z_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_Z_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
#if CNCSHIELD_NUM_AXIS > 3
		if ((directionUp&(1 << A_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_A_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_A_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
#endif
		// @formatter:on — enable formatter after this line
	}

	////////////////////////////////////////////////////////

	static void SetStepPin(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(CNCSHIELD_X_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
		if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(CNCSHIELD_Y_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
		if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(CNCSHIELD_Z_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
#if CNCSHIELD_NUM_AXIS > 3
		if (steps[A_AXIS] > cnt) { HALFastdigitalWriteNC(CNCSHIELD_A_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
#endif
	}

	////////////////////////////////////////////////////////

	static void ClearStepPin()
	{
		HALFastdigitalWriteNC(CNCSHIELD_X_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
		HALFastdigitalWriteNC(CNCSHIELD_Y_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
		HALFastdigitalWriteNC(CNCSHIELD_Z_STEP_PIN, CNCSHIELD_PIN_STEP_ON);
#if CNCSHIELD_NUM_AXIS > 3
		HALFastdigitalWriteNC(CNCSHIELD_A_STEP_PIN, CNCSHIELD_PIN_STEP_ON); 
#endif
	}

	////////////////////////////////////////////////////////
#ifdef USESTEPTIMER
#else

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		// The timing requirements for minimum pulse durations on the STEP pin are different for the two drivers. 
		// With the DRV8825, the high and low STEP pulses must each be at least 1.9 us; 
		// they can be as short as 1 us when using the A4988.

		if (!isSameDirection)
		{
			SetDirection(directionUp);
		}

		for (uint8_t cnt = 0;;)
		{
			SetStepPin(steps, cnt);
			cnt++;

			// AVR: GetPendingAxis will take 6-8 machine cycle per axis: 0.5 us
			//      Delay1() may be very short (=0 if axis >= 2)
			uint8_t pending = GetPendingAxis(steps, cnt);
			Delay1(CNCSHIELD_NUM_AXIS);

			ClearStepPin();

			if (pending == 0)
				break;

			Delay2();
		}
	}

	static uint8_t GetPendingAxis(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		// do not optimize => it will be used as "Delay1
		uint8_t pending = 0;
		if (steps[X_AXIS] > cnt) { pending++; }
		if (steps[Y_AXIS] > cnt) { pending++; }
		if (steps[Z_AXIS] > cnt) { pending++; }
#if CNCSHIELD_NUM_AXIS > 3
		if (steps[E0_AXIS] > cnt) { pending++;; }
#endif
		return pending;
	}
#endif

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override
	{
		// min and max is the same pin
		switch (referenceId)
		{
			case 0:
			case 1: return HALFastdigitalRead(CNCSHIELD_X_MIN_PIN);
			case 2:
			case 3: return HALFastdigitalRead(CNCSHIELD_Y_MIN_PIN);
			case 4:
			case 5: return HALFastdigitalRead(CNCSHIELD_Z_MIN_PIN);
			default: break;
		}
		return 255;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		uint8_t xref = HALFastdigitalRead(CNCSHIELD_X_MIN_PIN);
		uint8_t yref = HALFastdigitalRead(CNCSHIELD_Y_MIN_PIN);
		uint8_t zref = HALFastdigitalRead(CNCSHIELD_Z_MIN_PIN);
		// min and max is the same pin
		return
			_pod._referenceHitValue[0] == xref || _pod._referenceHitValue[1] == xref ||
			_pod._referenceHitValue[2] == yref || _pod._referenceHitValue[3] == yref ||
			_pod._referenceHitValue[4] == zref || _pod._referenceHitValue[5] == zref;
	}

	////////////////////////////////////////////////////////
};
