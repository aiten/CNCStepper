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
#include <string.h>

#include <Arduino.h>
#include <ctype.h>
#include <math.h>

#include "CNCLib.h"
#include <GCodeParserBase.h>
#include "MyMotionControl.h"
#include "StepperServo.h"

/////////////////////////////////////////////////////////

#define SEGMENTCOUNT 3

#define SEGMENT1	140000.0f
#define SEGMENT2	152000.0f
#define SEGMENT3	30000.0f

#define A SEGMENT2
#define B SEGMENT1
#define H 105000.0f	//  105.0   // height start first segment
#define E SEGMENT3

#define Amm mm1000_t(A)
#define Bmm mm1000_t(B)
#define Hmm mm1000_t(H)
#define Emm mm1000_t(E)

// c		=> triangle A/B/C
// s		=> diagonal x/y 
// alpha	=> angle of triangle
// alpha1	=> angle horizontal and c

// segment 2 moves parallel to surface if angle 1 is changed 

#define SEGMENT2PARALLEL

// pos 1.300ms => 55 Grad (from xy pane)
#define DEFAULTANGLE float(CENTER_LIMIT / MsForPI * MY_PI)
#define ANGLE1OFFSET float(DEFAULTANGLE - (55.0*M_PI/180.0))

// pos 1.300ms => 80 Grad (between A and B)
#define ANGLE1TOANGLE2 float(M_PI/2.0)
#define ANGLE2OFFSET float((DEFAULTANGLE - ((80.0-55.0+20.0)*M_PI/180.0)) - ANGLE1TOANGLE2)

#define ANGLE3OFFSET DEFAULTANGLE

#define SPLITMOVEDIST	10000

#define MY_PI	float(M_PI)

/////////////////////////////////////////////////////////

CMyMotionControl::CMyMotionControl() {}

/////////////////////////////////////////////////////////

inline float FromMs(mm1000_t ms, axis_t /* axis */)
{
	return float(ms / MsForPI * MY_PI);
}

/////////////////////////////////////////////////////////

inline mm1000_t ToMs(float angle, axis_t /* axis */)
{
	return mm1000_t(angle * MsForPI / MY_PI);
}

/////////////////////////////////////////////////////////

inline bool IsFloatOK(float val)
{
	return !isnan(val) && !isinf(val);
}

/////////////////////////////////////////////////////////

inline int ToGRADRound(float a)
{
	return int(a * 180.0 / MY_PI + 0.5);
}

/////////////////////////////////////////////////////////

inline float ToAngleRAD(mm1000_t angle)
{
	return float(angle / 1000.0 / 180.0 * MY_PI);
}

/////////////////////////////////////////////////////////

void CMyMotionControl::AdjustToAngle(float angle[NUM_AXIS])
{
#ifdef SEGMENT2PARALLEL
	angle[1] += angle[0];
#endif

	angle[0] += ANGLE1OFFSET;
	angle[1] += ANGLE2OFFSET;
	angle[2] += ANGLE3OFFSET;
}

/////////////////////////////////////////////////////////

void CMyMotionControl::AdjustFromAngle(float angle[NUM_AXIS])
{
	angle[0] -= ANGLE1OFFSET;
	angle[1] -= ANGLE2OFFSET;
	angle[2] -= ANGLE3OFFSET;

#ifdef SEGMENT2PARALLEL
	angle[1] -= angle[0];
#endif
}

/////////////////////////////////////////////////////////

void CMyMotionControl::TransformFromMachinePosition(const udist_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS])
{
	super::TransformFromMachinePosition(src, dest);

	float angle[NUM_AXIS];

	for (axis_t i = 0; i < SEGMENTCOUNT; i++)
		angle[i]  = FromMs(dest[i], i);

	AdjustFromAngle(angle);
	FromAngle(angle, dest);
}

/////////////////////////////////////////////////////////

