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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <CNCLib.h>
#include <CNCLibEx.h>

#include <Beep.h>

#include "MyLcd.h"
#include "../MyControl.h"
#include "../PlotterControl.h"
#include <RotaryButton.h>
#include <GCodeParser.h>
#include <GCode3DParser.h>
#include <U8g2lib.h>


////////////////////////////////////////////////////////////
//
// used full graphic controller for Ramps 1.4
//
////////////////////////////////////////////////////////////

#ifdef USE_U8G2_LIB
U8G2_ST7920_128X64_1_SW_SPI u8g(U8G2_R0, CAT(BOARDNAME,_ST7920_CLK_PIN), CAT(BOARDNAME,_ST7920_DAT_PIN), CAT(BOARDNAME,_ST7920_CS_PIN));	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
#else
U8GLIB_ST7920_128X64_1X u8g(CAT(BOARDNAME, _ST7920_CLK_PIN), CAT(BOARDNAME, _ST7920_DAT_PIN), CAT(BOARDNAME, _ST7920_CS_PIN));	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
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
	{ (DrawFunction)&CMyLcd::DrawLoopDebugPlotter, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopSpeedOverride, &CU8GLcd::ButtonPressSpeedOverride },
	//	{ &CU8GLcd::DrawLoopPreset, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopStartSD, &CU8GLcd::ButtonPressStartSDPage },
	{ &CU8GLcd::DrawLoopError, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopCommandHis, &CU8GLcd::ButtonPressShowMenu },
	{ &CU8GLcd::DrawLoopMenu, &CU8GLcd::ButtonPressMenuPage },
	{ NULL, NULL }
};

////////////////////////////////////////////////////////////

uint8_t CMyLcd::InitPostCommand(EnumAsByte(EGCodeSyntaxType) syntaxType, char* cmd)
{
	uint8_t idx = super::InitPostCommand(syntaxType, cmd);
	if (idx != 7)
	{
		cmd[idx++] = 27;		// escape
		cmd[idx]   = 0;
	}
	return idx;
}

////////////////////////////////////////////////////////////

void CMyLcd::Init()
{
	_lcd_numaxis = LCD_NUMAXIS;

	CBeep<(CAT(BOARDNAME, _LCD_BEEPER))>::Init();

	super::Init();

	SetRotaryPin(ROTARY_EN1,ROTARY_EN2, ROTARY_ENC, ROTARY_ENC_ON);
}

////////////////////////////////////////////////////////////

void CMyLcd::Beep(const SPlayTone* play, bool fromProgMem)
{
	if (fromProgMem)
	{
		CBeep<CAT(BOARDNAME, _LCD_BEEPER)>::PlayPGM(play);
	}
	else
	{
		CBeep<CAT(BOARDNAME, _LCD_BEEPER)>::Play(play);
	}
}

////////////////////////////////////////////////////////////

bool CMyLcd::DrawLoopDefault(EnumAsByte(EDrawLoopType) type, uintptr_t data)
{
	if (type == DrawLoopHeader)
	{
#if defined(__SAM3X8E__)
		DrawString(ToCol(4), ToRow(0), F("Plotter Due"));
#else
		DrawString(ToCol(4), ToRow(0), F("Plotter Mega"));
#endif
		return true;
	}

	return super::DrawLoopDefault(type, data);
}

////////////////////////////////////////////////////////////

bool CMyLcd::DrawLoopDebugPlotter(EnumAsByte(EDrawLoopType) type, uintptr_t data)
{
	bool ret = DrawLoopDebug(type, data);

	char tmp[16];

	if (type == DrawLoopDraw)
	{
		SetPosition(ToCol(0), ToRow(0 + 5) + PosLineOffset());
		Print(CSDist::ToString(Plotter.GetPen(), tmp, 2));
		Print(Plotter.IsPenDown() ? F(" is down") : F(" is up"));
		if (Plotter.IsResumePenDown())
			Print(F("(resume)"));
	}

	return ret;
}

////////////////////////////////////////////////////////////
