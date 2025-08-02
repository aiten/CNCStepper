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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <Arduino.h>
#include <ctype.h>

#include "CNCLib.h"
#include "MotionControlBase.h"

#ifdef _MSC_VER
#include "Control.h"
#endif

/////////////////////////////////////////////////////////

ToMm1000_t  CMotionControlBase::_toMm1000;
ToMachine_t CMotionControlBase::_toMachine;

/////////////////////////////////////////////////////////

template <> CMotionControlBase* CSingleton<CMotionControlBase>::_instance = nullptr;

/////////////////////////////////////////////////////////

void CMotionControlBase::GetPositions(mm1000_t current[NUM_AXIS]) const
{
	memcpy(current, _current, sizeof(_current));
}

/////////////////////////////////////////////////////////

mm1000_t CMotionControlBase::GetPosition(axis_t axis) const
{
	return _current[axis];
}

/////////////////////////////////////////////////////////

void CMotionControlBase::GetPosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS])
{
	TransformFromMachinePosition(src, dest);
}

/////////////////////////////////////////////////////////

void CMotionControlBase::SetPositionFromMachine()
{
	TransformFromMachinePosition(CStepper::GetInstance()->GetPositions(), _current);
	CLcd::InvalidateLcd();
}

/////////////////////////////////////////////////////////

void CMotionControlBase::TransformFromMachinePosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS])
{
	ToMm1000(src, dest);
}

/////////////////////////////////////////////////////////

bool CMotionControlBase::TransformPosition(const mm1000_t /* src */[NUM_AXIS], mm1000_t /* dest */[NUM_AXIS])
{
	return true;
}

/////////////////////////////////////////////////////////

void CMotionControlBase::MoveAbs(const mm1000_t to[NUM_AXIS], feedrate_t feedrate)
{
	// the ONLY method to move!!!!!
	// do not call Stepper direct

#ifdef _MSC_VER
	CStepper::GetInstance()->MSCInfo = CControl::GetInstance()->GetBuffer();
#endif

	mm1000_t to_proj[NUM_AXIS];
	udist_t  to_m[NUM_AXIS];

	memcpy(to_proj, to, sizeof(_current));

	if (TransformPosition(to, to_proj))
	{
		ToMachine(to_proj, to_m);

 		feedrate = GetFeedRate(to, feedrate);

		CStepper::GetInstance()->MoveAbs(to_m, GetStepRate(to, to_m, feedrate));

		if (CStepper::GetInstance()->IsError())
		{
			SetPositionFromMachine();
		}
		else
		{
			memcpy(_current, to, sizeof(_current));
		}
	}
}

/////////////////////////////////////////////////////////
// based on:
//	motion_control.c - high level interface for issuing motion commands
//	Part of Grbl

// Arc with axis_0 and axis_1
// all other linear
// calculate Segments with f = k * x + d (x... radius, f ... segments)

#define SEGMENTS_K	4.0

#define SEGMENTS_D	18.0		// 20 Grad 
//#define SEGMENTS_D	20.0		// 18 Grad
//#define SEGMENTS_D	22.0		// 16.3
//#define SEGMENTS_D	24.0		// 15
//#define SEGMENTS_D	36.0		// 10

// Use: Small angle approximation from http://en.wikipedia.org/wiki/Small-angle_approximation
/*
Figure 3 shows the relative errors of the small angle approximations.The angles at which the relative error exceeds 1 % are as follows :
tan O near O at about 0.176 radians(10Grad).
sin O near O at about 0.244 radians(14Grad).
cos O near 1 - O2 / 2 at about 0.664 radians(38Grad).

=> To be save => not more than 10Grad for Arc_Correction
*/

#define ARCCORRECTION	( 10.0 * M_PI / 180.0)		// every 10grad