bool CMyMotionControl::TransformPosition(const mm1000_t src[NUM_AXIS], mm1000_t dest[NUM_AXIS])
{
	float angle[NUM_AXIS];

	if (!super::TransformPosition(src, dest))
		return false;

	if (!ToAngle(dest, angle))
	{
		Error(F("TransformPosition: geometry"));
		return false;
	}

	AdjustToAngle(angle);

	for (axis_t i = 0; i < SEGMENTCOUNT; i++)
	{
		dest[i] = ToMs(angle[i], X_AXIS);
	}

	return true;
}

/////////////////////////////////////////////////////////

steprate_t CMyMotionControl::GetStepRate(const mm1000_t to[NUM_AXIS], const udist_t to_m[NUM_AXIS], feedrate_t feedrate) const
{
	mm1000_t max_to=0;
	udist_t max_to_m = 0;

	for (axis_t x = 0; x < NUM_AXIS; x++)
	{
		mm1000_t dist = GetPosition(x);
		dist = to[x] > dist ? (to[x] - dist) : (dist - to[x]);

		if (dist > max_to)
		{
			max_to = dist;
		}

		udist_t dist_m = CStepper::GetInstance()->GetPosition(x);
		dist_m = to_m[x] > dist_m ? (to_m[x] - dist_m) : (dist_m - to_m[x]);

		if (dist_m > max_to_m)
		{
			max_to_m = dist_m;
		}
	}

	return FeedRateToStepRate(0, (float(feedrate) * float(max_to_m)) / float(max_to));
}

/////////////////////////////////////////////////////////

bool CMyMotionControl::ToAngle(const mm1000_t pos[NUM_AXIS], float angle[NUM_AXIS])
{
	float x = float(pos[0]);
	float y = float(pos[1]);
	float z = float(pos[2]);

	float s = sqrt(x * x + y * y);

	float c2 = (s - E) * (s - E) + (z - H) * (z - H);
	float c  = sqrt(c2);										// triangle for first and second segment

	float alpha1 = (s - E) == 0.0 ? 0.0f : atanf((z - H) / (s - E));	// "base" angle of c
	float alpha  = acosf((B * B + c2 - A * A) / (2.0f * B * c));
	float gamma  = acosf((A * A + B * B - c2) / (2.0f * A * B));

	angle[0] = (alpha + alpha1);
	angle[1] = gamma;
	if (x == 0.0)
	{
		angle[2] = y > 0.0 ? float(MY_PI / 2.0) : - float(MY_PI / 2.0);
	}
	else
	{
		angle[2] = atan(y / x);
	}
	if (x < 0.0)
	{
		angle[2] = MY_PI + angle[2];
		if (angle[2] >= MY_PI)
			angle[2] -= 2.0 * MY_PI;
	}

	if (!IsFloatOK(angle[0])) return false;
	if (!IsFloatOK(angle[1])) return false;
	if (!IsFloatOK(angle[2])) return false;

	return true;
}

/////////////////////////////////////////////////////////

bool CMyMotionControl::FromAngle(const float angle[NUM_AXIS], mm1000_t dest[NUM_AXIS])
{
	float c2 = (A * A + B * B - 2.0f * A * B * cosf(angle[1]));
	float c  = sqrtf(c2);

	float alpha  = acosf((c2 + B * B - A * A) / (2.0f * B * c));
	float alpha1 = angle[0] - alpha;

	float s = cosf(alpha1) * c + E;

	dest[0] = CMm1000::Cast(cosf(angle[2]) * s);
	dest[1] = CMm1000::Cast(sinf(angle[2]) * s);

	dest[2] = CMm1000::Cast(H + sinf(alpha1) * c);

	return true;
}

/////////////////////////////////////////////////////////

