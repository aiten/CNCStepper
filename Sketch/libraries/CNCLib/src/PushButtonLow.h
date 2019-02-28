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

// PushButton with LOW is pressed

////////////////////////////////////////////////////////

template <pin_t PIN, uint8_t ONVALUE> class CPushButtonLow
{
public:

	CPushButtonLow() = default;

	static void Init(uint8_t inputMode = INPUT_PULLUP)
	{
		CHAL::pinMode(PIN, inputMode);
	}

	void Check()
	{
		bool isOn = CHAL::digitalRead(PIN) == ONVALUE;
		switch (_state)
		{
			case ButtonOff:
			{
				if (isOn)
				{
					_state = ButtonPressed;
				}
				break;
			}
			case ButtonPressed:
			{
				if (!isOn)
				{
					_state = ButtonPressedOff;
				}
				break;
			}
			case ButtonPressedOff: break;

			case ExpectButtonOff:
			{
				if (!isOn)
				{
					_state = ButtonOff;
				}
				break;
			}
		}
	}

	static bool IsPressed()
	{
		// current state!!!!!
		// use IsOn() 
		return CHAL::digitalRead(PIN) == ONVALUE;
	}

	bool IsOn()
	{
		// check and set state 
		// button must be released and pressed to get "true" again.

		Check();

		if (_state == ButtonPressed)
		{
			_state = ExpectButtonOff;
			return true;
		}
		else if (_state == ButtonPressedOff)
		{
			_state = ButtonOff;
			return true;
		}
		return false;
	}

protected
:

	enum EButtonState
	{
		ButtonOff = 0,		// button not pressed, not waiting
		ButtonPressed,		// report pressed if IsOn is called, Button still pressed, wait for Button Off
		ButtonPressedOff,	// Pressed an released but not reported
		ExpectButtonOff		// reported on IsOn, wait for "Off"
	};

	EnumAsByte(EButtonState) _state = ButtonOff;
};

////////////////////////////////////////////////////////
