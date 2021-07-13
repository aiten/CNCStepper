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

#include <MotionControl.h>

////////////////////////////////////////////////////////

class CMyMotionControl : public CMotionControl
{
private:

	typedef CMotionControl super;

public:

	CMyMotionControl();

	void PrintInfo();

	void         MoveAngle(const mm1000_t    dest[NUM_AXIS]);
	void         MoveAngleLog(const mm1000_t dest[NUM_AXIS]);
	virtual void MoveAbs(const mm1000_t      to[NUM_AXIS], feedrate_t feedRate) override;

protected:

	virtual void TransformFromMachinePosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS]) override;
	virtual bool TransformPosition(const mm1000_t           src[NUM_AXIS], mm1000_t dest[NUM_AXIS]) override;

	steprate_t GetStepRate(const mm1000_t to[NUM_AXIS], const udist_t to_m[NUM_AXIS], feedrate_t feedrate) const override;

private:

	static bool ToAngle(const mm1000_t pos[NUM_AXIS], float      angle[NUM_AXIS]);
	static bool FromAngle(const float  angle[NUM_AXIS], mm1000_t dest[NUM_AXIS]);

	static void AdjustToAngle(float   angle[NUM_AXIS]);
	static void AdjustFromAngle(float angle[NUM_AXIS]);

#ifdef _MSC_VER

public:

	virtual void UnitTest() override;
	bool         Test(mm1000_t dest1, mm1000_t dest2, mm1000_t dest3, bool printOK);

#endif
};

////////////////////////////////////////////////////////
