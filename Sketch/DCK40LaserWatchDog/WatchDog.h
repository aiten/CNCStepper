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

class WatchDog
{
private:

	bool    _isOn = false;
	uint8_t _onValue;
	uint8_t _offValue;
	uint8_t _pin;

public:

	void Init(uint8_t pin, uint8_t onValue)
	{
		_pin      = pin;
		_onValue  = onValue;
		_offValue = onValue == LOW ? HIGH : LOW;

		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, _offValue);
	}

	bool OnOff(bool on)
	{
		return on ? On() : Off();
	}

	bool On()
	{
		digitalWrite(_pin, _onValue);
		if (_isOn == false)
		{
			_isOn = true;
			return true;
		}
		return false;
	}

	////////////////////////////////////////////////////////////

	bool IsOn() const
	{
		return _isOn;
	}

	////////////////////////////////////////////////////////////

	bool Off()
	{
		digitalWrite(_pin, _offValue);
		if (_isOn == true)
		{
			_isOn = false;
			return true;
		}
		return false;
	}
};
