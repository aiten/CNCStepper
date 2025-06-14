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

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y

////////////////////////////////////////////////////////

#include "HAL.h"

//////////////////////////////////////////

extern uint8_t ToPrecisionU10(uint32_t);
extern uint8_t ToPrecisionU10(uint16_t);
extern uint8_t ToPrecisionS10(int32_t );

extern uint8_t ToPrecisionU2(uint32_t);
extern uint8_t ToPrecisionU2(uint16_t);
extern uint8_t ToPrecisionS2(int32_t );

////////////////////////////////////////////////////////

template <class T> bool IsBitSet(T   t, uint8_t bit) { return (t & ((T(1)) << bit)) != 0; };
template <class T> bool IsBitClear(T t, uint8_t bit) { return (t & ((T(1)) << bit)) == 0; };
template <class T> void BitSet(T&    t, uint8_t bit) { t |= (T(1)) << bit; };
template <class T> void BitClear(T&  t, uint8_t bit) { t &= ~((T(1)) << bit); };

////////////////////////////////////////////////////////

inline unsigned int RoundMulDivUInt(unsigned int v, unsigned int m, unsigned int d)
{
	return static_cast<unsigned int>((uint32_t(v) * uint32_t(m) + uint32_t(d / 2)) / d);
}

inline uint8_t RoundMulDivU8(uint8_t v, uint8_t m, uint8_t d)
{
	return uint8_t((static_cast<unsigned int>(v) * m + d / 2) / d);
}

inline uint32_t RoundMulDivU32(uint32_t v, uint32_t m, uint32_t d)
{
	return (v * m + d / 2) / d;
}

inline int32_t RoundMulDivI32(int32_t v, int32_t m, int32_t d)
{
	return (v * m + d / 2) / d;
}

inline uint32_t MulDivU32(uint32_t v, uint32_t m, uint32_t d)
{
	return (v * m) / d;
}

inline int32_t MulDivI32(int32_t v, int32_t m, int32_t d)
{
	return (v * m) / d;
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt_round(uint32_t val);
uint32_t _ulsqrt(uint32_t       val);

////////////////////////////////////////////////////////

#if defined(_MSC_VER) || defined(__SAM3X8E__) || defined(__SAMD21G18A__) || defined(ESP32)

typedef struct _udiv_t
{
	uint16_t quot;
	uint16_t rem;
}            udiv_t;

inline udiv_t udiv(uint16_t __num, uint16_t __denom)
{
	div_t  d = div(__num, __denom);
	udiv_t ud;
	ud.quot = uint16_t(d.quot);
	ud.rem  = uint16_t(d.rem);
	return ud;
}

#else

typedef struct _udiv_t {
	uint16_t quot;
	uint16_t rem;
} udiv_t;

extern udiv_t udiv(uint16_t __num, uint16_t __denom) __asm__("__udivmodhi4") __ATTR_CONST__;

#endif

////////////////////////////////////////////////////////

template <typename T, uint8_t sz> void DumpArray(FLSTR head, const T pos[sz], bool newline)
{
	if (head != nullptr)
	{
		StepperSerial.print(head);
		StepperSerial.print(F("="));
	}
	StepperSerial.print(pos[0]);
	for (uint8_t i = 1; i < sz; i++)
	{
		StepperSerial.print(F(","));
		StepperSerial.print(pos[i]);
	}
	if (newline)
	{
		StepperSerial.println();
	}
	else
	{
		StepperSerial.print(F(":"));
	}
}

////////////////////////////////////////////////////////

template <typename T> void DumpType(FLSTR head, T value, bool newline)
{
	if (head != nullptr)
	{
		StepperSerial.print(head);
		StepperSerial.print(F("="));
	}
	StepperSerial.print(value);

	if (newline)
	{
		StepperSerial.println();
	}
	else
	{
		StepperSerial.print(F(":"));
	}
}

////////////////////////////////////////////////////////
