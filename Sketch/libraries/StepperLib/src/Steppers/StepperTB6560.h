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
//#include "StepperTB6560_Pins.h"

////////////////////////////////////////////////////////

class CStepperTB6560 : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperTB6560()
	{
		_numAxes = 4;
	}

	////////////////////////////////////////////////////////

	virtual void Init() override
	{
		super::Init();

		CHAL::pinModeOutput(TB6560_X_STEP_PIN);
		CHAL::pinModeOutput(TB6560_X_DIR_PIN);
		CHAL::pinModeOutput(TB6560_X_ENABLE_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_X_MIN_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_X_MAX_PIN);

		CHAL::pinModeOutput(TB6560_Y_STEP_PIN);
		CHAL::pinModeOutput(TB6560_Y_DIR_PIN);
		CHAL::pinModeOutput(TB6560_Y_ENABLE_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_Y_MIN_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_Y_MAX_PIN);

		CHAL::pinModeOutput(TB6560_Z_STEP_PIN);
		CHAL::pinModeOutput(TB6560_Z_DIR_PIN);
		CHAL::pinModeOutput(TB6560_Z_ENABLE_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_Z_MIN_PIN);
		//	CHAL::pinModeInputPullUp(TB6560_Z_MAX_PIN);
		/*
		CHAL::pinModeOutput(TB6560_A_STEP_PIN);
		CHAL::pinModeOutput(TB6560_A_DIR_PIN);
		CHAL::pinModeOutput(TB6560_A_ENABLE_PIN);

		CHAL::pinModeOutput(TB6560_B_STEP_PIN);
		CHAL::pinModeOutput(TB6560_B_DIR_PIN);
		CHAL::pinModeOutput(TB6560_B_ENABLE_PIN);
		*/
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

		HALFastdigitalWrite(TB6560_X_STEP_PIN, TB6560_PIN_STEP_ON);
		HALFastdigitalWrite(TB6560_Y_STEP_PIN, TB6560_PIN_STEP_ON);
		HALFastdigitalWrite(TB6560_Z_STEP_PIN, TB6560_PIN_STEP_ON);
		//	HALFastdigitalWrite(TB6560_A_STEP_PIN, TB6560_PIN_STEP_ON);
		//	HALFastdigitalWrite(TB6560_B_STEP_PIN, TB6560_PIN_STEP_ON);

#ifdef _MSC_VER
#pragma warning( default : 4127 )
#endif
	}

