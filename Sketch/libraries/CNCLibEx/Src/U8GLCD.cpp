////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2016 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#define SPEEDOVERIDESTEPSIZE	5

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arduino.h>
#include <U8glib.h>

#include <CNCLib.h>
#include <CNCLibEx.h>

#include "Beep.h"
#include "RotaryButton.h"

#include "U8GLCD.h"

#include "GCodeParser.h"
#include "GCode3DParser.h"

////////////////////////////////////////////////////////////

#define HeadLineOffset (-2)
#define PosLineOffset  (_lcd_numaxis > 5 ? 0 : 1)

////////////////////////////////////////////////////////////
//
// used full graphic controller for Ramps 1.4 or FD
//
////////////////////////////////////////////////////////////

CU8GLcd::CU8GLcd()
{  
}

////////////////////////////////////////////////////////////

void CU8GLcd::Init()
{
	if (_lcd_numaxis > 5)
	{
		_charHeight = 9;
		_charWidth = 6;
		_font = u8g_font_6x12;
	}
	super::Init();

	GetMenu().SetMainMenu();
	SetDefaultPage();
}

////////////////////////////////////////////////////////////

uint8_t CU8GLcd::GetPageCount()
{
	uint8_t count;
	for (count = 0; GetDrawFunction(&_pagedef[count].draw) != NULL; count++)
	{
	}
	return count;
}

////////////////////////////////////////////////////////////

void CU8GLcd::SetDefaultPage()
{
	_currentpage = 1;					// TODO: first (0 based) page is default
	SetRotaryFocusMainPage();
}

////////////////////////////////////////////////////////////

void CU8GLcd::SetMenuPage()
{
	_currentpage = GetPageCount()-1;	// TODO: last is default menu
	GetMenu().SetMainMenu();
	SetRotaryFocusMenuPage();
	OKBeep();
}

////////////////////////////////////////////////////////////

uint8_t CU8GLcd::GetPage()
{
	if (_rotaryFocus == RotaryMainPage)
	{
		uint8_t page = (uint8_t) _rotarybutton.GetPageIdx(GetPageCount());

		if (page != _currentpage)
		{
			Invalidate();
			_currentpage = page;
			GetMenu().SetMainMenu();
		}
	}

	return _currentpage;
}

////////////////////////////////////////////////////////////

void CU8GLcd::SetRotaryFocusMainPage()
{
	_rotarybutton.SetPageIdx((rotarypos_t) _currentpage); _rotarybutton.SetMinMax(0, GetPageCount() - 1, true);
	_rotaryFocus = RotaryMainPage;
}

////////////////////////////////////////////////////////////

void CU8GLcd::TimerInterrupt()
{
	super::TimerInterrupt();

	switch (_rotarybutton.Tick())
	{
		case CRotaryButton<rotarypos_t, ROTARY_ACCURACY>::Overrun:
			break;
		case CRotaryButton<rotarypos_t, ROTARY_ACCURACY>::Underflow:
			break;
	}

	_rotarypushbutton.Check();
}

////////////////////////////////////////////////////////////

void CU8GLcd::Poll()
{
	GetPage();		// force invalidate if page changed

	super::Poll();

	if (_rotarypushbutton.IsOn())
	{
		ButtonPress();
	}
}

////////////////////////////////////////////////////////////

void CU8GLcd::Command(char* buffer)
{
	super::Command(buffer);

	if (*buffer)
	{
		uint8_t totalcols = TotalCols();
		for (uint8_t commandlenght = 0; *buffer && commandlenght < totalcols; commandlenght++)
		{
			QueueCommandHistory(*(buffer++));
		}
		QueueCommandHistory(0);
	}
}

////////////////////////////////////////////////////////////

void CU8GLcd::QueueCommandHistory(char ch)
{
	if (_commandHis.IsFull())
	{
		// dequeue last command
		do
		{
			_commandHis.Dequeue();
		} 
		while (!_commandHis.IsEmpty() && _commandHis.Head() != 0);
	}
	_commandHis.Enqueue(ch);

}

////////////////////////////////////////////////////////////

unsigned long CU8GLcd::Draw(EDrawType draw)
{
	if (draw==DrawFirst)
	{
		SetDefaultPage();
	}

	DrawFunction fnc = GetDrawFunction(&_pagedef[GetPage()].draw);

	return DrawLoop(fnc);
}

////////////////////////////////////////////////////////////