void CMotionControlBase::Arc(const mm1000_t to[NUM_AXIS], mm1000_t offset0, mm1000_t offset1, axis_t axis_0, axis_t axis_1, bool isclockwise, feedrate_t feedrate)
{
	// start from current position!

	mm1000_t current[NUM_AXIS];
	GetPositions(current);

	mm1000_t center_axis0 = current[axis_0] + offset0;
	mm1000_t center_axis1 = current[axis_1] + offset1;

	mm1000_t linear_travel_max     = 0;
	mm1000_t dist_linear[NUM_AXIS] = { 0 };

	for (axis_t x = 0; x < NUM_AXIS; x++)
	{
		if (x != axis_0 && x != axis_1)
		{
			dist_linear[x] = to[x] - current[x];
			if (dist_linear[x] > linear_travel_max)
				linear_travel_max = dist_linear[x];
		}
	}

	float radius = hypot(float(offset0), float(offset1));

	auto     r_axis0  = float(-offset0); // Radius vector from center to current location
	auto     r_axis1  = float(-offset1);
	mm1000_t rt_axis0 = to[axis_0] - center_axis0;
	mm1000_t rt_axis1 = to[axis_1] - center_axis1;

	// CCW angle between position and target from circle center. Only one atan2() trig computation required.
	float angular_travel = atan2(r_axis0 * rt_axis1 - r_axis1 * rt_axis0, r_axis0 * rt_axis0 + r_axis1 * rt_axis1);
	if (angular_travel == 0.0 || angular_travel == -0.0)
	{
		// 360Grad
		if (isclockwise)
		{
			angular_travel = float(-2.0 * M_PI);
		}
		else
		{
			angular_travel = float(2.0 * M_PI);
		}
	}
	else
	{
		if (angular_travel < 0.0)
		{
			angular_travel += float(2.0 * M_PI);
		}
		if (isclockwise)
		{
			angular_travel -= float(2.0 * M_PI);
		}
	}

#if defined(_MSC_VER)
#pragma warning (suppress:4189)
	float mm1000_of_travel = hypot(angular_travel * radius, float(abs(linear_travel_max)));
	mm1000_of_travel;
#endif
	if (hypot(angular_travel * radius, float(abs(linear_travel_max))) < 1)
	{
		return;
	}

	// difference to Grbl => use dynamic calculation of segments => suitable for small r
	//
	// segments for full circle => (CONST_K * r * M_PI * b + CONST_D)		(r in mm, b ...2?)

	auto segments = uint16_t(abs(floor((CMm1000::ConvertTo(mm1000_t(2 * SEGMENTS_K * M_PI)) * radius + SEGMENTS_D) * angular_travel / (2.0 * M_PI))));

#if defined(_MSC_VER)
	double segments_full = CMm1000::ConvertTo(mm1000_t(2 * SEGMENTS_K * M_PI)) * radius + SEGMENTS_D;
	segments_full;
	Trace("Gx command with\tr=%f\tfull_segments=%f\tangular=%f\tangularG=%f\tsegments=%i\n", radius, segments_full, angular_travel, angular_travel / M_PI * 180, segments);
#endif

	if (segments > 1)
	{
		float theta_per_segment = angular_travel / segments;

		auto arc_correction = int8_t(ARCCORRECTION / theta_per_segment);
		if (arc_correction < 0)
		{
			arc_correction = -arc_correction;
		}

		// Vector rotation matrix values
		float cos_T = float(1.0 - 0.5 * theta_per_segment * theta_per_segment);
		float sin_T = theta_per_segment;

		uint8_t count = 0;

		for (uint16_t i = 1; i < segments; i++)
		{
			if (count < arc_correction)
			{
				// Apply vector rotation matrix 
				float r_axisi = r_axis0 * sin_T + r_axis1 * cos_T;
				r_axis0       = r_axis0 * cos_T - r_axis1 * sin_T;
				r_axis1       = r_axisi;
				count++;
			}
			else
			{
				// Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments.
				// Compute exact location by applying transformation matrix from initial radius vector(=-offset).
				float cos_Ti = cos(i * theta_per_segment);
				float sin_Ti = sin(i * theta_per_segment);
				r_axis0      = -offset0 * cos_Ti + offset1 * sin_Ti;
				r_axis1      = -offset0 * sin_Ti - offset1 * cos_Ti;
				count        = 0;
			}

			// Update arc_target location

			current[axis_0] = mm1000_t(center_axis0 + r_axis0);
			current[axis_1] = mm1000_t(center_axis1 + r_axis1);

			for (axis_t x = 0; x < NUM_AXIS; x++)
			{
				if (dist_linear[x])
				{
					current[x] = to[x] - RoundMulDivI32(dist_linear[x], segments - i, segments);
				}
			}

			MoveAbs(current, feedrate);
		}
	}

	// Ensure last segment arrives at target location.
	MoveAbs(to, feedrate);
}

