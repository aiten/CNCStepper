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

//#define FORCE_NO_LCD

////////////////////////////////////////////////////////

#if  defined(FORCE_NO_LCD) || defined(__AVR_ATmega328P__) || defined(__SAM3X8E__)

#else

#define MYUSE_LCD

////////////////////////////////////////////////////////

#include <LiquidCrystal_I2C.h>
#include <Lcd.h>

////////////////////////////////////////////////////////

#define MYLCD_ROWS	4
#define MYLCD_COLS	20

////////////////////////////////////////////////////////

class CMyLcd : public CLcd
{
private:

	typedef CLcd super;

public:

	virtual void Init() override;
	virtual void TimerInterrupt() override;

	virtual void Beep(const SPlayTone*, bool) override { };

protected:

	virtual unsigned long Draw(EDrawType draw) override;
	virtual unsigned long Splash() override;
	void                  FirstDraw();

private:

	void DrawPos(uint8_t col, uint8_t row, mm1000_t pos);
	void DrawES(uint8_t  col, uint8_t row, bool     es);

	void DrawPen(uint8_t col, uint8_t row);
};

////////////////////////////////////////////////////////

extern CMyLcd Lcd;

#endif
