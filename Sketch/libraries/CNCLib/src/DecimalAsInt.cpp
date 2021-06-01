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

////////////////////////////////////////////////////////

#include "CNCLib.h"
#include "DecimalAsInt.h"

////////////////////////////////////////////////////////

char* CMm1000::ToString(mm1000_t pos, char* tmp, uint8_t precision, uint8_t scale)
{
	// right aligned with precision and scale  (+round to scale)
	// call the base class only here to avoid multiple "inline" of a big function

	return super::ToString(pos, tmp, precision, scale);
}

////////////////////////////////////////////////////////

char* CInch100000::ToString(inch100000_t pos, char* tmp, uint8_t precision, uint8_t scale)
{
	return super::ToString(pos, tmp, precision, scale);
}

////////////////////////////////////////////////////////////

char* CSDist::ToString(sdist_t v, char* tmp, uint8_t precision)
{
	// right aligned
	tmp[precision] = 0; // terminating 0

	uint8_t x = ToPrecisionS10(v);

	if (x == 0)
	{
		// 0 => Precision = 0
		x = 1;
	}

	if (v < 0)
	{
		// add -
		x++;
	}

	if (precision < x)
	{
		for (x = 0; x < precision; x++)
		{
			tmp[x] = 'x';
		}
	}
	else
	{
		precision = precision - x;

		for (x = 0; x < precision; x++)
		{
			tmp[x] = ' ';
		}
		_ltoa(v, tmp + precision, 10);
	}

	return tmp;
}
