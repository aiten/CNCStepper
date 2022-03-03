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

#include <limits.h>
#include "Stepper.h"

////////////////////////////////////////////////////////

// Convert between logical and physical "steps"
//
// Cartesian coordinate system
// Use ToMachine, ToMm100 to convert between same aligned machine and logical cartesian coordinate system

////////////////////////////////////////////////////////

#define STEPRATETOFEEDRATE(a) (CMotionControlBase::GetInstance()->ToMm1000(0,(a)*60l))
#define FEEDRATETOSTEPRATE(a) (CMotionControlBase::GetInstance()->ToMachine(0,(a)/60l))

typedef mm1000_t (*ToMm1000_t)(axis_t axis, sdist_t val);
typedef sdist_t (* ToMachine_t)(axis_t axis, mm1000_t val);

////////////////////////////////////////////////////////

// machine-pos									=> physical steps off stepper		(of CStepper class)
// machine-mm1000, standardized-machine-pos 	=> mm100_t off stepper				(CStepper pso converted in mm1000)
// logical-pos			 						=> rotated or transformed position	(CMotionControlBase same as above)

////////////////////////////////////////////////////////

class CMotionControlBase : public CSingleton<CMotionControlBase>
{
	////////////////////////////////////////
	// converting machine-pos to machine-mm1000

public:
#ifdef REDUCED_SIZE
	#define STEPSPERMM1000_SIZE 1
#else
#define STEPSPERMM1000_SIZE NUM_AXIS
#endif

	static void InitConversion(ToMm1000_t toMm1000, ToMachine_t toMachine)
	{
		_ToMm1000  = toMm1000;
		_ToMachine = toMachine;
	}

	static void InitConversionStepsPer(float stepspermm1000)
	{
		InitConversion(ToMm1000_StepsPer, ToMachine_StepsPer);
		for (float& axis_stepsPerMm : StepsPerMm1000)
		{
			axis_stepsPerMm = stepspermm1000;
		}
	}

	static void InitConversionBestStepsPer(float stepspermm1000);
	static void SetConversionStepsPerEx() { InitConversion(ToMm1000_StepsPerEx, ToMachine_StepsPerEx); }

	static void SetConversionStepsPerEx(axis_t axis, float stepspermm1000)
	{
		StepsPerMm1000[axis] = stepspermm1000;;
	}

	static mm1000_t ToMm1000(axis_t axis, sdist_t val) { return _ToMm1000(axis, val); }
	static sdist_t  ToMachine(axis_t axis, mm1000_t val) { return _ToMachine(axis, val); }

	static void ToMachine(const mm1000_t mm1000[NUM_AXIS], udist_t machine[NUM_AXIS]) { for (axis_t x = 0; x < NUM_AXIS; x++) { machine[x] = _ToMachine(x, mm1000[x]); } };
	static void ToMm1000(const udist_t machine[NUM_AXIS], mm1000_t mm1000[NUM_AXIS]) { for (axis_t x = 0; x < NUM_AXIS; x++) { mm1000[x] = _ToMm1000(x, machine[x]); } };

	bool    IsError() const { return _error != nullptr; };
	error_t GetError() const { return _error; }
	void    ClearError() { _error = nullptr; }

protected:
	virtual void TransformFromMachinePosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS]);
	virtual bool TransformPosition(const mm1000_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS]);

	feedrate_t GetFeedRate(const mm1000_t to[NUM_AXIS], feedrate_t feedrate) const;

	virtual steprate_t GetStepRate(const mm1000_t to[NUM_AXIS], const udist_t to_m[NUM_AXIS], feedrate_t feedrate) const;
	static steprate_t  FeedRateToStepRate(axis_t axis, feedrate_t feedrate);

	static feedrate_t StepRateToFeedRate(axis_t axis, steprate_t steprate);

	mm1000_t _current[NUM_AXIS];

	void Error(error_t error) { _error = error; }
	void Error() { Error(MESSAGE_UNKNOWNERROR); }

private:
	static ToMm1000_t  _ToMm1000;
	static ToMachine_t _ToMachine;
	error_t            _error = nullptr;

public:
	void SetPositionFromMachine();
	void GetPosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS]);

public:
#ifdef _MSC_VER

	virtual void UnitTest() { }

	CMotionControlBase()
	{
		memset(&_current, 0, sizeof(_current));
	}

	feedrate_t CalcFeedRate(const mm1000_t to[NUM_AXIS], feedrate_t feedrate) const
	{
		return GetFeedRate(to, feedrate);
	}
	