/////////////////////////////////////////////////////////

steprate_t CMotionControlBase::GetStepRate(const mm1000_t to[NUM_AXIS], const udist_t to_m[NUM_AXIS], feedrate_t feedrate) const
{
	// TODO: look to all axis to calculate the correct (max) steprate. It is not only AXIS_X
	return FeedRateToStepRate(0, feedrate);
}

/////////////////////////////////////////////////////////

feedrate_t CMotionControlBase::GetFeedRate(const mm1000_t to[NUM_AXIS], feedrate_t feedrate) const
{
	// feedrate < 0 => no arc correction (allowable max for all axis)
	// from current position

#define AvoidOverrun 256

	// axis_t maxdistaxis = 0;

	if (feedrate >= 0)
	{
		mm1000_t maxdist    = 0;
		mm1000_t sum        = 0;
		mm1000_t sumOverRun = 0;
		bool     useOverrun = false;
		uint8_t  axiscount  = 0;

		for (axis_t x = 0; x < NUM_AXIS; x++)
		{
			mm1000_t dist = GetPosition(x);
			dist          = to[x] > dist ? (to[x] - dist) : (dist - to[x]);

			if (dist != 0)
			{
				axiscount++;
				if (dist > maxdist)
				{
					// maxdistaxis = x;
					maxdist     = dist;
				}

				if (dist > 0xffff)
				{
					useOverrun = true;
				}
				else
				{
					mm1000_t oldsum = sum;
					sum += dist * dist;
					useOverrun |= oldsum > sum;
				}
				sumOverRun += (dist / AvoidOverrun) * (dist / AvoidOverrun);
			}
		}
		if (axiscount > 1) //  && maxDist > 0)
		{
			if (useOverrun)
			{
				maxdist = maxdist / AvoidOverrun;
				sum     = _ulsqrt_round(sumOverRun);
			}
			else
			{
				sum = _ulsqrt_round(sum);
			}

			if (maxdist != sum && sum != 0)
			{
				// avoid overrun: feedrate * maxDist
				if (ToPrecisionU2(uint32_t(feedrate)) + ToPrecisionU2(uint32_t(maxdist)) > 30)
				{
					// use float to avoid overruns
					feedrate = feedrate_t(float(feedrate) * float(maxdist) / float(sum));
				}
				else
				{
					feedrate = RoundMulDivU32(feedrate, maxdist, sum);
				}
			}
		}
	}

	return feedrate;
}

/////////////////////////////////////////////////////////

steprate_t CMotionControlBase::FeedRateToStepRate(axis_t axis, feedrate_t feedrate)
{
	// 60 because of min=>sec (feedrate in mm1000/min)
	if (feedrate < 0)
	{
		feedrate = -feedrate;
	}

	// feedrate => 32bit, steprate can be 16 bit
	feedrate_t steprate32 = _toMachine(axis, feedrate / 60);
	if (steprate32 > STEPRATE_MAX)
	{
		return STEPRATE_MAX;
	}
	auto steprate = steprate_t(steprate32);
	return steprate ? steprate : 1;
}

/////////////////////////////////////////////////////////

