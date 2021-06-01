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

//////////////////////////////////////////

template <typename T, uint8_t SCALE, uint32_t SCALEMASK> class CDecimalAsInt
{
protected:

	T _value;

public:

	CDecimalAsInt() { _value    = 0; }
	CDecimalAsInt(T v) { _value = v; }

protected:

	// make a wrapper to the function to avoid inline => protected

	static char* ToString(int32_t pos, char* tmp, uint8_t precision, uint8_t scale)
	{
		const bool isNegative = pos < 0;

		pos += AddForRound(scale, isNegative);

		uint8_t x = ToPrecisionS10(pos);

		if (x < (SCALE + 1))
		{
			x = (SCALE + 1); // 0..999 => 0.000
		}

		if (isNegative)
		{
			x++;
		}

		if (scale == 0)
		{
			x -= SCALE; // with dot
		}
		else if (scale <= SCALE)
		{
			// need dot
			x++;
			x -= SCALE - scale;
		}
		else
		{
			x += scale - SCALE;
		}

		if (x > precision)
		{
			// overflow
			for (x = 0; x < precision; x++)
			{
				tmp[x] = 'x';
			}
			if (scale > 0)
			{
				tmp[precision - scale - 1] = '.';
			}
			if (isNegative)
			{
				tmp[0] = '-';
			}
			tmp[x] = 0;
		}
		else
		{
			tmp[precision--] = 0;

			ldiv_t ud = ldiv(pos, SCALEMASK);

			if (ud.quot < 0)
			{
				ud.quot = -ud.quot;
			}
			if (ud.rem < 0)
			{
				ud.rem = -ud.rem;
			}

			if (scale > 0)
			{
				for (x = scale; x > SCALE; x--)
				{
					tmp[precision--] = '0';
				}

				ldiv_t sud;
				sud.quot = ud.rem;

				for (x = SCALE; x > 0; x--)
				{
					sud = ldiv(sud.quot, 10);
					if (x <= scale)
					{
						tmp[precision--] = '0' + uint8_t(sud.rem);
					}
				}
				tmp[precision--] = '.';
			}

			do
			{
				ud               = ldiv(ud.quot, 10);
				tmp[precision--] = '0' + uint8_t(ud.rem);
			}
			while (ud.quot != 0);

			if (isNegative)
			{
				tmp[precision--] = '-';
			}

			while (precision != uint8_t(-1))
			{
				tmp[precision--] = ' ';
			}
		}

		return tmp;
	}

	static char* SkipSpaces(char* t)
	{
		while ((*t) == ' ') t++;
		return t;
	};

private:

	static int32_t AddForRound(uint8_t scale, bool isNegativ)
	{
		if (isNegativ)
		{
			switch (scale)
			{
				case 0: return -int32_t(SCALEMASK / 2);
				case 1: return -int32_t(SCALEMASK / 20);
				case 2: return -int32_t(SCALEMASK / 200);
				case 3: return -int32_t(SCALEMASK / 2000);
				case 4: return -int32_t(SCALEMASK / 20000);
				case 5: return -int32_t(SCALEMASK / 200000);
				default: return 0;
			}
		}
		switch (scale)
		{
			case 0: return SCALEMASK / 2;
			case 1: return SCALEMASK / 20;
			case 2: return SCALEMASK / 200;
			case 3: return SCALEMASK / 2000;
			case 4: return SCALEMASK / 20000;
			case 5: return SCALEMASK / 200000;
			default: return 0;
		}
	}

public:

	static T Cast(expr_t v) { return T(v); }				// do not use lrint => convert to double first

	static float DegreeToRAD(T v) { return float(v / (SCALEMASK * 180.0 / M_PI)); }
	static T     FromRAD(float v) { return Cast(float(v * SCALEMASK * 180 / M_PI)); }

	static expr_t ConvertTo(T        v) { return expr_t(v) / SCALEMASK; }
	static T      ConvertFrom(expr_t v) { return T(v * SCALEMASK); }
};

//////////////////////////////////////////

class CMm1000 : public CDecimalAsInt<mm1000_t,SCALE_MM, 1000>
{
private:

	typedef CDecimalAsInt super;

public:

	static char* ToString(mm1000_t pos, char* tmp, uint8_t precision, uint8_t scale); // right aligned
	static char* ToString(mm1000_t pos, char* tmp, uint8_t scale) { return SkipSpaces(ToString(pos, tmp, 11, scale)); }

	char* ToString(char* tmp, uint8_t precision, uint8_t scale) const { return ToString(_value, tmp, precision, scale); } // right aligned
	char* ToString(char* tmp, uint8_t scale) const { return ToString(_value, tmp, scale); }
};

//////////////////////////////////////////

class CInch100000 : public CDecimalAsInt<inch100000_t,SCALE_INCH, 100000>
{
private:

	typedef CDecimalAsInt super;

public:

	static char* ToString(inch100000_t pos, char* tmp, uint8_t precision, uint8_t scale); // right aligned
	static char* ToString(inch100000_t pos, char* tmp, uint8_t scale) { return SkipSpaces(ToString(pos, tmp, 11, scale)); }

	char* ToString(char* tmp, uint8_t precision, uint8_t scale) const { return ToString(_value, tmp, precision, scale); } // right aligned
	char* ToString(char* tmp, uint8_t scale) const { return ToString(_value, tmp, scale); }
};

//////////////////////////////////////////

class CFeedrate1000 : public CDecimalAsInt<mm1000_t, SCALE_FEEDRATE, 1000>
{
private:

	typedef CDecimalAsInt super;
};

////////////////////////////////////////////////////////

class CSDist
{
private:
	sdist_t _value;

public:

	CSDist() { _value          = 0; }
	CSDist(sdist_t v) { _value = v; }

	static char* ToString(sdist_t v, char* tmp, uint8_t precision); // right aligned
};

//////////////////////////////////////////
