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

template <pin_t PIN, uint8_t ONVALUE, uint32_t STABLETIME> class CReadPinIOTriggerControl
{
public:

	static void Init(uint8_t inputMode = INPUT_PULLUP)
	{
		CHAL::pinMode(PIN, inputMode);
	}

	bool IsOn()
	{
		if (CHAL::digitalRead(PIN) == ONVALUE)
		{
			uint32_t now = millis();

			if (_timeOn == 0) // first on
			{
				_timeOn = now;
			}

			return (now - _timeOn >= STABLETIME);
		}

		_timeOn = 0;
		return false;
	}

private:

	uint32_t _timeOn = 0;
};

////////////////////////////////////////////////////////
