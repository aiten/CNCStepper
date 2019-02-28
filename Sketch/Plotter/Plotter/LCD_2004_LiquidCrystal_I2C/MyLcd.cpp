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

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <StepperLib.h>
#include <CNCLib.h>

#include "..\MyControl.h"
#include "MyLcd.h"

#ifdef MYUSE_LCD

#include "..\PlotterControl.h"
//#include "HPGLParser.h"

////////////////////////////////////////////////////////////

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

////////////////////////////////////////////////////////////

//LiquidCrystal_I2C	lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
LiquidCrystal_I2C lcd(I2C_ADDR, MYLCD_COLS, MYLCD_ROWS);

////////////////////////////////////////////////////////////

void CMyLcd::Init()
{
	lcd.begin(MYLCD_COLS, MYLCD_ROWS);      // initialize the lcd 
	lcd.backlight();						// finish with backlight on  

	super::Init();
}

////////////////////////////////////////////////////////////

void CMyLcd::TimerInterrupt()
{
	super::TimerInterrupt();
	/*
		if (READ(KILL_PIN)==0)
		{
			CStepper::GetInstance()->AbortMove();
		}

		button.Tick(READ(BTN_EN1),READ(BTN_EN2));
	*/
}

////////////////////////////////////////////////////////////

void CMyLcd::FirstDraw()
{
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("X:xxx.xx"));
	lcd.setCursor(0, 1);
	lcd.print(F("Y:xxx.xx"));
	lcd.setCursor(0, 2);
	lcd.print(F("Pen:x"));
}

////////////////////////////////////////////////////////////

unsigned long CMyLcd::Splash()
{
	lcd.clear();
	lcd.setCursor(6, 0); //Start at character 4 on line 0
	lcd.print(F("Hello"));

	lcd.setCursor(0, 2);
	lcd.print(F("Herbert Aitenbichler"));
	lcd.setCursor(0, 3);
	lcd.print(F(__DATE__ "," __TIME__));

	delay(100);

	return 5000l;
}

////////////////////////////////////////////////////////////

unsigned long CMyLcd::Draw(EDrawType /* draw */)
{
	static bool firstDraw = true;

	if (firstDraw)
	{
		FirstDraw();
		firstDraw = false;
	}
	DrawPos(2, 0, CMotionControlBase::GetInstance()->GetPosition(X_AXIS));
	DrawPos(2, 1, CMotionControlBase::GetInstance()->GetPosition(Y_AXIS));
	DrawES(17, 0, CStepper::GetInstance()->GetReferenceValue(CStepper::GetInstance()->ToReferenceId(X_AXIS, true)));
	DrawES(18, 0, CStepper::GetInstance()->GetReferenceValue(CStepper::GetInstance()->ToReferenceId(Y_AXIS, true)));
	DrawES(19, 0, CStepper::GetInstance()->GetReferenceValue(CStepper::GetInstance()->ToReferenceId(Z_AXIS, true)));
	DrawPen(4, 2);

	lcd.setCursor(0, 3);
	lcd.print(CStepper::GetInstance()->GetTotalSteps());

	uint8_t queued = CStepper::GetInstance()->QueuedMovements();
	lcd.setCursor(18, 3);
	if (queued < 10)
		lcd.print(' ');
	lcd.print((short)queued);

	return 333;
}

////////////////////////////////////////////////////////////

void CMyLcd::DrawPos(uint8_t col, uint8_t row, mm1000_t pos)
{
	lcd.setCursor(col, row);

	char tmp[16];

	CMm1000::ToString(pos, tmp, 6, 2);

	lcd.print(tmp);
}

////////////////////////////////////////////////////////////

void CMyLcd::DrawES(uint8_t col, uint8_t row, bool es)
{
	lcd.setCursor(col, row);
	lcd.print(es ? F("1") : F("0"));
}

////////////////////////////////////////////////////////////

void CMyLcd::DrawPen(uint8_t col, uint8_t row)
{
	lcd.setCursor(col, row);
	lcd.print(Plotter.GetPen());
	lcd.print(Plotter.IsPenDown() ? F(" down") : F(" up  "));
}

#endif
