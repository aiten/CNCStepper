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

#ifdef _MSC_VER

#define _CRT_SECURE_NO_WARNINGS
#define _AFX_SECURE_NO_WARNINGS 

#endif

//////////////////////////////////////////

#include "UtilitiesStepperLib.h"

////////////////////////////////////////////////////////

uint8_t ToPrecisionU10(uint16_t v)
{
	if (v < 1) return 0;
	if (v < 10) return 1;
	if (v < 100) return 2;
	if (v < 1000) return 3;
	if (v < 10000) return 4;
	return 5;
}

uint8_t ToPrecisionU10(uint32_t v)
{
	if (v < 10000) return ToPrecisionU10(uint16_t(v));
	if (v < 100000) return 5;
	if (v < 1000000) return 6;
	if (v < 10000000) return 7;
	if (v < 100000000) return 8;
	if (v < 1000000000) return 9;
	return 10;
}

uint8_t ToPrecisionS10(int32_t v)
{
	if (v < 0)
	{
		return ToPrecisionU10(uint32_t(-v));
	}
	return ToPrecisionU10(uint32_t(v));
}


////////////////////////////////////////////////////////

uint8_t ToPrecisionU2(uint16_t v)
{
	uint8_t i = 0;
	for (; v != 0; i++)
	{
		v /= 2;
	}
	return i;
}

uint8_t ToPrecisionU2(uint32_t v)
{
	uint8_t i = 0;
	for (; v != 0; i++)
	{
		v /= 2;
	}
	return i;
}

uint8_t ToPrecisionS2(int32_t v)
{
	if (v < 0)
	{
		return ToPrecisionU2(uint32_t(-v));
	}
	return ToPrecisionU2(uint32_t(v));
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt_round(uint32_t val, bool round)
{
	uint32_t temp;
	uint32_t g     = 0;
	uint32_t b     = 0x8000;
	uint32_t bshft = 15;

	for (uint8_t i = 0; i < 15; i++)
	{
		temp = g;
		temp = temp + (b >> 1);
		temp = temp << (bshft + 1);
		if (val >= temp)
		{
			g += b;
			val -= temp;
		}
		bshft--;
		b = b >> 1;
	}
	temp = (g << 1);
	temp = temp | 1;
	if (val >= temp)
	{
		g += b;
		val -= temp;
	}

	if (round && val > g)
	{
		g++;
	}

	return g;
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt_round(uint32_t val)
{
	return _ulsqrt_round(val, true);
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt(uint32_t val)
{
	return _ulsqrt_round(val, false);
}
