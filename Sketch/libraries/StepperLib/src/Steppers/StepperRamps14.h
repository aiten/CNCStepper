////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include "Stepper.h"

////////////////////////////////////////////////////////

#if defined(__AVR_ATmega2560__) || defined(_MSC_VER) || defined(__SAM3X8E__)

// only available on Arduino Mega or due

////////////////////////////////////////////////////////

#define E0_AXIS A_AXIS
#define E1_AXIS B_AXIS

#define RAMPS14_ENDSTOPCOUNT 6

#if defined(__SAM3X8E__)
#define USESTEPTIMER
#define STEPTIMERDELAYINMICRO 2
#endif

////////////////////////////////////////////////////////

class CStepperRamps14 : public CStepper
{
private:
	typedef CStepper super;
public:

	////////////////////////////////////////////////////////

	CStepperRamps14()
	{
		_numAxes = 5;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

		CHAL::pinModeOutput(RAMPS14_X_STEP_PIN);
		CHAL::pinModeOutput(RAMPS14_X_DIR_PIN);
		CHAL::pinModeOutput(RAMPS14_X_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_X_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_X_MAX_PIN);
		HALFastdigitalWrite(RAMPS14_X_STEP_PIN, RAMPS14_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPS14_Y_STEP_PIN);
		CHAL::pinModeOutput(RAMPS14_Y_DIR_PIN);
		CHAL::pinModeOutput(RAMPS14_Y_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_Y_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_Y_MAX_PIN);
		HALFastdigitalWrite(RAMPS14_Y_STEP_PIN, RAMPS14_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPS14_Z_STEP_PIN);
		CHAL::pinModeOutput(RAMPS14_Z_DIR_PIN);
		CHAL::pinModeOutput(RAMPS14_Z_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_Z_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPS14_Z_MAX_PIN);
		HALFastdigitalWrite(RAMPS14_Z_STEP_PIN, RAMPS14_PIN_STEP_ON);

#if RAMPS14_NUM_AXIS > 3
		CHAL::pinModeOutput(RAMPS14_E0_STEP_PIN);
		CHAL::pinModeOutput(RAMPS14_E0_DIR_PIN);
		CHAL::pinModeOutput(RAMPS14_E0_ENABLE_PIN);
		HALFastdigitalWrite(RAMPS14_E0_STEP_PIN, RAMPS14_PIN_STEP_ON);

#if RAMPS14_NUM_AXIS > 4
		CHAL::pinModeOutput(RAMPS14_E1_STEP_PIN);
		CHAL::pinModeOutput(RAMPS14_E1_DIR_PIN);
		CHAL::pinModeOutput(RAMPS14_E1_ENABLE_PIN);
		HALFastdigitalWrite(RAMPS14_E1_STEP_PIN, RAMPS14_PIN_STEP_ON);

#endif
#endif


#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif

		SetDirection(0);

#ifdef USESTEPTIMER
		CHAL::InitTimer2OneShot(HandleStepPinInterrupt);
#endif
	}

	////////////////////////////////////////////////////////

protected:

	virtual void SetEnable(axis_t axis, uint8_t level, bool /* force */) override
	{
		switch (axis)
		{
			// @formatter:off — disable formatter after this line
			case X_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPS14_X_ENABLE_PIN, RAMPS14_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPS14_X_ENABLE_PIN, RAMPS14_PIN_ENABLE_OFF); break;
			case Y_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPS14_Y_ENABLE_PIN, RAMPS14_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPS14_Y_ENABLE_PIN, RAMPS14_PIN_ENABLE_OFF); break;
			case Z_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPS14_Z_ENABLE_PIN, RAMPS14_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPS14_Z_ENABLE_PIN, RAMPS14_PIN_ENABLE_OFF); break;
#if RAMPS14_NUM_AXIS > 3
			case E0_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPS14_E0_ENABLE_PIN, RAMPS14_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPS14_E0_ENABLE_PIN, RAMPS14_PIN_ENABLE_OFF); break;
#if RAMPS14_NUM_AXIS > 4
			case E1_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPS14_E1_ENABLE_PIN, RAMPS14_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPS14_E1_ENABLE_PIN, RAMPS14_PIN_ENABLE_OFF); break;
