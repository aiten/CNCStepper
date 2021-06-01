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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <CNCLib.h>
#include <CNCLibEx.h>

#include <Beep.h>

#include "MyLcd.h"
#include "MyControl.h"
#include "RotaryButton.h"
#include "GCodeParser.h"
#include "GCode3DParser.h"
#include <U8g2lib.h>

////////////////////////////////////////////////////////////
//
// used full graphic controller for Ramps 1.4
//
////////////////////////////////////////////////////////////

#ifdef USE_U8G2_LIB
U8G2_ST7920_128X64_1_SW_SPI u8g(U8G2_R0, 23, 17, 16);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
#else
U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
#endif

U8G2& CMyLcd::GetU8G() { return u8g; }

////////////////////////////////////////////////////////////

CMyLcd Lcd;

////////////////////////////////////////////////////////////

PROGMEM const CU8GLcd::SPageDef CU8GLcd::_pageDef[] =
{
	{ &CU8GLcd::DrawLoopSplash, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopPosAbs, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopPos, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopRotate2D, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopRotate3D, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopDebug, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopSpeedOverride, &CU8GLcd::ButtonPressSpeedOverride },
	{ &CU8GLcd::DrawLoopPreset, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopStartSD, &CU8GLcd::ButtonPressStartSDPage },
	{ &CU8GLcd::DrawLoopError, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopCommandHis, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopMenu, &CU8GLcd::ButtonPressMenuPage },
	{ nullptr, nullptr }
};

////////////////////////////////////////////////////////////

void CMyLcd::Init()
{
	_lcd_numaxis = 4;

	CBeep<LCD_BEEPER>::Init();

	super::Init();

	SetRotaryPin(ROTARY_EN1,ROTARY_EN2, ROTARY_ENC, ROTARY_ENC_ON);

	CHAL::pinModeInputPullUp(LCD_KILL_PIN);
}

////////////////////////////////////////////////////////////

void CMyLcd::Beep(const SPlayTone* play, bool fromProgMem)
{
	if (fromProgMem)
	{
		CBeep<LCD_BEEPER>::PlayPGM(play);
	}
	else
	{
		CBeep<LCD_BEEPER>::Play(play);
	}
}

////////////////////////////////////////////////////////////

bool CMyLcd::DrawLoopDefault(EnumAsByte(EDrawLoopType) type, uintptr_t data)
{
	if (type == DrawLoopHeader)
	{
		DrawString(ToCol(0), ToRow(0), F("iRobot Mega"));
		return true;
	}

	return super::DrawLoopDefault(type, data);
}
