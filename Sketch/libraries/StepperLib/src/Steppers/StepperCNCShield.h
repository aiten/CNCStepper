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

#define CNCSHIELD_ENDSTOPCOUNT 3

////////////////////////////////////////////////////////

class CStepperCNCShield : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperCNCShield()
	{
		_num_axis = CNCSHIELD_NUM_AXIS;
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

	virtual void  SetEnable(axis_t /* axis */, uint8_t level, bool /* force */) override
	{
		if (level != LevelOff)	HALFastdigitalWrite(CNCSHIELD_ENABLE_PIN,CNCSHIELD_PIN_ENABLE_ON);	else	HALFastdigitalWrite(CNCSHIELD_ENABLE_PIN,CNCSHIELD_PIN_ENABLE_OFF);
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
		if ((directionUp&(1 << X_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_X_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_X_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
		if ((directionUp&(1 << Y_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_Y_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_Y_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
		if ((directionUp&(1 << Z_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_Z_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_Z_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
#if CNCSHIELD_NUM_AXIS > 3
		if ((directionUp&(1 << A_AXIS)) != 0) HALFastdigitalWriteNC(CNCSHIELD_A_DIR_PIN, CNCSHIELD_PIN_DIR_OFF); else HALFastdigitalWriteNC(CNCSHIELD_A_DIR_PIN, CNCSHIELD_PIN_DIR_ON);
#endif
	}

	////////////////////////////////////////////////////////

	static uint8_t SetStepPin(const uint8_t steps[NUM_AXIS], uint8_t cnt)
	{
		uint8_t pending = 0;
		if (steps[X_AXIS]) { pending += steps[X_AXIS] - cnt;  HALFastdigitalWriteNC(CNCSHIELD_X_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
		if (steps[Y_AXIS]) { pending += steps[Y_AXIS] - cnt;  HALFastdigitalWriteNC(CNCSHIELD_Y_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
		if (steps[Z_AXIS]) { pending += steps[Z_AXIS] - cnt;  HALFastdigitalWriteNC(CNCSHIELD_Z_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
#if CNCSHIELD_NUM_AXIS > 3
		if (steps[A_AXIS]) { pending += steps[A_AXIS] - cnt; HALFastdigitalWriteNC(CNCSHIELD_A_STEP_PIN, CNCSHIELD_PIN_STEP_OFF); }
#endif

		return pending;
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

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		// The timing requirements for minimum pulse durations on the STEP pin are different for the two drivers. 
		// With the DRV8825, the high and low STEP pulses must each be at least 1.9 us; 
		// they can be as short as 1 us when using the A4988.

#ifdef USESTEPTIMER

		InitStepDirTimer(steps);
		if (!isSameDirection)
		{
			SetDirection(directionUp);
		}
		HandleStepPinInterrupt();
		CHAL::StartTimer2(TIMER2VALUEFROMMICROSEC(STEPTIMERDELAYINMICRO));

#else

		if (!isSameDirection)
		{
			SetDirection(directionUp);
		}

		for (uint8_t cnt = 1;; cnt++)
		{
			uint8_t pending = SetStepPin(steps, cnt);
			Delay1(CNCSHIELD_NUM_AXIS);
			ClearStepPin();

			if (pending == 0)
				break;

			Delay2();
		}
#endif
	}

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceid) override
	{
		// min and max is the same pin
		switch (referenceid)
		{
			case 0:
			case 1: return HALFastdigitalRead(CNCSHIELD_X_MIN_PIN);
			case 2:
			case 3: return HALFastdigitalRead(CNCSHIELD_Y_MIN_PIN);
			case 4:
			case 5: return HALFastdigitalRead(CNCSHIELD_Z_MIN_PIN);
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