unsigned long CU8GLcd::DrawLoop()
{
	unsigned long timeout = 1000;

	if (_curretDraw)
	{
		if ((this->*_curretDraw)(DrawLoopSetup,0))
		{
			GetU8G().firstPage();
			do
			{
				if (!(this->*_curretDraw)(DrawLoopHeader,0))
					break;

				if (!(this->*_curretDraw)(DrawLoopDraw,0))
					break;
			} 
			while (GetU8G().nextPage());
		}
		
		(this->*_curretDraw)(DrawLoopQueryTimerout,(ptr_t) &timeout);
	}
	return timeout;
}

////////////////////////////////////////////////////////////

#if defined(__AVR_ARCH__)

CU8GLcd::ButtonFunction CU8GLcd::GetButtonPress(const void* adr)
{
	struct ButtonFunctionWrapper
	{
		ButtonFunction fnc;
	}x;

	memcpy_P(&x, adr, sizeof(ButtonFunctionWrapper));

	return x.fnc;
}

CU8GLcd::DrawFunction CU8GLcd::GetDrawFunction(const void* adr)
{
	struct DrawFunctionWrapper
	{
		DrawFunction fnc;
	}x;

	memcpy_P(&x, adr, sizeof(DrawFunctionWrapper));

	return x.fnc;
}

#endif

////////////////////////////////////////////////////////////

char* CU8GLcd::DrawPos(axis_t axis, mm1000_t pos, char*tmp,  uint8_t precision)
{
	if (CGCodeParserBase::IsInch(axis))
	{
//		return CInch100000::ToString(MulDivI32(pos, 1000, 254), tmp, precision, 4);
		return CInch100000::ToString(MulDivI32(pos, 500, 127), tmp, precision, 4);
	}
	
	return CMm1000::ToString(pos, tmp, precision, 2);
}
								
////////////////////////////////////////////////////////////

void CU8GLcd::ButtonPress()
{
	ButtonFunction fnc = GetButtonPress(&_pagedef[GetPage()].buttonpress);

	if (fnc)
	{
		(*this.*fnc)();
		DrawRequest(DrawForceAll);
	}
}

////////////////////////////////////////////////////////////