protected:

	////////////////////////////////////////////////////////

	virtual void SetEnable(axis_t axis, uint8_t level, bool /* force */) override
	{
#define SETLEVEL(pin) if (level != LevelOff)	HALFastdigitalWrite(pin,TB6560_PIN_ENABLE_ON);	else	HALFastdigitalWrite(pin,TB6560_PIN_ENABLE_OFF);
		switch (axis)
		{
			// @formatter:off — disable formatter after this line
			case X_AXIS: SETLEVEL(TB6560_X_ENABLE_PIN);	break;
			case Y_AXIS: SETLEVEL(TB6560_Y_ENABLE_PIN);	break;
			case Z_AXIS: SETLEVEL(TB6560_Z_ENABLE_PIN);	break;
				//		case A_AXIS: SETLEVEL(TB6560_A_ENABLE_PIN); break;
				//		case B_AXIS: SETLEVEL(TB6560_B_ENABLE_PIN); break;
			default: break;
				// @formatter:on — enable formatter after this line
		}
#undef SETLEVEL
	}

	////////////////////////////////////////////////////////

	virtual uint8_t GetEnable(axis_t axis) override
	{
		switch (axis)
		{
			case X_AXIS: return ConvertLevel(HALFastdigitalRead(TB6560_X_ENABLE_PIN) == TB6560_PIN_ENABLE_ON);
			case Y_AXIS: return ConvertLevel(HALFastdigitalRead(TB6560_Y_ENABLE_PIN) == TB6560_PIN_ENABLE_ON);
			case Z_AXIS: return ConvertLevel(HALFastdigitalRead(TB6560_Z_ENABLE_PIN) == TB6560_PIN_ENABLE_ON);
				//		case A_AXIS: return ConvertLevel(HALFastdigitalRead(TB6560_A_ENABLE_PIN) == TB6560_PIN_ENABLE_ON);
				//		case B_AXIS: return ConvertLevel(HALFastdigitalRead(TB6560_B_ENABLE_PIN) == TB6560_PIN_ENABLE_ON);
			default: break;
		}
		return 0;
	}

	////////////////////////////////////////////////////////

	virtual void Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override
	{
		// Step:   LOW to HIGH

#define SETDIR(a,dirpin)		if ((directionUp&(1<<(a))) != 0) HALFastdigitalWriteNC(dirpin,TB6560_PIN_DIR_OFF); else HALFastdigitalWriteNC(dirpin,TB6560_PIN_DIR_ON);

		SETDIR(X_AXIS, TB6560_X_DIR_PIN);
		SETDIR(Y_AXIS, TB6560_Y_DIR_PIN);
		SETDIR(Z_AXIS, TB6560_Z_DIR_PIN);
		//	SETDIR(A_AXIS,TB6560_A_DIR_PIN);
		//	SETDIR(B_AXIS,TB6560_B_DIR_PIN);

		for (uint8_t cnt = 0;; cnt++)
		{
			// @formatter:off — disable formatter after this line

			bool have = false;
			if (steps[X_AXIS] > cnt) {	HALFastdigitalWriteNC(TB6560_X_STEP_PIN, TB6560_PIN_STEP_ON);	have = true; }
			if (steps[Y_AXIS] > cnt) {	HALFastdigitalWriteNC(TB6560_Y_STEP_PIN, TB6560_PIN_STEP_ON);	have = true; }
			if (steps[Z_AXIS] > cnt) {	HALFastdigitalWriteNC(TB6560_Z_STEP_PIN, TB6560_PIN_STEP_ON);	have = true; }
			//		if (steps[A_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_A_STEP_PIN,TB6560_PIN_STEP_ON); have = true; }
			//		if (steps[B_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_B_STEP_PIN,TB6560_PIN_STEP_ON); have = true; }

			CHAL::DelayMicroseconds(7);

			if (steps[X_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_X_STEP_PIN, TB6560_PIN_STEP_OFF); }
			if (steps[Y_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_Y_STEP_PIN, TB6560_PIN_STEP_OFF); }
			if (steps[Z_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_Z_STEP_PIN, TB6560_PIN_STEP_OFF); }
			//		if (steps[A_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_A_STEP_PIN,TB6560_PIN_STEP_OFF); }
			//		if (steps[B_AXIS] > cnt) { HALFastdigitalWriteNC(TB6560_B_STEP_PIN,TB6560_PIN_STEP_OFF); }

			// @formatter:on — enable formatter after this line

			if (!have) break;

			CHAL::DelayMicroseconds(7);
		}

#undef SETDIR
	}

public:

	////////////////////////////////////////////////////////

	virtual uint8_t GetReferenceValue(uint8_t referenceId) override
	{
		/*
		switch (referenceId)
		{
		case 0: return HALFastdigitalRead(TB6560_X_MIN_PIN) == TB6560_REF_ON;
		case 1: return HALFastdigitalRead(TB6560_X_MAX_PIN) == TB6560_REF_ON;
		case 2: return HALFastdigitalRead(TB6560_Y_MIN_PIN) == TB6560_REF_ON;
		case 3: return HALFastdigitalRead(TB6560_Y_MAX_PIN) == TB6560_REF_ON;
		case 4: return HALFastdigitalRead(TB6560_Z_MIN_PIN) == TB6560_REF_ON;
		case 5: return HALFastdigitalRead(TB6560_Z_MAX_PIN) == TB6560_REF_ON;
		// No reference for A & B
		//		case 6: return HALFastdigitalRead(TB6560_A_MIN_PIN)==TB6560_REF_ON;
		//		case 7: return HALFastdigitalRead(TB6560_A_MAX_PIN)==TB6560_REF_ON;
		//		case 8: return HALFastdigitalRead(TB6560_B_MIN_PIN)==TB6560_REF_ON;
		//		case 9: return HALFastdigitalRead(TB6560_B_MAX_PIN)==TB6560_REF_ON;
		}
		*/
		return 0;
	}

	////////////////////////////////////////////////////////

	virtual bool IsAnyReference() override
	{
		return false;
		/*
		return
		(_referenceHitValue[0] && HALFastdigitalRead(TB6560_X_MIN_PIN) == TB6560_REF_ON) ||
		(_referenceHitValue[1] && HALFastdigitalRead(TB6560_X_MAX_PIN) == TB6560_REF_ON) ||
		(_referenceHitValue[2] && HALFastdigitalRead(TB6560_Y_MIN_PIN) == TB6560_REF_ON) ||
		(_referenceHitValue[3] && HALFastdigitalRead(TB6560_Y_MAX_PIN) == TB6560_REF_ON) ||
		(_referenceHitValue[4] && HALFastdigitalRead(TB6560_Z_MIN_PIN) == TB6560_REF_ON) ||
		(_referenceHitValue[5] && HALFastdigitalRead(TB6560_Z_MAX_PIN) == TB6560_REF_ON);
		*/
	}
};
