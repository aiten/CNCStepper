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

#define MASH6050S_ENDSTOPCOUNT 4
#define MASH6050S_CHANGEDIRECTIONMICROS	5

#if defined(__SAM3X8E__)
#define USESTEPTIMER
#define STEPTIMERDELAYINMICRO 2
#endif

////////////////////////////////////////////////////////

class CStepperMash6050S : public CStepper
{
private:
	typedef CStepper super;
public:

	////////////////////////////////////////////////////////

	CStepperMash6050S()
	{
		_numAxes = 4;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

		_pod._idleLevel = LevelMax;		// no Idle

		CHAL::pinModeOutput(MASH6050S_X_STEP_PIN);
		CHAL::pinModeOutput(MASH6050S_X_DIR_PIN);
		CHAL::pinMode(MASH6050S_X_MIN_PIN, MASH6050S_INPUTPINMODE);

		CHAL::pinModeOutput(MASH6050S_Y_STEP_PIN);
		CHAL::pinModeOutput(MASH6050S_Y_DIR_PIN);
		CHAL::pinMode(MASH6050S_Y_MIN_PIN, MASH6050S_INPUTPINMODE);

		CHAL::pinModeOutput(MASH6050S_Z_STEP_PIN);
		CHAL::pinModeOutput(MASH6050S_Z_DIR_PIN);
		CHAL::pinMode(MASH6050S_Z_MAX_PIN, MASH6050S_INPUTPINMODE);

		CHAL::pinModeOutput(MASH6050S_C_STEP_PIN);
		CHAL::pinModeOutput(MASH6050S_C_DIR_PIN);
		CHAL::pinMode(MASH6050S_C_MIN_PIN, MASH6050S_INPUTPINMODE);


		ClearStepPin();
		SetDirection(0);

#ifdef USESTEPTIMER
		CHAL::InitTimer2OneShot(HandleStepPinInterrupt);
#endif
	}

protected:

	////////////////////////////////////////////////////////

	virtual void SetEnable(axis_t, uint8_t, bool) override { }

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t) override
	{
		return LevelMax;
	}

	////////////////////////////////////////////////////////

#if defined(__SAM3X8E__) || defined(__SAMD21G18A__)

	static void Delay1() ALWAYSINLINE { CHAL::DelayMicroseconds(2); }
	static void Delay2() ALWAYSINLINE { CHAL::DelayMicroseconds(2); }

#else //AVR

	static void Delay1() ALWAYSINLINE { CHAL::DelayMicroseconds(1); }
	static void Delay2() ALWAYSINLINE { CHAL::DelayMicroseconds(1); }

#endif

	////////////////////////////////////////////////////////

	static void SetDirection(axisArray_t directionUp)
	{
		// @formatter:off — disable formatter after this line
		if ((directionUp&(1 << X_AXIS)) != 0) HALFastdigitalWriteNC(MASH6050S_X_DIR_PIN, MASH6050S_PIN_DIR_OFF); else HALFastdigitalWriteNC(MASH6050S_X_DIR_PIN, MASH6050S_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0) HALFastdigitalWriteNC(MASH6050S_Y_DIR_PIN, MASH6050S_PIN_DIR_OFF); else HALFastdigitalWriteNC(MASH6050S_Y_DIR_PIN, MASH6050S_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0) HALFastdigitalWriteNC(MASH6050S_Z_DIR_PIN, MASH6050S_PIN_DIR_OFF); else HALFastdigitalWriteNC(MASH6050S_Z_DIR_PIN, MASH6050S_PIN_DIR_ON);
		if ((directionUp&(1 << A_AXIS)) != 0) HALFastdigitalWriteNC(MASH6050S_C_DIR_PIN, MASH6050S_PIN_DIR_OFF); else HALFastdigitalWriteNC(MASH6050S_C_DIR_PIN, MASH6050S_PIN_DIR_ON);
		// @formatter:on — enable formatter after this line
	}

	////////////////////////////////////////////////////////

	static void SetStepPin(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(MASH6050S_X_STEP_PIN, MASH6050S_PIN_STEP_OFF); }
		if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(MASH6050S_Y_STEP_PIN, MASH6050S_PIN_STEP_OFF); }
		if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(MASH6050S_Z_STEP_PIN, MASH6050S_PIN_STEP_OFF); }
		if (steps[A_AXIS] > cnt) { HALFastdigitalWriteNC(MASH6050S_C_STEP_PIN, MASH6050S_PIN_STEP_OFF); }
	}

	////////////////////////////////////////////////////////

	static void ClearStepPin()
	{
		HALFastdigitalWriteNC(MASH6050S_X_STEP_PIN, MASH6050S_PIN_STEP_ON);
		HALFastdigitalWriteNC(MASH6050S_Y_STEP_PIN, MASH6050S_PIN_STEP_ON);
		HALFastdigitalWriteNC(MASH6050S_Z_STEP_PIN, MASH6050S_PIN_STEP_ON);
		HALFastdigitalWriteNC(MASH6050S_C_STEP_PIN, MASH6050S_PIN_STEP_ON);
	}

	////////////////////////////////////////////////////////

