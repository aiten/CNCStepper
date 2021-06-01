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

class CStepperL298N : public CStepper
{
private:
	typedef CStepper super;
public:

	             CStepperL298N();
	virtual void Init() override;

protected:

	static pin_t _pin[NUM_AXIS][4];
	static pin_t _pinEnable[NUM_AXIS];
	//	static pin_t _pinEnable[NUM_AXIS][2];
	static pin_t _pinRef[NUM_AXIS * 2];

protected:

	virtual void    SetEnable(axis_t   axis, uint8_t level, bool force) override;
	virtual uint8_t GetEnable(axis_t   axis) override;
	virtual void    Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override;

public:

	virtual bool    IsAnyReference() override;
	virtual uint8_t GetReferenceValue(uint8_t referenceId) override;

	// Set before Init()
	// @formatter:off — disable formatter after this line
	void SetPin(axis_t axis, pin_t in1, pin_t in2, pin_t in3, pin_t in4)	{ _pin[axis][0] = in1;_pin[axis][1] = in2;_pin[axis][2] = in3;	_pin[axis][3] = in4;}
	void SetPin(axis_t axis, pin_t in1, pin_t in2)							{ _pin[axis][0] = in1; _pin[axis][1] = in2; _pin[axis][2] = 0; _pin[axis][3] = 0; }
	void SetRefPin(axis_t axis, pin_t refMin, pin_t refMax)					{ _pinRef[ToReferenceId(axis, true)] = refMin;	_pinRef[ToReferenceId(axis, false)] = refMax; }
	// @formatter:on — enable formatter after this line

	void SetEnablePin(axis_t axis, pin_t en) { _pinEnable[axis] = en; }
	//	void SetEnablePin(axis_t axis, pin_t en1, pin_t en2)			{ _pinEnable[axis][0] = en1;  _pinEnable[axis][1] = en2; }

	void SetFullStepMode(bool fullstepMode) { _fullStepMode = fullstepMode; };

private:

	bool IsActive(axis_t axis) { return _pin[axis][0] != 0; }
	bool Is4Pin(axis_t   axis) { return _pin[axis][2] != 0; }
	bool Is2Pin(axis_t   axis) { return _pin[axis][2] == 0; }

	bool IsUseEN1(axis_t axis) { return _pinEnable[axis] != 0; }
	//	bool IsUseEN1(axis_t axis)										{ return _pinEnable[axis][0] != 0; }
	//	bool IsUseEN2(axis_t axis)										{ return _pinEnable[axis][1] != 0; }

	uint8_t _stepIdx[NUM_AXIS];
	bool    _fullStepMode;

	void SetPhase(axis_t axis);

	void SetPhase(axis_t axis, uint8_t bitmask);

	////////////////////////////////////////////////////////
};
