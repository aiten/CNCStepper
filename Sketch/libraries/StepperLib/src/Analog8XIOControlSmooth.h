/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

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
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

template <pin_t PWMPIN, pin_t DIRPIN> class CAnalog8XIOControlSmooth
{
public:

	void Init() // init and set default value
	{
		_nextTime     = 0;
		_currentLevel = _ioLevel = 0;
		CHAL::pinMode(DIRPIN, OUTPUT);
		Out(0);
#ifndef REDUCED_SIZE
		_level = 0;
#endif
	}

	void Init(int16_t level) // init and set default value
	{
		Init();
		On(level);
	}

	void On(int16_t level) // Set level and turn on
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
	void SetLevel(int16_t level)
	{
		_level = level;
	}

	int16_t GetLevel() const
	{
		return _level;
	}
#endif

	int16_t GetIOLevel() const
	{
		return _ioLevel;
	}

	int16_t GetCurrentIOLevel() const
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

	static void Out(int16_t lvl)
	{
		CHAL::digitalWrite(DIRPIN, lvl >= 0);
		CHAL::analogWrite8(PWMPIN, uint8_t(abs(lvl)));
	}

	uint32_t _nextTime;			// time to modify level

#ifndef REDUCED_SIZE
	int16_t _level;				// value if "enabled", On/Off will switch between 0..level
#endif

	int16_t _currentLevel;		// used for analogWrite
	int16_t _ioLevel;			// current level

	uint8_t _delayMs;

	void MySetLevel(int16_t level)
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
