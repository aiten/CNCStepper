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
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

template <pin_t PIN> class CAnalog8InvertIOControl
{
public:

	void Init(uint8_t level = 0) // init and set default value
	{
		MySetLevel(level);
		_level = level;
	}

	void On(uint8_t level) // Set level and turn on
	{
		_level = level;
		MySetLevel(level);
	}

	void OnMax() // turn on at max level, same as On(255)
	{
		On(255);
	}

	void On() // turn on at specified level (see Level property)
	{
		MySetLevel(_level);
	}

	void Off() // turn off, use On() to switch on at same value
	{
		MySetLevel(0);
	}

	bool IsOn() const
	{
		return _ioLevel != 0;
	}

	void SetLevel(uint8_t level)
	{
		_level = level;
	}

	uint8_t GetLevel() const
	{
		return _level;
	}

	uint8_t GetIOLevel() const
	{
		return _ioLevel;
	}

private:

	uint8_t _level;
	uint8_t _ioLevel;

	void MySetLevel(uint8_t level)
	{
		_ioLevel = level;
		CHAL::analogWrite8(PIN, 255 - level);
	}
};

////////////////////////////////////////////////////////
