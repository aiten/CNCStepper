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

////////////////////////////////////////////////////////

template <pin_t PWMPIN> class CAnalog8IOControlSmooth
{
public:

	void Init() // init and set default value
	{
		_nextTime     = 0;
		_currentLevel = _ioLevel = 0;
		Out(0);
#ifndef REDUCED_SIZE
		_level = 0;
#endif
	}

	void Init(uint8_t level) // init and set default value
	{
		Init();
		On(level);
	}

	void On(uint8_t level) // Set level and turn on
	{
#ifndef REDUCED_SIZE
		_level = level;
#endif
		MySetLevel(level);
	}

	void OnMax() // turn on at max level, same as On(255)
	{
		On(255);
	}

#ifndef REDUCED_SIZE
	void On() // turn on at specified level (see Level property)
	{
		MySetLevel(_level);
	}
#endif

	void Off() // turn off, use On() to switch on at same value
	{
		MySetLevel(0);
	}

	bool IsOn() const
	{
		return _ioLevel != 0;
	}

#ifndef REDUCED_SIZE
	void SetLevel(uint8_t level)
	{
		_level = level;
	}

	uint8_t GetLevel() const
	{
		return _level;
	}
#endif

	uint8_t GetIOLevel() const
	{
		return _ioLevel;
	}

	uint8_t GetCurrentIOLevel() const
	{
		return _currentLevel;
	}

	void Poll()
	{
		uint32_t milli;
		if (_currentLevel != _ioLevel && (milli = millis()) >= _nextTime)
		{
			_nextTime = milli + _delayMs;
			if (_currentLevel > _ioLevel)
			{
				_currentLevel--;
			}
			else
			{
				_currentLevel++;
			}

			Out(_currentLevel);
		}
	}

	void PollForce()
	{
		_nextTime = 0;
		Poll();
	}

	void SetDelay(uint8_t delayMs)
	{
		_delayMs = delayMs;
	}

private:

	static void Out(uint8_t lvl)
	{
		CHAL::analogWrite8(PWMPIN, lvl);
	}

	uint32_t _nextTime;			// time to modify level
#ifndef REDUCED_SIZE
	uint8_t _level;				// value if "enabled", On/Off will switch between 0..level
#endif

	uint8_t _currentLevel;		// used for analogWrite
	uint8_t _ioLevel;			// current level
	uint8_t _delayMs;

	void MySetLevel(uint8_t level) NEVER_INLINE_AVR
	{
		_ioLevel = level;
		if (_delayMs == 0)
		{
			_currentLevel = level;
			Out(level);
		}
	}
};

////////////////////////////////////////////////////////