#endif

	////////////////////////////////////////
	// all positions are logical-pos

	void         Arc(const mm1000_t to[NUM_AXIS], mm1000_t offset0, mm1000_t offset1, axis_t axis_0, axis_t axis_1, bool isclockwise, feedrate_t feedrate);
	virtual void MoveAbs(const mm1000_t to[NUM_AXIS], feedrate_t feedrate);

	void     GetPositions(mm1000_t current[NUM_AXIS]) const;
	mm1000_t GetPosition(axis_t axis) const;

	static feedrate_t GetMaxFeedRate(axis_t axis, feedrate_t feedrate = LONG_MAX);

	/////////////////////////////////////////////////////////
	// some helper function to move (all result in MoveAbs(...)

	void MoveAbsEx(feedrate_t feedrate, uint16_t axis, mm1000_t d, ...); // repeat axis and d until axis not in 0 .. NUM_AXIS-1
	void MoveRelEx(feedrate_t feedrate, uint16_t axis, mm1000_t d, ...); // repeat axis and d until axis not in 0 .. NUM_AXIS-1

	/////////////////////////////////////////////////////////
	// Samples for converting functions
	//
	// Remark: use fix div and multiplier 2^x => compiler will generate faster inline function 
	//		   256 (as div and mul) => compiler will generate faster inline function 
	//		   if MulDiv is not able with 256 use float => this is faster
	//
	// mm1000_t => 2^31 = 2147483648 = 2147483.648 mm = 2147.483648m 
	//
	//

	static float StepsPerMm1000[STEPSPERMM1000_SIZE];

	static mm1000_t ToMm1000_StepsPer(axis_t /* axis */, sdist_t val);
	static sdist_t  ToMachine_StepsPer(axis_t /* axis */, mm1000_t val);

	static mm1000_t ToMm1000_StepsPerEx(axis_t axis, sdist_t val);
	static sdist_t  ToMachine_StepsPerEx(axis_t axis, mm1000_t val);


	// one Step = 0.1mm => 10/rot
	static mm1000_t ToMm1000_1_10(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 100, 1); }
	static sdist_t  ToMachine_1_10(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 1, 100); }

	// one Step = 0.01mm => 100/rot
	static mm1000_t ToMm1000_1_100(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 10, 1); }
	static sdist_t  ToMachine_1_100(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 1, 10); }

	// one Step = 0.001mm => 1000/rot
	static mm1000_t ToMm1000_1_1000(axis_t /* axis */, sdist_t val) { return val; } //{ return  RoundMulDivU32(val, 1, 1); }
	static sdist_t  ToMachine_1_1000(axis_t /* axis */, mm1000_t val) { return val; } //{ return  MulDivU32(val, 1, 1); }


	////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// 200 / 2-Stepper (halfStep) => 400 / rotation

	// functions: 1 rotation(400Steps) = 1mm

	static mm1000_t ToMm1000_1_400(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 5, 2); }
	static sdist_t  ToMachine_1_400(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 2, 5); }

	////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// 200 / 8-Stepper => 1600 / rotation

	// functions: 1 rotation(1600Steps) = 1mm

	static mm1000_t ToMm1000_1_1600(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 5, 8); }
	static sdist_t  ToMachine_1_1600(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 8, 5); }

	////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// 200 / 16-Stepper => 3200 / rotation

	// functions: 1 rotation(3200Steps) = 1mm

	static mm1000_t ToMm1000_1_3200(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 80, 256); }
	static sdist_t  ToMachine_1_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 16, 5); }

	static mm1000_t ToMm1000Inch_1_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 25, 2032); }
	static sdist_t  ToMachineInch_1_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 2032, 25); }

	// functions: 1 rotation = 1.25mm

	static mm1000_t ToMm1000_1d25_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 100, 256); }
	static sdist_t  ToMachine_1d25_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 64, 25); }

	// functions: 1 rotation = 1.5mm

	static mm1000_t ToMm1000_1d5_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 120, 256); }
	static sdist_t  ToMachine_1d5_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 32, 15); }

	// functions: 1 rotation = 2mm

	static mm1000_t ToMm1000_2_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 160, 256); }
	static sdist_t  ToMachine_2_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 8, 5); }

	// functions: 1 rotation = 3mm

	static mm1000_t ToMm1000_3_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 240, 256); }
	static sdist_t  ToMachine_3_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 16, 15); }

	// functions: 1 rotation = 4mm

	static mm1000_t ToMm1000_4_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 320, 256); }
	static sdist_t  ToMachine_4_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 4, 5); }

	// functions: 1 rotation = 5mm

	static mm1000_t ToMm1000_5_3200(axis_t /* axis */, sdist_t val) { return MulDivU32(val, 400, 256); }
	static sdist_t  ToMachine_5_3200(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 16, 25); }


	////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// 200 / 32-Stepper => 6400 / rotation

	// functions: 1 rotation(6400Steps) = 1mm

	static mm1000_t ToMm1000_1_6400(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 40, 256); }
	static sdist_t  ToMachine_1_6400(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 32, 5); }

	// functions: 1 rotation(6400Steps) = 5mm

	static mm1000_t ToMm1000_5_6400(axis_t /* axis */, sdist_t val) { return RoundMulDivU32(val, 200, 256); }
	static sdist_t  ToMachine_5_6400(axis_t /* axis */, mm1000_t val) { return MulDivU32(val, 32, 25); }
};

////////////////////////////////////////////////////////
