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

////////////////////////////////////////////////////////////

#include <avr\interrupt.h>

#define SAMPLECOUNT 16
#define SAMPLETIME  200

class WaterFlow
{
public:
	WaterFlow()
	{
		for (uint8_t i = 0; i < SAMPLECOUNT; i++)
		{
			_count[i]     = 0;
			_countTime[i] = 0;
		}
	}

	void Init(uint8_t pin)
	{
		_countIdx             = SAMPLECOUNT - 1;
		_countTime[_countIdx] = millis();
		Next();

		_ISRInstance = this;
		pinMode(pin, INPUT);
		attachInterrupt(digitalPinToInterrupt(pin), StaticISRPinRising, RISING);
	}

	unsigned int AvgCount(unsigned int timeDiff = 1000)
	{
		{
			uint8_t s = SREG;
			cli();					//  DisableInterrupts();
			TestSampleTime();
			SREG = s;
		}

		// no disable interrupts => cache

		unsigned char countIdx = _countIdx;
		uint32_t      sumTime  = 0;
		unsigned int  sumCount = 0;

		for (unsigned char diff = 1; diff < SAMPLECOUNT && sumTime < timeDiff; diff++)
		{
			unsigned char idx = PrevIndex(countIdx, diff);

			sumTime += _countTime[idx];
			sumCount += _count[idx];
		}

		if (sumTime <= SAMPLETIME)
			return 0;

		return ScaleCount(sumCount, sumTime, timeDiff);
	}

private:

	static WaterFlow* _ISRInstance;

	static unsigned int ScaleCount(unsigned int count, uint32_t totalTime, uint32_t scaleToTime)
	{
		return uint32_t(count) * scaleToTime / totalTime;
	}

	static unsigned char NextIndex(unsigned char idx, unsigned char count)
	{
		return static_cast<unsigned char>((idx + count)) % (SAMPLECOUNT);
	}

	static unsigned char PrevIndex(unsigned char idx, unsigned char count)
	{
		return (idx >= count) ? idx - count : (SAMPLECOUNT) - (count - idx);
	}

	void TestSampleTime()
	{
		if (millis() > _countUntil)
			Next();
	}

	static void StaticISRPinRising()
	{
		_ISRInstance->ISRPinRising();
	}

	void ISRPinRising()
	{
		TestSampleTime();
		_count[_countIdx]++;
	}

	void Next()
	{
		// prev
		_countTime[_countIdx] = millis() - _countTime[_countIdx];

		// set next (no cli => cache)
		uint8_t idxNext     = (_countIdx + 1) % SAMPLECOUNT;
		_count[idxNext]     = 0;
		_countTime[idxNext] = millis();
		_countUntil         = _countTime[idxNext] + SAMPLETIME;
		_countIdx           = idxNext;
	}

	uint32_t               _countUntil;
	volatile unsigned char _countIdx = 0;
	volatile int           _count[SAMPLECOUNT];
	uint32_t               _countTime[SAMPLECOUNT];
};

////////////////////////////////////////////////////////////
