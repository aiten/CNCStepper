/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
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
