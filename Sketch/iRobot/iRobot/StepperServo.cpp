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

#include "StepperServo.h"
//#include "StepperServo_Pins.h"

////////////////////////////////////////////////////////

CStepperServo::CStepperServo() {}

////////////////////////////////////////////////////////

void CStepperServo::Init()
{
	super::Init();

	_pod._idleLevel = LevelMax;		// no Idle

	_servo[0].attach(MG995_SERVO1_PIN);            // do not change, see Adafruit_TiCoServo for available pins
	_servo[1].attach(MG995_SERVO2_PIN);
	_servo[2].attach(MG995_SERVO3_PIN);
	_servo[3].attach(MG995_SERVO4_PIN);
}

////////////////////////////////////////////////////////

void CStepperServo::Step(const uint8_t /* steps */[NUM_AXIS], uint8_t /* directionUp */, bool /* isSameDirection */)
{
	SetServo();
}

////////////////////////////////////////////////////////

void CStepperServo::SetServo()
{
	for (axis_t i = 0; i < NUM_AXIS; i++)
	{
		udist_t pos;
		if (i == Y_AXIS)
		{
			pos = udist_t(MAX_LIMIT) + udist_t(MIN_LIMIT) - GetCurrentPosition(i);
		}
		else
		{
			pos = GetCurrentPosition(i);
		}

		pos = (pos + SERVO_POSITION_SCALE / 2) / SERVO_POSITION_SCALE;	// and round

		if (pos != _lastPos[i])
		{
			_servo[i].write(pos);
			_lastPos[i] = pos;
		}
	}
}

////////////////////////////////////////////////////////

void CStepperServo::SetEnable(axis_t /* axis */, uint8_t /* level */, bool /* force */) {}

////////////////////////////////////////////////////////

uint8_t CStepperServo::GetEnable(axis_t /* axis */)
{
	return LevelMax;
}

////////////////////////////////////////////////////////

uint8_t CStepperServo::GetReferenceValue(uint8_t /* referenceId */)
{
	return false;
}

////////////////////////////////////////////////////////

bool CStepperServo::IsAnyReference()
{
	return false;
}

////////////////////////////////////////////////////////
