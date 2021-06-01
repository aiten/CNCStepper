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

#if defined(__AVR_ATmega2560__) || defined(_MSC_VER) || defined(__SAM3X8E__)

// only available on Arduino Mega or due

////////////////////////////////////////////////////////

#define E0_AXIS A_AXIS
#define E1_AXIS B_AXIS
#define E2_AXIS C_AXIS

#define RAMPSFD_ENDSTOPCOUNT 6

#if defined(__SAM3X8E__)
#define USESTEPTIMER
#define STEPTIMERDELAYINMICRO 2
#endif

class CStepperRampsFD : public CStepper
{
private:
	typedef CStepper super;
public:

	////////////////////////////////////////////////////////

	CStepperRampsFD()
	{
		_numAxes = 6;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

		CHAL::pinModeOutput(RAMPSFD_X_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_X_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_X_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_X_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_X_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_Y_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_Y_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_Y_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Y_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Y_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_Z_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_Z_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_Z_ENABLE_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Z_MIN_PIN);
		CHAL::pinModeInputPullUp(RAMPSFD_Z_MAX_PIN);
		HALFastdigitalWrite(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_ON);

		CHAL::pinModeOutput(RAMPSFD_E0_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E0_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E0_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(RAMPSFD_E0_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(RAMPSFD_E0_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_ON);

#ifndef RAMPSFD_DISABLE_E1
		CHAL::pinModeOutput(RAMPSFD_E1_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E1_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E1_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(RAMPSFD_E1_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(RAMPSFD_E1_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_ON);
#endif

		CHAL::pinModeOutput(RAMPSFD_E2_STEP_PIN);
		CHAL::pinModeOutput(RAMPSFD_E2_DIR_PIN);
		CHAL::pinModeOutput(RAMPSFD_E2_ENABLE_PIN);
		//  CHAL::pinModeInputPullUp(E2_MIN_PIN);         
		//  CHAL::pinModeInputPullUp(E2_MAX_PIN);         
		HALFastdigitalWrite(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_ON);

#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif

		SetDirection(0);

#ifdef USESTEPTIMER
		CHAL::InitTimer2OneShot(HandleStepPinInterrupt);
#endif

		// init some outputs!

		CHAL::pinModeInputPullUp(RAMPSFD_ESTOP_PIN);

		CHAL::pinModeOutput(RAMPSFD_FET5D12_PIN);
		HALFastdigitalWrite(RAMPSFD_FET5D12_PIN, 0);
		CHAL::pinModeOutput(RAMPSFD_FET6D2_PIN);
		HALFastdigitalWrite(RAMPSFD_FET6D2_PIN, 0);
	}

	////////////////////////////////////////////////////////

protected:

	virtual void SetEnable(axis_t axis, uint8_t level, bool /* force */) override
	{
		switch (axis)
		{
			// @formatter:off — disable formatter after this line
			case X_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_X_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_X_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case Y_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_Y_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_Y_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case Z_AXIS:  if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_Z_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_Z_ENABLE_PIN,  RAMPSFD_PIN_ENABLE_OFF); break;
			case E0_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E0_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E0_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;
#ifndef RAMPSFD_DISABLE_E1
			case E1_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E1_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E1_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;
#endif
			case E2_AXIS: if (level != LevelOff)	HALFastdigitalWrite(RAMPSFD_E2_ENABLE_PIN, RAMPSFD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(RAMPSFD_E2_ENABLE_PIN, RAMPSFD_PIN_ENABLE_OFF); break;
				// @formatter:on — enable formatter after this line

			default: break;
		}
	}

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t axis) override
	{
		switch (axis)
		{
			case X_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_X_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case Y_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_Y_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case Z_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_Z_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
			case E0_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E0_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
#ifndef RAMPSFD_DISABLE_E1
			case E1_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E1_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
#endif
			case E2_AXIS: return ConvertLevel(HALFastdigitalRead(RAMPSFD_E2_ENABLE_PIN) == RAMPSFD_PIN_ENABLE_ON);
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
		if ((directionUp&(1 << X_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_X_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_X_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_Y_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_Y_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0)  HALFastdigitalWriteNC(RAMPSFD_Z_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_Z_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		if ((directionUp&(1 << E0_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E0_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E0_DIR_PIN, RAMPSFD_PIN_DIR_ON);
#ifndef RAMPSFD_DISABLE_E1
		if ((directionUp&(1 << E1_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E1_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E1_DIR_PIN, RAMPSFD_PIN_DIR_ON);
#endif
		if ((directionUp&(1 << E2_AXIS)) != 0) HALFastdigitalWriteNC(RAMPSFD_E2_DIR_PIN, RAMPSFD_PIN_DIR_OFF); else HALFastdigitalWriteNC(RAMPSFD_E2_DIR_PIN, RAMPSFD_PIN_DIR_ON);
		// @formatter:on — enable formatter after this line
	}

	////////////////////////////////////////////////////////

	static void SetStepPin(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
		if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
		if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
		if (steps[E0_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
#ifndef RAMPSFD_DISABLE_E1
		if (steps[E1_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
#endif
		if (steps[E2_AXIS] > cnt) { HALFastdigitalWriteNC(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_OFF); }
	}

	////////////////////////////////////////////////////////

	static void ClearStepPin()
	{
		HALFastdigitalWriteNC(RAMPSFD_X_STEP_PIN, RAMPSFD_PIN_STEP_ON);
		HALFastdigitalWriteNC(RAMPSFD_Y_STEP_PIN, RAMPSFD_PIN_STEP_ON);
		HALFastdigitalWriteNC(RAMPSFD_Z_STEP_PIN, RAMPSFD_PIN_STEP_ON);
		HALFastdigitalWriteNC(RAMPSFD_E0_STEP_PIN, RAMPSFD_PIN_STEP_ON);
#ifndef RAMPSFD_DISABLE_E1
		HALFastdigitalWriteNC(RAMPSFD_E1_STEP_PIN, RAMPSFD_PIN_STEP_ON); 
#endif
		HALFastdigitalWriteNC(RAMPSFD_E2_STEP_PIN, RAMPSFD_PIN_STEP_ON);
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
			Delay1(RAMPSFD_NUM_AXIS);
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

//			Delay1(RAMPSFD_NUM_AXIS);
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
			Delay1(RAMPSFD_NUM_AXIS);

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
		if (steps[E0_AXIS] > cnt) { pending++; }
#ifndef RAMPSFD_DISABLE_E1
		if (steps[E1_AXIS] > cnt) { pending++; }
#endif
		if (steps[E2_AXIS] > cnt) { pending++; }
		return pending;
	}

#endif

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override
	{
		switch (referenceId)
		{
			case 0: return HALFastdigitalRead(RAMPSFD_X_MIN_PIN);
			case 1: return HALFastdigitalRead(RAMPSFD_X_MAX_PIN);
			case 2: return HALFastdigitalRead(RAMPSFD_Y_MIN_PIN);
			case 3: return HALFastdigitalRead(RAMPSFD_Y_MAX_PIN);
			case 4: return HALFastdigitalRead(RAMPSFD_Z_MIN_PIN);
			case 5: return HALFastdigitalRead(RAMPSFD_Z_MAX_PIN);
				/* No reference for E0 & E1 & E2
				case 6: return HALFastdigitalRead(RAMPSFD_E0_MIN_PIN)==REF_ON;
				case 7: return HALFastdigitalRead(RAMPSFD_E0_MAX_PIN)==REF_ON;
				case 8: return HALFastdigitalRead(RAMPSFD_E1_MIN_PIN)==REF_ON;
				case 9: return HALFastdigitalRead(RAMPSFD_E1_MAX_PIN)==REF_ON;
				case 10:return HALFastdigitalRead(RAMPSFD_E2_MIN_PIN)==REF_ON;
				case 11:return HALFastdigitalRead(RAMPSFD_E2_MAX_PIN)==REF_ON;
				*/
			default: break;
		}
		return 255;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		return
			_pod._referenceHitValue[0] == HALFastdigitalRead(RAMPSFD_X_MIN_PIN) ||
			_pod._referenceHitValue[1] == HALFastdigitalRead(RAMPSFD_X_MAX_PIN) ||
			_pod._referenceHitValue[2] == HALFastdigitalRead(RAMPSFD_Y_MIN_PIN) ||
			_pod._referenceHitValue[3] == HALFastdigitalRead(RAMPSFD_Y_MAX_PIN) ||
			_pod._referenceHitValue[4] == HALFastdigitalRead(RAMPSFD_Z_MIN_PIN) ||
			_pod._referenceHitValue[5] == HALFastdigitalRead(RAMPSFD_Z_MAX_PIN);
	}
};

#endif