#endif
#endif
			default: break;
				// @formatter:on — enable formatter after this line
		}
	}

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t axis) override
	{
		switch (axis)
		{
			case X_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPS14_X_ENABLE_PIN) == RAMPS14_PIN_ENABLE_ON);
			case Y_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPS14_Y_ENABLE_PIN) == RAMPS14_PIN_ENABLE_ON);
			case Z_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPS14_Z_ENABLE_PIN) == RAMPS14_PIN_ENABLE_ON);
#if RAMPS14_NUM_AXIS > 3
			case E0_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPS14_E0_ENABLE_PIN) == RAMPS14_PIN_ENABLE_ON);
#if RAMPS14_NUM_AXIS > 4
			case E1_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPS14_E1_ENABLE_PIN) == RAMPS14_PIN_ENABLE_ON);
#endif
#endif
			default: break;
		}
		return LevelOff;
	}

	////////////////////////////////////////////////////////

#if defined(RAMPS14_USE_A4998)
		#define USE_A4998
#else
#undef USE_A4998
#endif
#include "StepperA4998_DRV8825.h"

	////////////////////////////////////////////////////////

	static void SetDirection(axisArray_t directionUp)
	{
		// @formatter:off — disable formatter after this line
		if ((directionUp&(1 << X_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPS14_X_DIR_PIN, RAMPS14_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPS14_X_DIR_PIN, RAMPS14_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPS14_Y_DIR_PIN, RAMPS14_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPS14_Y_DIR_PIN, RAMPS14_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPS14_Z_DIR_PIN, RAMPS14_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPS14_Z_DIR_PIN, RAMPS14_PIN_DIR_ON);
#if RAMPS14_NUM_AXIS > 3
		if ((directionUp&(1 << E0_AXIS)) != 0) HALFastdigitalWriteNC(RAMPS14_E0_DIR_PIN, RAMPS14_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPS14_E0_DIR_PIN, RAMPS14_PIN_DIR_ON);
#if RAMPS14_NUM_AXIS > 4
		if ((directionUp&(1 << E1_AXIS)) != 0) HALFastdigitalWriteNC(RAMPS14_E1_DIR_PIN, RAMPS14_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPS14_E1_DIR_PIN, RAMPS14_PIN_DIR_ON);
#endif
#endif
		// @formatter:on — enable formatter after this line
	}

	////////////////////////////////////////////////////////

	static void SetStepPin(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPS14_X_STEP_PIN, RAMPS14_PIN_STEP_OFF); }
		if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPS14_Y_STEP_PIN, RAMPS14_PIN_STEP_OFF); }
		if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPS14_Z_STEP_PIN, RAMPS14_PIN_STEP_OFF); }
#if RAMPS14_NUM_AXIS > 3
		if (steps[E0_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPS14_E0_STEP_PIN, RAMPS14_PIN_STEP_OFF); }
#if RAMPS14_NUM_AXIS > 4
		if (steps[E1_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPS14_E1_STEP_PIN, RAMPS14_PIN_STEP_OFF); }
#endif
#endif
	}

	////////////////////////////////////////////////////////

	static void ClearStepPin()
	{
		HALFastdigitalWriteNC(RAMPS14_X_STEP_PIN, RAMPS14_PIN_STEP_ON);
		HALFastdigitalWriteNC(RAMPS14_Y_STEP_PIN, RAMPS14_PIN_STEP_ON);
		HALFastdigitalWriteNC(RAMPS14_Z_STEP_PIN, RAMPS14_PIN_STEP_ON);
#if RAMPS14_NUM_AXIS > 3
		HALFastdigitalWriteNC(RAMPS14_E0_STEP_PIN, RAMPS14_PIN_STEP_ON);
#if RAMPS14_NUM_AXIS > 4
		HALFastdigitalWriteNC(RAMPS14_E1_STEP_PIN, RAMPS14_PIN_STEP_ON);
#endif
#endif
	}

	////////////////////////////////////////////////////////