void CMyMotionControl::MoveAbs(const mm1000_t to[NUM_AXIS], feedrate_t feedRate)
{
	uint16_t moveCount = 1;
	mm1000_t nextTo[NUM_AXIS];
	mm1000_t totalDist[NUM_AXIS];

	mm1000_t       maxDist   = 0;
	const mm1000_t splitDist = SPLITMOVEDIST;

	axis_t i;

	for (i = 0; i < NUM_AXIS; i++)
	{
		mm1000_t dist = to[i] - _current[i];
		totalDist[i]  = dist;

		if (dist < 0) dist = -dist;
		if (dist > maxDist)
			maxDist = dist;
	}

	if (maxDist > splitDist)
	{
		moveCount = uint16_t(maxDist / splitDist);
		if ((maxDist % splitDist) != 0)
			moveCount++;
	}

	for (uint16_t j = moveCount - 1; j > 0; j--)
	{
		for (i = 0; i < NUM_AXIS; i++)
		{
			mm1000_t nextPos = RoundMulDivI32(totalDist[i], j, moveCount);
			nextTo[i]        = to[i] - nextPos;
		}

		super::MoveAbs(nextTo, feedRate);
		if (IsError()) return;
	}

	super::MoveAbs(to, feedRate);
}

/////////////////////////////////////////////////////////

void CMyMotionControl::MoveAngle(const mm1000_t dest[NUM_AXIS])
{
	udist_t to[NUM_AXIS] = { 0 };

	axis_t i;

	for (i    = 0; i < SEGMENTCOUNT; i++)
		to[i] = ToMs(ToAngleRAD(dest[i]), i);

	for (i = 0; i < NUM_AXIS; i++)
	{
		if (to[i] == 0)
			to[i] = CStepper::GetInstance()->GetCurrentPosition(i);
	}

	CStepper::GetInstance()->MoveAbs(to);
	SetPositionFromMachine();
}

/////////////////////////////////////////////////////////

void CMyMotionControl::MoveAngleLog(const mm1000_t dest[NUM_AXIS])
{
	axis_t i;
	float  angle[NUM_AXIS] = { 0.0 };

	for (i       = 0; i < SEGMENTCOUNT; i++)
		angle[i] = FromMs(CStepper::GetInstance()->GetCurrentPosition(i), i);

	AdjustFromAngle(angle);

	mm1000_t to[NUM_AXIS];
	memcpy(to, _current, sizeof(_current));

	for (i = 0; i < SEGMENTCOUNT; i++)
		if (dest[i] != 0) angle[i] = ToAngleRAD(dest[i]);

	FromAngle(angle, to);

	MoveAbs(to, CGCodeParserBase::GetG1FeedRate());
}

/////////////////////////////////////////////////////////

void CMyMotionControl::PrintInfo()
{
	float angle[NUM_AXIS];

	if (!ToAngle(_current, angle))
	{
		Error(F("TransformPosition: geometry"));
	}

	char tmp[16];

	StepperSerial.print(ToGRADRound(angle[0]));
	StepperSerial.print(F(":"));
	StepperSerial.print(ToGRADRound(angle[1]));
	StepperSerial.print(F(":"));
	StepperSerial.print(ToGRADRound(angle[2]));
	StepperSerial.print(F("=>"));

	AdjustToAngle(angle);

	StepperSerial.print(ToGRADRound(angle[0]));
	StepperSerial.print(F(":"));
	StepperSerial.print(ToGRADRound(angle[1]));
	StepperSerial.print(F(":"));
	StepperSerial.print(ToGRADRound(angle[2]));
	StepperSerial.print(F("=>"));

	StepperSerial.print(CMm1000::ToString(ToMs(angle[0],X_AXIS), tmp, 3));
	StepperSerial.print(F(":"));
	StepperSerial.print(CMm1000::ToString(ToMs(angle[1],Y_AXIS), tmp, 3));
	StepperSerial.print(F(":"));
	StepperSerial.print(CMm1000::ToString(ToMs(angle[2],Z_AXIS), tmp, 3));
}


/////////////////////////////////////////////////////////

#ifdef _MSC_VER

