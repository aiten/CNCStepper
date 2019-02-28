////////////////////////////////////////////////////////
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

#include <StepperLib.h>
#include <CNCLib.h>

void PenUp()
{
}

void PenDown()
{
}

////////////////////////////////////////////////////////////

void ToPoint(int x, int y, int * dx, int * dy, float rad)
{
	float mysin=sin(rad);
	float mycos=cos(rad);

	*dx = (int) ((float)x * mycos + (float)y * mysin);
	*dy = (int) ((float)y * mycos - (float)x * mysin);
}

////////////////////////////////////////////////////////////

long Dist(unsigned  x1, unsigned y1, unsigned x2, unsigned y2)
{
	long dx = (x1 > x2) ? x1-x2 : x2-x1;
	long dy = (y1 > y2) ? y1-y2 : y2-y1;
	return dx*dx+dy*dy;
//	return sqrt(dx*dx + dy*dy);
}

void Polygon(CMsvcStepper& Stepper, mdist_t x, mdist_t y, mdist_t radius, int n, unsigned int grad, steprate_t penmovespeed )
{
  // mittelpunkt!
  if (n>1)
  {
    int i;
    int dxx,dyy;
    int penDiff=0;
	udist_t z=Stepper.GetCurrentPosition(2);

    PenUp();

    for (i=0;i<=n;i++)
    {
      float rad = float((360.0/n*i)+grad) / float(180.0) * float(M_PI); 

      ToPoint(radius,0,&dxx,&dyy,rad); 

	  long long maxd = max(abs(dxx),abs(dyy));
	  long long move = (int)sqrt(double(dxx)*double(dxx) + double(dyy)*double(dyy));
	  steprate_t vmax = (steprate_t)(((long long)penmovespeed)*maxd / move);

//	  Stepper.MoveOrigin3(dxx,dyy,penDiff,penmovespeed);
	  Stepper.MoveAbs3(x + dxx, y + dyy, z + penDiff,  vmax);

      if (i==0)
      {
        penDiff = 0; // Settings.penDown-Settings.penUp;
        PenDown();
      }
    }

    PenUp();
  }
}