#ifdef USESTEPTIMER

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		while (_setState != NextIsDone)
		{
			_setState = MyStep(_mySteps, (EnumAsByte(ESetPinState)) _setState, _myCnt);
			Delay1();
		}

		InitStepDirTimer(steps);
		if (!isSameDirection)
		{
			SetDirection(directionUp);
			if (MASH6050S_CHANGEDIRECTIONMICROS)
			{
				CHAL::StartTimer2OneShot(TIMER2VALUEFROMMICROSEC(MASH6050S_CHANGEDIRECTIONMICROS));
				// wait until interrupt
				return;
			}
		}
		HandleStepPinInterrupt();
	}

	static void HandleStepPinInterrupt()
	{
		_setState = MyStep(_mySteps, (EnumAsByte(ESetPinState)) _setState, _myCnt);
		if (_setState == NextIsDone)
			CHAL::StopTimer2();
		else
			CHAL::StartTimer2OneShot(TIMER2VALUEFROMMICROSEC(STEPTIMERDELAYINMICRO));
	}

	static EnumAsByte(ESetPinState) MyStep(const uint8_t steps[NUM_AXIS], EnumAsByte(ESetPinState) state, uint8_t& cnt)
	{
		if (state == NextIsSetPin)
		{
			SetStepPin(steps, cnt);
			cnt++;

			state = AnyPendingAxis(steps, cnt) ? NextIsClearPin : NextIsClearDonePin;
		}
		else
		{
			ClearStepPin();
			state = state == NextIsClearPin ? NextIsSetPin : NextIsDone;
		}
		return state;
	}

	static bool AnyPendingAxis(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		// optimize => not used as "Delay1
		return steps[X_AXIS] > cnt || steps[Y_AXIS] > cnt || steps[Z_AXIS] > cnt || steps[A_AXIS] > cnt;
	}

#else

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		if (!isSameDirection)
		{
			SetDirection(directionUp);
			if (MASH6050S_CHANGEDIRECTIONMICROS)
			{
				CHAL::DelayMicroseconds(MASH6050S_CHANGEDIRECTIONMICROS);
			}
		}

		for (uint8_t cnt = 0;;)
		{
			SetStepPin(steps, cnt);
			cnt++;

			// AVR: GetPendingAxis will take 6-8 machine cycle per axis: 0.5 us
			//      Delay1() may be very short (=0 if axis >= 2)
			uint8_t pending = GetPendingAxis(steps, cnt);
			Delay1();

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
		if (steps[A_AXIS] > cnt) { pending++; }

		return pending;
	}

#endif

	////////////////////////////////////////////////////////

public:

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override
	{
		switch (referenceId)
		{
			case 0: return HALFastdigitalRead(MASH6050S_X_MIN_PIN);
			case 2: return HALFastdigitalRead(MASH6050S_Y_MIN_PIN);
			case 5: return HALFastdigitalRead(MASH6050S_Z_MAX_PIN);
			case 6: return HALFastdigitalRead(MASH6050S_C_MIN_PIN);
			default: break;
		}
		return 255;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		return
			_pod._referenceHitValue[0] == HALFastdigitalRead(MASH6050S_X_MIN_PIN) ||
			_pod._referenceHitValue[2] == HALFastdigitalRead(MASH6050S_Y_MIN_PIN) ||
			_pod._referenceHitValue[5] == HALFastdigitalRead(MASH6050S_Z_MAX_PIN) ||
			_pod._referenceHitValue[6] == HALFastdigitalRead(MASH6050S_C_MIN_PIN);
	}

	////////////////////////////////////////////////////////
};
