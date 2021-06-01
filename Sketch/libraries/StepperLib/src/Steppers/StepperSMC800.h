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

#define SMC800_NUM_AXIS	3

////////////////////////////////////////////////////////

class CStepperSMC800 : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperSMC800();

	virtual void Init() override;

	void Remove();

public:

	virtual bool    IsAnyReference() override { return GetReferenceValue(0) == HIGH; };
	virtual uint8_t GetReferenceValue(uint8_t referenceId) override;

	void SetFullStepMode(axis_t axis, bool fullStepMode) { _fullStepMode[axis] = fullStepMode; };

protected:

	virtual void    Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override;
	virtual void    SetEnable(axis_t   axis, uint8_t                level, bool       force) override;
	virtual uint8_t GetEnable(axis_t   axis) override;

	virtual void MoveAwayFromReference(axis_t axis, sdist_t dist, steprate_t vMax) override;

	////////////////////////////////////////////////////////

private:

	uint8_t _stepIdx[SMC800_NUM_AXIS];
	uint8_t _level[SMC800_NUM_AXIS];
	bool    _fullStepMode[NUM_AXIS];

	void SetPhase(axis_t axis);

	static void OutSMC800Cmd(const uint8_t val);
};
