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

#include "StepperSMC800.h"

////////////////////////////////////////////////////////

// not implemented on due

#if !defined(__SAM3X8E__) && !defined(__SAMD21G18A__)

////////////////////////////////////////////////////////

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define SMC800_REFININ 40
#define SMC800_STROBEPIN 41

#elif defined(__AVR_ATmega328P__) || defined (_MSC_VER)

#define SMC800_REFININ 11
#define SMC800_STROBEPIN 10
// use SMC800 Byte 2-9

#else
ToDo;
#endif

////////////////////////////////////////////////////////

enum EStepperBaseAxis
{
	StepperX = 0,
	StepperY = 64,
	StepperZ = 128
};

////////////////////////////////////////////////////////

static const uint8_t sbm800HalfStep0[8] PROGMEM   = { 0x3F, 0x3F, 0x1F, 0x1F, 0x1B, 0x1B, 0x3B, 0x3B };
static const uint8_t sbm800HalfStep20[8] PROGMEM  = { 0x37, 0x36, 0x1E, 0x16, 0x13, 0x12, 0x3A, 0x32 };
static const uint8_t sbm800HalfStep60[8] PROGMEM  = { 0x2F, 0x2D, 0x1D, 0x0D, 0x0B, 0x09, 0x39, 0x29 };
static const uint8_t sbm800HalfStep100[8] PROGMEM = { 0x27, 0x2D, 0x1C, 0x0D, 0x03, 0x09, 0x38, 0x29 };

static const uint8_t sbm800FullStep0[4] PROGMEM   = { 0x3F, 0x3B, 0x1B, 0x1F };
static const uint8_t sbm800FullStep20[4] PROGMEM  = { 0x36, 0x32, 0x12, 0x16 };
static const uint8_t sbm800FullStep60[4] PROGMEM  = { 0x2D, 0x29, 0x09, 0x0D };
static const uint8_t sbm800FullStep100[4] PROGMEM = { 0x24, 0x20, 0x00, 0x04 };

static const uint8_t stepperAdd[SMC800_NUM_AXIS] PROGMEM = { StepperX, StepperY, StepperZ };

////////////////////////////////////////////////////////

CStepperSMC800::CStepperSMC800()
{
	_numAxes = SMC800_NUM_AXIS;
}

////////////////////////////////////////////////////////

void CStepperSMC800::OutSMC800Cmd(const uint8_t val)
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

	PORTL = val;

#elif defined(__AVR_ATmega328P__)

	PORTD = (PORTD & 3) + (val << 2);
	PORTB = (PORTB & 0b11111100) + (val >> 6);

#elif defined(_MSC_VER)
	val;
#else
	ToDo
#endif

	HALFastdigitalWrite(SMC800_STROBEPIN, 0);
	HALFastdigitalWrite(SMC800_STROBEPIN, 1);
}

////////////////////////////////////////////////////////

void CStepperSMC800::Init()
{
	super::Init();

	uint8_t i;
	for (i = 0; i < SMC800_NUM_AXIS; i++) _stepIdx[i] = 0;
	for (i = 0; i < SMC800_NUM_AXIS; i++) _level[i]   = LevelOff;
	for (i = 0; i < NUM_AXIS; i++) _fullStepMode[i]   = false;

	_pod._idleLevel = Level20P;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

	DDRL = 0xff;

#elif defined(__AVR_ATmega328P__)

	DDRD = (DDRD & 3) + 0b11111100;
	DDRB = (DDRB & 0b11111100) + 3;

#elif defined(_MSC_VER)

#else
	ToDo
#endif

	CHAL::pinModeOutput(SMC800_STROBEPIN);
	HALFastdigitalWrite(SMC800_STROBEPIN, 1);

	CHAL::pinModeInputPullUp(SMC800_REFININ);
}

////////////////////////////////////////////////////////

void CStepperSMC800::Remove()
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

	DDRL = 0x0;

#elif defined(__AVR_ATmega328P__)

	DDRD = DDRD & 3;
	DDRB = DDRB & 0b11111100;

#elif defined(_MSC_VER)

#else
	ToDo
#endif

	CHAL::pinMode(SMC800_REFININ, INPUT);
	CHAL::pinMode(SMC800_STROBEPIN, INPUT);
}