#ifdef USESTEPTIMER

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		// The timing requirements for minimum pulse durations on the STEP pin are different for the two drivers. 
		// With the DRV8825, the high and low STEP pulses must each be at least 1.9 us; 
		// they can be as short as 1 us when using the A4988.

		while (_setState != NextIsDone)
		{
			_setState = MyStep(_mySteps, (EnumAsByte(ESetPinState)) _setState, _myCnt);
			Delay1(RAMPS14_NUM_AXIS);
		}

		InitStepDirTimer(steps);
		if (!isSameDirection)
		{
			SetDirection(directionUp);
			if (A4998DRV8825_CHANGEDIRECTIONMICROS)
			{
				CHAL::StartTimer2OneShot(TIMER2VALUEFROMMICROSEC(A4998DRV8825_CHANGEDIRECTIONMICROS));
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

//			Delay1(RAMPS14_NUM_AXIS);
//			ClearStepPin();
//			state = state == NextIsClearPin ? NextIsSetPin : NextIsDone;
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
		return steps[X_AXIS] > cnt || steps[Y_AXIS] > cnt || steps[Z_AXIS] > cnt
#if RAMPS14_NUM_AXIS > 3
			|| steps[E0_AXIS] > cnt
#if RAMPS14_NUM_AXIS > 4
			|| steps[E1_AXIS] > cnt
#endif
#endif
			;
	}

#else

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		// The timing requirements for minimum pulse durations on the STEP pin are different for the two drivers. 
		// With the DRV8825, the high and low STEP pulses must each be at least 1.9 us; 
		// they can be as short as 1 us when using the A4988.

		if (!isSameDirection)
		{
			SetDirection(directionUp);
			if (A4998DRV8825_CHANGEDIRECTIONMICROS)
			{
				CHAL::DelayMicroseconds(A4998DRV8825_CHANGEDIRECTIONMICROS);
			}
		}

		for (uint8_t cnt = 0;;)
		{
			SetStepPin(steps, cnt);
			cnt++;

			// AVR: GetPendingAxis will take 6-8 machine cycle per axis: 0.5 us
			//      Delay1() may be very short (=0 if axis >= 2)
			uint8_t pending = GetPendingAxis(steps, cnt);
			Delay1(RAMPS14_NUM_AXIS);

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
#if RAMPS14_NUM_AXIS > 3
		if (steps[E0_AXIS] > cnt) { pending++;; }
#if RAMPS14_NUM_AXIS > 4
		if (steps[E1_AXIS] > cnt) { pending++;; }
#endif
#endif
		return pending;
	}

#endif

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override
	{
		switch (referenceId)
		{
			case 0: return HALFastdigitalRead(RAMPS14_X_MIN_PIN);
			case 1: return HALFastdigitalRead(RAMPS14_X_MAX_PIN);
			case 2: return HALFastdigitalRead(RAMPS14_Y_MIN_PIN);
			case 3: return HALFastdigitalRead(RAMPS14_Y_MAX_PIN);
			case 4: return HALFastdigitalRead(RAMPS14_Z_MIN_PIN);
			case 5: return HALFastdigitalRead(RAMPS14_Z_MAX_PIN);
				/* No reference for E0 & E1
				case 6: return HALFastdigitalRead(RAMPS14_E0_MIN_PIN)==RAMPS14_REF_ON;
				case 7: return HALFastdigitalRead(RAMPS14_E0_MAX_PIN)==RAMPS14_REF_ON;
				case 8: return HALFastdigitalRead(RAMPS14_E1_MIN_PIN)==RAMPS14_REF_ON;
				case 9: return HALFastdigitalRead(RAMPS14_E1_MAX_PIN)==RAMPS14_REF_ON;
				*/
			default: break;
		}
		return 255;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		return
			_pod._referenceHitValue[0] == HALFastdigitalRead(RAMPS14_X_MIN_PIN) ||
			_pod._referenceHitValue[1] == HALFastdigitalRead(RAMPS14_X_MAX_PIN) ||
			_pod._referenceHitValue[2] == HALFastdigitalRead(RAMPS14_Y_MIN_PIN) ||
			_pod._referenceHitValue[3] == HALFastdigitalRead(RAMPS14_Y_MAX_PIN) ||
			_pod._referenceHitValue[4] == HALFastdigitalRead(RAMPS14_Z_MIN_PIN) ||
			_pod._referenceHitValue[5] == HALFastdigitalRead(RAMPS14_Z_MAX_PIN);
	}

	////////////////////////////////////////////////////////
};

#endif