unsigned long CU8GLcd::Splash()
{
	DrawLoop(&CU8GLcd::DrawLoopSplash);
	OKBeep();
	return 3000;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopDefault(EnumAsByte(EDrawLoopType) type, ptr_t /* data */)
{
	switch (type)
	{
		case DrawLoopSetup:
		{
			GetU8G().setFont(_font);
			return true;
		}
/*		=> default is 1000
		case DrawLoopQueryTimerout: 
		{
			*((unsigned long*)data) = 1000;
			return true;
		}
*/
	}
	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopSplash(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 200000; return true; }
	if (type!=DrawLoopDraw)	return DrawLoopDefault(type,data);

	GetU8G().drawStr(ToCol(TotalCols() / 2 - 1), ToRow(2), F("by"));
	GetU8G().drawStr(ToCol(3), ToRow(3), F("H. Aitenbichler"));
	GetU8G().drawStr(ToCol(5), ToRow(5), F(__DATE__));

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopDebug(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type==DrawLoopHeader)	return true;
	if (type!=DrawLoopDraw)		return DrawLoopDefault(type,data);

	GetU8G().drawStr(ToCol(0), ToRow(0) + HeadLineOffset, F("Debug"));

	char tmp[16];

	for (uint8_t i = 0; i < _lcd_numaxis; i++)
	{
		GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);

		udist_t pos = CStepper::GetInstance()->GetCurrentPosition(i);

		GetU8G().print(CSDist::ToString(pos, tmp, 6));
		GetU8G().print(F(" "));
		GetU8G().print(DrawPos(i,CMotionControlBase::GetInstance()->ToMm1000(i, pos),tmp,6));
		GetU8G().print(F(" "));

		GetU8G().print(CStepper::GetInstance()->IsReference(CStepper::GetInstance()->ToReferenceId(i, true)) ? '1' : '0');
		GetU8G().print(CStepper::GetInstance()->IsReference(CStepper::GetInstance()->ToReferenceId(i, false)) ? '1' : '0');

		GetU8G().print((CStepper::GetInstance()->GetLastDirection()&(1 << i)) ? '+' : '-');
	}

	GetU8G().setPrintPos(ToCol(20), ToRow(0 + 1) + PosLineOffset);
	GetU8G().print(CControl::GetInstance()->IOControl(CControl::Probe) ? '1' : '0');

	GetU8G().setPrintPos(ToCol(19), ToRow(0 + 2) + PosLineOffset);
	GetU8G().print(CSDist::ToString(CStepper::GetInstance()->QueuedMovements(), tmp, 2));

	GetU8G().setPrintPos(ToCol(18), ToRow(4) + PosLineOffset);
	GetU8G().print(CSDist::ToString(CStepper::SpeedOverrideToP(CStepper::GetInstance()->GetSpeedOverride()), tmp, 3));

	GetU8G().setPrintPos(ToCol(20), ToRow(5) + PosLineOffset);
	GetU8G().print(CControl::GetInstance()->IsHold() ? '1' : '0');

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopPosAbs(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type==DrawLoopHeader)	return true;
	if (type!=DrawLoopDraw)		return DrawLoopDefault(type,data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset); GetU8G().print(F("Absolut  Current"));
	char tmp[16];

	for (uint8_t i = 0; i < _lcd_numaxis; i++)
	{
		mm1000_t psall = CGCodeParser::GetAllPreset(i);

		GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);
		tmp[0] = 0; GetU8G().print(CMenuBase::AddAxisName(tmp,i));

		GetU8G().print(DrawPos(i,CMotionControlBase::GetInstance()->GetPosition(i),tmp,7));
		GetU8G().print(F(" "));
		GetU8G().print(DrawPos(i,CMotionControlBase::GetInstance()->GetPosition(i) - psall,tmp,7));
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopPos(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type == DrawLoopHeader)	return true;
	if (type != DrawLoopDraw)	return DrawLoopDefault(type, data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset); GetU8G().print(F("Absolut# Current"));
	char tmp[16];

	mm1000_t dest[NUM_AXIS];
	udist_t src[NUM_AXIS];
	CStepper::GetInstance()->GetCurrentPositions(src);

	CMotionControlBase::GetInstance()->GetPosition(src, dest);

	for (uint8_t i = 0; i < _lcd_numaxis; i++)
	{
		mm1000_t psall = CGCodeParser::GetAllPreset(i);

		GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);
		tmp[0] = 0; GetU8G().print(CMenuBase::AddAxisName(tmp, i));

		GetU8G().print(DrawPos(i,dest[i],tmp,7));
		GetU8G().print(F(" "));
		GetU8G().print(DrawPos(i,dest[i] - psall,tmp,7));
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopRotate2D(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type == DrawLoopHeader)	return true;
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 200000; return true; }
	if (type != DrawLoopDraw)	return DrawLoopDefault(type, data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset); GetU8G().print(F("Rotate 2D"));
	char tmp[16];

	for (uint8_t i = 0; i < NUM_AXISXYZ; i++)
	{
		GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);
		tmp[0] = 0; GetU8G().print(CMenuBase::AddAxisName(tmp, i));

		mm1000_t ofs = CMotionControl::GetInstance()->GetOffset2D(i);
		GetU8G().print(DrawPos(i,ofs,tmp,7));

		if (CMotionControl::GetInstance()->IsEnabled2D(i))
		{
			mm1000_t rad = CMm1000::FromRAD(CMotionControl::GetInstance()->GetAngle2D(i));

			GetU8G().print(F(" "));
			GetU8G().print(CMm1000::ToString(rad,tmp,7,2));
		}
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopRotate3D(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type == DrawLoopHeader)	return true;
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 200000; return true; }
	if (type != DrawLoopDraw)		return DrawLoopDefault(type, data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset); GetU8G().print(F("Rotate 3D"));

	if (CMotionControl::GetInstance()->IsRotate())
	{
		char tmp[16];
	
		for (uint8_t i = 0; i < NUM_AXISXYZ; i++)
		{
			mm1000_t ofs  = CMotionControl::GetInstance()->GetOffset(i);
			mm1000_t vect = CMotionControl::GetInstance()->GetVector(i);

			GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);
			tmp[0] = 0; GetU8G().print(CMenuBase::AddAxisName(tmp, i));

			GetU8G().print(DrawPos(i,ofs,tmp,7));
			GetU8G().print(F(" "));
			GetU8G().print(DrawPos(i,vect,tmp,7));
		}

		GetU8G().setPrintPos(ToCol(0), ToRow(NUM_AXISXYZ + 1) + PosLineOffset);
		GetU8G().print(F("R"));
		GetU8G().print(CMm1000::ToString(CMm1000::FromRAD(CMotionControl::GetInstance()->GetAngle()), tmp, 7, 2));
	}
	else
	{
		GetU8G().setPrintPos(ToCol(6), ToRow(2 + 1) + PosLineOffset);
		GetU8G().print(F("no rotation"));
	}

	return true;
}

////////////////////////////////////////////////////////////

inline uint8_t ToPageIdx(uint8_t idx)
{
	return idx / SPEEDOVERIDESTEPSIZE;
}

