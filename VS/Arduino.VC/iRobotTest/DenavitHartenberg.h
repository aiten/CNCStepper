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

#include "..\..\..\Sketch\libraries\CNCLib\src\Matrix4x4.h"

class CDenavitHartenberg
{
public:
	CDenavitHartenberg();
	~CDenavitHartenberg();


	void ToPosition(float   in[NUM_AXIS], float out[3]);
	void FromPosition(float in[3], float        out[NUM_AXIS], float epsilon);

protected:

	void InitMatrix(CMatrix4x4<float>& m, float in[NUM_AXIS]);

private:

	struct SSearchDef
	{
		float min;
		float max;
		float dist;
		float changeToPrev;
	};

	void TestConvert(CMatrix4x4<float>& m, float inout[4], bool out = false);

	float SearchMinOld(float pos[3], float inout[NUM_AXIS], uint8_t idx, struct SSearchDef& def, float epsilon);

	float CalcDist(float pos[3], float in[NUM_AXIS]);

	float SearchStep(float pos[3], float inout[NUM_AXIS], uint8_t idx, float diff, struct SSearchDef& def);


#define MAXSIZE 4

	bool Jacobi(double a[][MAXSIZE], double b[], int n, int maxIterations, double tolerance, double x[]);
};