////////////////////////////////////////////////////////

void CStepperSMC800::Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool)
{
	for (axis_t axis = 0; axis < NUM_AXIS; axis++)
	{
		if (steps[axis])
		{
			if (directionUp & 1)
			{
				_stepIdx[axis] += steps[axis];
			}
			else
			{
				_stepIdx[axis] -= steps[axis];
			}
			SetPhase(axis);
		}
		directionUp /= 2;
	}
}

////////////////////////////////////////////////////////

void CStepperSMC800::SetEnable(axis_t axis, uint8_t level, bool force)
{
	if (axis < SMC800_NUM_AXIS)
	{
		if (level > Level60P) _level[axis] = LevelMax;
#ifndef REDUCED_SIZE
		else if (level > Level20P) _level[axis] = Level60P;
#endif
		else if (level > LevelOff) _level[axis] = Level20P;
		else _level[axis]                       = LevelOff;

		if (force) SetPhase(axis);
	}
}

////////////////////////////////////////////////////////

uint8_t CStepperSMC800::GetEnable(axis_t axis)
{
	if (axis >= SMC800_NUM_AXIS) return 0;
	return _level[axis];
}

////////////////////////////////////////////////////////

void CStepperSMC800::SetPhase(axis_t axis)
{
	if (axis < SMC800_NUM_AXIS)
	{
		uint8_t addIO   = pgm_read_byte(&stepperAdd[axis]);
		uint8_t stepIdx = _stepIdx[axis];

		if (_fullStepMode[axis])
		{
			stepIdx = stepIdx & 0x3;
			switch (_level[axis])
			{
				// @formatter:off — disable formatter after this line
				default:
				case LevelMax: OutSMC800Cmd(pgm_read_byte(&sbm800FullStep100[stepIdx]) + addIO); break;
#ifndef REDUCED_SIZE
				case Level60P: OutSMC800Cmd(pgm_read_byte(&sbm800FullStep60[stepIdx]) + addIO);	break;
#endif
				case Level20P: OutSMC800Cmd(pgm_read_byte(&sbm800FullStep20[stepIdx]) + addIO);	break;
				case LevelOff: OutSMC800Cmd(pgm_read_byte(&sbm800FullStep0[stepIdx]) + addIO);	break;
					// @formatter:on — enable formatter after this line
			}
		}
		else
		{
			stepIdx = stepIdx & 0x7;
			switch (_level[axis])
			{
				// @formatter:off — disable formatter after this line
				default:
				case LevelMax: OutSMC800Cmd(pgm_read_byte(&sbm800HalfStep100[stepIdx]) + addIO); break;
#ifndef REDUCED_SIZE
				case Level60P: OutSMC800Cmd(pgm_read_byte(&sbm800HalfStep60[stepIdx]) + addIO);	break;
#endif
				case Level20P: OutSMC800Cmd(pgm_read_byte(&sbm800HalfStep20[stepIdx]) + addIO);	break;
				case LevelOff: OutSMC800Cmd(pgm_read_byte(&sbm800HalfStep0[stepIdx]) + addIO);	break;
					// @formatter:on — enable formatter after this line
			}
		}
	}
}

////////////////////////////////////////////////////////

uint8_t CStepperSMC800::GetReferenceValue(uint8_t /*referenceId*/)
{
	return HALFastdigitalRead(SMC800_REFININ);
}

////////////////////////////////////////////////////////

void CStepperSMC800::MoveAwayFromReference(axis_t /* axis */, sdist_t dist, steprate_t vMax)
{
#ifndef REDUCED_SIZE
	MoveRelEx(vMax, X_AXIS, min(dist, (sdist_t)GetLimitMax(X_AXIS) / 2),
	          Y_AXIS, min(dist, (sdist_t)GetLimitMax(Y_AXIS) / 2),
	          Z_AXIS, min(dist, (sdist_t)GetLimitMax(Z_AXIS) / 2),
	          -1);
#else
	MoveRelEx(vMax, X_AXIS, min((mdist_t) dist, 256), 
					Y_AXIS, min((mdist_t) dist, 256), 
					Z_AXIS, min((mdist_t) dist, 256), 
					-1);
#endif
}

#endif