mm1000_t ToMM(float mm)
{
	return mm1000_t(mm * 1000);
}

void CMyMotionControl::UnitTest()
{
	Test(1000, 200000, Hmm, true);		// max dist
	Test(0, 200000, Hmm, true);			// max dist
	Test(-1000, 200000, Hmm, true);		// max dist

	Test(1000, -200000, Hmm, true);		// max dist
	Test(0, -200000, Hmm, true);		// max dist
	Test(-1000, -200000, Hmm, true);	// max dist

	Test(mm1000_t(SEGMENT1 + SEGMENT2 + SEGMENT3), 0, Hmm, true);				// max dist
	Test(mm1000_t(SEGMENT2 + SEGMENT3), 0, mm1000_t(SEGMENT1) + Hmm, true);		// max height

	Test(mm1000_t(SEGMENT2 + SEGMENT3), 0, mm1000_t(SEGMENT1) + Hmm, true);		// max height

	Test(200000, 0, Hmm, true);
	Test(200000, 100000, Hmm, true);
	Test(100000, 100000, Hmm, true);

	// test for H = 105

	mm1000_t x, y, z;

	const mm1000_t step = 10000;

	if (true)
	{
		for (x = 0; x <= A + B + E + 1000; x += step)
		{
			Test(x, 0, Hmm, true);
		}
	}

	if (false)
	{
		for (x = 00000; x <= 300000; x += step)
		{
			for (y = 0; y <= 300000; y += step)
			{
				for (z = 0; z <= 300000; z += step)
				{
					Test(x, y, z, true);
				}
			}
		}
	}

	Test(200000, 0, 50000, true);
	Test(200000, 0, 150000, true);
	Test(200000, 50000, 150000, true);
	Test(300000, 150000, 200000, true);
}

inline float ToRAD(float a) { return (a * 180.0f / MY_PI); }
inline float ToMM(mm1000_t a) { return (a / 1000.0f); }
inline bool  CompareMaxDiff(mm1000_t a, mm1000_t b, mm1000_t diff = 10) { return (abs(a - b) >= diff); }

#define FORMAT_MM "%.0f:%.0f:%.0f"
#define FORMAT_GRAD "%.0f:%.0f:%.0f"

bool CMyMotionControl::Test(mm1000_t src1, mm1000_t src2, mm1000_t src3, bool printOK)
{
	float    angle[NUM_AXIS];
	mm1000_t src[NUM_AXIS] = { src1, src2, src3 };
	axis_t   i;

	if (!ToAngle(src, angle))
		return false;

	float a[NUM_AXIS] = { 0.0 };

	if (false)
	{
		for (i = 0; i < SEGMENTCOUNT; i++)
		{
			mm1000_t tmp = ToMs(angle[i], i);
			a[i]         = FromMs(tmp, i);
		}
	}
	else
	{
		for (i   = 0; i < SEGMENTCOUNT; i++)
			a[i] = angle[i];
	}

	mm1000_t dest[NUM_AXIS];

	FromAngle(a, dest);

	bool isError = false;
	for (i      = 0; i < SEGMENTCOUNT && !isError; i++)
		isError = CompareMaxDiff(src[i], dest[i]);

	if (printOK || isError)
	{
		if (false)
		{
			printf(FORMAT_MM" => ", ToMM(src1), ToMM(src2), ToMM(src3));
			printf(FORMAT_GRAD" = > ", ToRAD(angle[0]), ToRAD(angle[1]), ToRAD(angle[2]));
			printf(FORMAT_MM, ToMM(dest[0]), ToMM(dest[1]), ToMM(dest[2]));
		}
		else
		{
			printf("{{ %f, %f, %f },\t", ToMM(src1), ToMM(src2), ToMM(src3));
			printf("{ %f, %f, %f }},", (angle[0]), (angle[1]), (angle[2]));
		}

		if (isError)
			printf(" ERROR");

		printf("\n");
	}

	return isError;
}

#endif