bool CU8GLcd::DrawLoopSpeedOverride(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type == DrawLoopHeader)			return true;
	if (type==DrawLoopQueryTimerout && _rotaryFocus == RotarySlider)	{ *((unsigned long*)data) = 333; return true; }
	if (type != DrawLoopDraw)			return DrawLoopDefault(type, data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset); GetU8G().print(F("Speed Override"));
	char tmp[16];

	GetU8G().setPrintPos(ToCol(8), ToRow(2 + 1) + PosLineOffset);

	if (_rotaryFocus == RotarySlider)
	{
		uint8_t speedInP = _rotarybutton.GetPageIdx(ToPageIdx(100)+1) * SPEEDOVERIDESTEPSIZE;
		CStepper::GetInstance()->SetSpeedOverride(CStepper::PToSpeedOverride(speedInP));
		GetU8G().print('>');
	}

	GetU8G().print(CSDist::ToString(CStepper::SpeedOverrideToP(CStepper::GetInstance()->GetSpeedOverride()), tmp, 3));

	if (_rotaryFocus == RotarySlider)
		GetU8G().print('<');

	return true;
}

////////////////////////////////////////////////////////////

void CU8GLcd::ButtonPressSpeedOverride()
{
	if (_rotaryFocus == RotarySlider)	
	{
		SetRotaryFocusMainPage();
		OKBeep();
	}
	else 
	{
		_rotarybutton.SetMinMax(1, ToPageIdx(100), false);
		_rotarybutton.SetPageIdx((rotarypos_t)ToPageIdx(CStepper::SpeedOverrideToP(CStepper::GetInstance()->GetSpeedOverride())));
		_rotaryFocus = RotarySlider;
		OKBeep();
	}
}

////////////////////////////////////////////////////////////

void CU8GLcd::ButtonPressShowMenu()
{
	SetMenuPage();
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopPreset(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type==DrawLoopHeader)			return true;
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 200000; return true; }
	if (type!=DrawLoopDraw)				return DrawLoopDefault(type,data);

	mm1000_t ps;

	const __FlashStringHelper* zeroShiftName[] PROGMEM = { F("G53"), F("G54"), F("G55"), F("G56"), F("G57"), F("G58"), F("G59") };

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset);  GetU8G().print(F("Preset: ")); GetU8G().print(zeroShiftName[CGCodeParser::GetZeroPresetIdx()]); GetU8G().print(F(" G92 Height"));

	char tmp[16];

	for (uint8_t i = 0; i < _lcd_numaxis; i++)
	{
		GetU8G().setPrintPos(ToCol(0), ToRow(i + 1) + PosLineOffset);
		tmp[0] = 0; GetU8G().print(CMenuBase::AddAxisName(tmp,i));
		ps = CGCodeParser::GetG54PosPreset(i);
		GetU8G().print(DrawPos(i,ps,tmp,7));

		ps = CGCodeParser::GetG92PosPreset(i);
		GetU8G().print(DrawPos(i,ps,tmp,7));

		ps = CGCodeParser::GetToolHeightPosPreset(i);
		GetU8G().print(DrawPos(i,ps, tmp, 6));
	}
	return true;
}

////////////////////////////////////////////////////////////

