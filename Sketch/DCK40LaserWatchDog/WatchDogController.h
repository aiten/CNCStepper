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

#include "WaterFlow.h"
#include "WatchDog.h"
#include "LinearLookup.h"

#define RELAY1_PIN	11
#define RELAY2_PIN	10

#define INPUT1_PIN	9
#define INPUT2_PIN	8
#define INPUT3_PIN	7

////////////////////////////////////////////////////////////

#define MINDRAW_INTERVALL 100

////////////////////////////////////////////////////////////


#define ALIVE_PIN 13
#define ALIVE_BLINK_RATE 250

#define WATCHDOG_PIN  RELAY1_PIN
#define WATCHDOG_ON LOW

#define WATERFLOW_PIN 2
#define WATCHDOG_MINFLOW  50

#define WATERTEMP_PIN A0

#define WATCHDOG_MINTEMPON  4
#define WATCHDOG_MINTEMPOFF 5
#define WATCHDOG_MAXTEMPON  35.0
#define WATCHDOG_MAXTEMPOFF 34.5

#define WATERTEMP_OVERSAMPLING 16

////////////////////////////////////////////////////////////

class WatchDogController
{
public:

	WatchDogController() {};

	void Setup();
	void Loop();

private:

	uint32_t     _redrawTime = 0;
	unsigned int _secActive  = 0;

	uint32_t _lastBlink      = 0;
	bool     _blinkWasOn     = true;
	uint32_t _lastDraw       = 0;
	bool     _drawLCDRequest = false;

	WaterFlow _flow;
	WatchDog  _watchDog;

	float ReadTemp();

	bool IsWatchDogWaterFlowOn();
	bool IsWatchDogTempOn();

	bool IsWatchDogSW1On();
	bool IsWatchDogSW2On();
	bool IsWatchDogSW3On();

	bool IsWatchDogOn();

	void DrawLcd();

	float        _currentTemp = 0.0;
	unsigned int _currentFlow = 0xffff;

	float        _lastTemp = 0.0;
	unsigned int _lastFlow = 0xffff;

	bool _sw1On = true;
	bool _sw2On = true;
	bool _sw3On = true;
};

////////////////////////////////////////////////////////////