feedrate_t CMotionControlBase::StepRateToFeedRate(axis_t axis, steprate_t steprate)
{
	// 60 because of min=>sec (feedrate in mm1000/min)
	auto feedrate = feedrate_t(_toMm1000(axis, steprate * 60l));
	return feedrate ? feedrate : 1;
}

/////////////////////////////////////////////////////////

feedrate_t CMotionControlBase::GetMaxFeedRate(axis_t axis, feedrate_t feedRate)
{
	steprate_t maxStepRate = CStepper::GetInstance()->GetMaxSpeed(axis);
	feedrate_t maxFeedRate = StepRateToFeedRate(axis, maxStepRate);

	return min(feedRate, maxFeedRate);
}

////////////////////////////////////////////////////////
// repeat axis and d until axis not in 0 .. NUM_AXIS

void CMotionControlBase::MoveAbsEx(feedrate_t feedRate, uint16_t axis, mm1000_t d, ...)
{
	mm1000_t dest[NUM_AXIS];
	GetPositions(dest);

	va_list arglist;
	va_start(arglist, d);

	while (axis < NUM_AXIS)
	{
		dest[axis] = d; // replace current position

#ifdef _MSC_VER
		axis = va_arg(arglist, uint16_t);
		d    = va_arg(arglist, mm1000_t);
#else
		axis = va_arg(arglist, unsigned int);		// only "int" supported on arduino
		d = va_arg(arglist, uint32_t);
#endif
	}

	va_end(arglist);

	MoveAbs(dest, feedRate);
}

////////////////////////////////////////////////////////
// repeat axis and d until axis not in 0 .. NUM_AXIS

void CMotionControlBase::MoveRelEx(feedrate_t feedrate, uint16_t axis, mm1000_t d, ...)
{
	mm1000_t dest[NUM_AXIS];
	GetPositions(dest);

	va_list arglist;
	va_start(arglist, d);

	while (axis < NUM_AXIS)
	{
		dest[axis] += d; // add to current position

#ifdef _MSC_VER
		axis = va_arg(arglist, uint16_t);
		d    = va_arg(arglist, mm1000_t);
#else
		axis = va_arg(arglist, unsigned int);		// only "int" supported on arduino
		d = va_arg(arglist, uint32_t);
#endif
	}

	va_end(arglist);

	MoveAbs(dest, feedrate);
}

////////////////////////////////////////////////////////

void CMotionControlBase::InitConversionBestStepsPer(float stepspermm1000)
{
	InitConversionStepsPer(stepspermm1000);

	if (stepspermm1000 == 3.2f)
	{
		InitConversion(ToMm1000_1_3200, ToMachine_1_3200);
	}
	else if (stepspermm1000 == 6.4f)
	{
		InitConversion(ToMm1000_1_6400, ToMachine_1_6400);
	}
	else if (stepspermm1000 == 0.64f)
	{
		InitConversion(ToMm1000_5_3200, ToMachine_5_3200);
	}
}

////////////////////////////////////////////////////////

float CMotionControlBase::StepsPerMm1000[STEPSPERMM1000_SIZE];

////////////////////////////////////////////////////////

mm1000_t CMotionControlBase::ToMm1000_StepsPer(axis_t /* axis */, sdist_t val)
{
	return mm1000_t(val / StepsPerMm1000[0]);
}

sdist_t CMotionControlBase::ToMachine_StepsPer(axis_t /* axis */, mm1000_t val)
{
	return sdist_t(val * StepsPerMm1000[0]);
}

////////////////////////////////////////////////////////

mm1000_t CMotionControlBase::ToMm1000_StepsPerEx(axis_t axis, sdist_t val)
{
	return mm1000_t(val / StepsPerMm1000[axis]);
}

sdist_t CMotionControlBase::ToMachine_StepsPerEx(axis_t axis, mm1000_t val)
{
	return sdist_t(val * StepsPerMm1000[axis]);
}