void CU8GLcd::ButtonPressStartSDPage()
{
	PostCommand(F("m21"));									// Init SD

	if (PostCommand(F("m23 proxxon.nc")))
	{
		PostCommand(F("m24"));
	}

	OKBeep();
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopStartSD(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type!=DrawLoopDraw)		return DrawLoopDefault(type,data);
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 5000; return true; }

	char tmp[16];

	if (!CGCode3DParser::GetExecutingFile())
  	    GetU8G().drawStr(ToCol(3), ToRow(2), F("Press to start"));

	GetU8G().setPrintPos(ToCol(0), ToRow(3) + PosLineOffset); GetU8G().print(F("File: ")); GetU8G().print(CGCode3DParser::GetExecutingFileName());
	GetU8G().setPrintPos(ToCol(0), ToRow(4) + PosLineOffset); GetU8G().print(F("At:   ")); GetU8G().print(CSDist::ToString(CGCode3DParser::GetExecutingFilePosition(), tmp, 8));
	GetU8G().setPrintPos(ToCol(0), ToRow(5) + PosLineOffset); GetU8G().print(F("Line: ")); GetU8G().print(CSDist::ToString(CGCode3DParser::GetExecutingFileLine(), tmp, 8));

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopError(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type!=DrawLoopDraw)		return DrawLoopDefault(type,data);
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 5000; return true; }

	uint8_t errors = 0;

	if (CStepper::GetInstance()->GetFatalError())
		GetU8G().drawStr(ToCol(0), ToRow(++errors + 1), CStepper::GetInstance()->GetFatalError());

	if (GetDiagnostic())
		GetU8G().drawStr(ToCol(0), ToRow(++errors + 1), GetDiagnostic());

	if (CControl::GetInstance()->IsKilled())
		GetU8G().drawStr(ToCol(0), ToRow(++errors + 1), F("emergency stop"));

	if (CControl::GetInstance()->IsHold())
		GetU8G().drawStr(ToCol(0), ToRow(++errors + 1), F("hold"));

	if (errors == 0)
		GetU8G().drawStr(ToCol(0), ToRow(2), F("no errors"));

	return true;
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopCommandHis(EnumAsByte(EDrawLoopType) type,ptr_t data)
{
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 5000; return true; }
	if (type!=DrawLoopDraw)		return DrawLoopDefault(type,data);

	uint8_t totalCols = TotalCols();
	uint8_t totalRows = TotalRows();
	//	char tmp[totalCols + 1];
	char tmp[40 + 1];
	uint8_t commandpos = _commandHis.T2HInit();	// idx of \0 of last command

	for (uint8_t i = 0; i < totalRows - 1; i++)
	{
		GetU8G().setPrintPos(ToCol(0), ToRow(totalRows - i - 1) + PosLineOffset);

		uint8_t idx = totalCols;
		tmp[idx] = 0;

		if (_commandHis.T2HTest(commandpos))
		{
			for (commandpos = _commandHis.T2HInc(commandpos); _commandHis.T2HTest(commandpos) && _commandHis.Buffer[commandpos] != 0;commandpos = _commandHis.T2HInc(commandpos))
			{
				tmp[--idx] = _commandHis.Buffer[commandpos];
			}
			GetU8G().print(&tmp[idx]);
		}
	}

	return true;
}

////////////////////////////////////////////////////////////

uint8_t CU8GLcd::GetMenuIdx()
{
	if (_rotaryFocus == RotaryMenuPage)
	{
		uint8_t menu = _rotarybutton.GetPageIdx(GetMenu().GetMenuDef()->GetItemCount());
		if (menu != GetMenu().GetPosition())
		{
			GetMenu().SetPosition(menu);
		}
	}

	return GetMenu().GetPosition();
}

////////////////////////////////////////////////////////////

void CU8GLcd::SetRotaryFocusMenuPage()
{
	_rotarybutton.SetPageIdx(GetMenu().GetPosition()); _rotarybutton.SetMinMax(0, GetMenu().GetMenuDef()->GetItemCount() - 1, false);
	_rotaryFocus = RotaryMenuPage;
}

////////////////////////////////////////////////////////////

void CU8GLcd::ButtonPressMenuPage()
{
	switch (_rotaryFocus)
	{
		case RotaryMainPage:	SetRotaryFocusMenuPage(); OKBeep();  break;
		case RotaryMenuPage:
		{
			if (!GetMenu().Select())
			{
				ErrorBeep();
			}

			break;
		}
	}
}

////////////////////////////////////////////////////////////

bool CU8GLcd::DrawLoopMenu(EnumAsByte(EDrawLoopType) type, ptr_t data)
{
	if (type==DrawLoopHeader)			return true;
	if (type==DrawLoopQueryTimerout)	{ *((unsigned long*)data) = 333; return true; }
	if (type!=DrawLoopDraw)				return DrawLoopDefault(type,data);

	GetU8G().setPrintPos(ToCol(0), ToRow(0) + HeadLineOffset);
	GetU8G().print(F("Menu: "));
	GetU8G().print(GetMenu().GetMenuDef()->GetText());

	uint8_t x = 255;
	const uint8_t printFirstLine = 1;
	const uint8_t printLastLine = (TotalRows()- 1);
	const uint8_t menuEntries = GetMenu().GetMenuDef()->GetItemCount();

	if (_rotaryFocus == RotaryMenuPage)
	{
		x = GetMenuIdx();													// get and set menupositions
		GetMenu().AdjustOffset(printFirstLine, printLastLine);
	}

	uint8_t i;

	for (i = 0; i < menuEntries; i++)
	{
		uint8_t printtorow = GetMenu().ToPrintLine(printFirstLine, printLastLine, i);
		if (printtorow != 255)
		{
			GetU8G().setPrintPos(ToCol(0), ToRow(printtorow) + PosLineOffset);
			if (i == x && _rotaryFocus == RotaryMenuPage)
				GetU8G().print(F(">"));
			else
				GetU8G().print(F(" "));


			GetU8G().print(GetMenu().GetMenuDef()->GetItems()[i].GetText());
		}
	}

	return true;
}

////////////////////////////////////////////////////////////
