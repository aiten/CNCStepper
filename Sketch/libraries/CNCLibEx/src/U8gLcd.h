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

#define USE_U8G2_LIB
#include <U8g2lib.h>

#ifndef USE_U8G2_LIB
#define U8G2 U8GLIB
#endif

#include <Lcd.h>
#include <RotaryButton.h>
#include <PushButton.h>
#include "ConfigurationCNCLibEx.h"

////////////////////////////////////////////////////////////

#define CU8GLcd_ROTARY_ACCURACY	4

#define CU8GLcd_LCD_GROW 64
#define CU8GLcd_LCD_GCOL 128

#define CU8GLcd_SCREENSAVERTIMEOUT 120000

////////////////////////////////////////////////////////

class CU8GLcd : public CLcd
{
private:

	typedef CLcd super;

public:

	CU8GLcd();

	virtual void Init() override;

	enum EDrawLoopType
	{
		DrawLoopSetup,
		DrawLoopHeader,
		DrawLoopDraw,
		DrawLoopQueryTimerout
	};

	typedef bool (CU8GLcd::*DrawFunction)(EnumAsByte(EDrawLoopType) type, uintptr_t data);
	typedef void (CU8GLcd::*ButtonFunction)();

protected:

	virtual void Poll() override;
	virtual void TimerInterrupt() override;
	virtual void Command(char* cmd) override;

	////////////////////////////////////////////////////////

protected:

	virtual class U8G2&    GetU8G() = 0;
	virtual class CMenu3D& GetMenu() = 0;

	uint8_t GetPageCount();

	inline void DrawString(uint8_t x, uint8_t y, FLSTR s)
	{
#ifdef USE_U8G2_LIB
		GetU8G().setCursor(x, y);
		GetU8G().print(s);
#else
		GetU8G().drawStr(x, y, s);
#endif
	}

	inline void SetPosition(uint8_t x, uint8_t y)
	{
#ifdef USE_U8G2_LIB
		GetU8G().setCursor(x, y);
#else
		GetU8G().setPrintPos(x, y);
#endif
	}

#ifndef _MSC_VER

	inline void Print(FLSTR s)
	{
		GetU8G().print(s);
	}

#endif

	inline void Print(const char* s)
	{
		GetU8G().print(s);
	}
#
	inline void Print(char ch)
	{
		GetU8G().print(ch);
	}

public:

	////////////////////////////////////////////////////////
	// for menu

	void SetDefaultPage();

	void MenuChanged()
	{
		SetRotaryFocusMenuPage();
		DrawLoop();
		OKBeep();
	}

	uint8_t GetMenuIdx();

	bool PrintMenuLine(uint8_t& drawidx, uint8_t selectedIdx, bool& isselected);

	////////////////////////////////////////////////////////

protected:

	struct SPageDef
	{
		DrawFunction   draw;
		ButtonFunction buttonpress;
	};

	static const SPageDef _pageDef[];

	typedef signed int rotarypos_t;

	enum ERotaryFocus
	{
		RotaryMainPage,
		RotaryMenuPage,
		RotarySlider
	};

	uint8_t GetPage();

	virtual uint32_t Draw(EDrawType draw) override;
	virtual uint32_t Splash() override;

	void QueueCommandHistory(char ch);

	uint32_t DrawLoop(DrawFunction drawfnc)
	{
		_curretDraw = drawfnc;
		return DrawLoop();
	}

	uint32_t DrawLoop();

	virtual bool DrawLoopDefault(EnumAsByte(EDrawLoopType) type, uintptr_t data);

	void SetMenuPage();

	void ButtonPress();

	void SetRotaryFocusMainPage();
	void SetRotaryFocusMenuPage();

	void ButtonPressStartSDPage();
	void ButtonPressMenuPage();
	void ButtonPressShowMenu();
	void ButtonPressSpeedOverride();

	bool DrawLoopScreenSaver(EnumAsByte(EDrawLoopType)   type, uintptr_t data);
	bool DrawLoopSplash(EnumAsByte(EDrawLoopType)        type, uintptr_t data);
	bool DrawLoopDebug(EnumAsByte(EDrawLoopType)         type, uintptr_t data);
	bool DrawLoopPosAbs(EnumAsByte(EDrawLoopType)        type, uintptr_t data);
	bool DrawLoopPos(EnumAsByte(EDrawLoopType)           type, uintptr_t data);
	bool DrawLoopRotate2D(EnumAsByte(EDrawLoopType)      type, uintptr_t data);
	bool DrawLoopRotate3D(EnumAsByte(EDrawLoopType)      type, uintptr_t data);
	bool DrawLoopSpeedOverride(EnumAsByte(EDrawLoopType) type, uintptr_t data);
	bool DrawLoopPreset(EnumAsByte(EDrawLoopType)        type, uintptr_t data);
	bool DrawLoopStartSD(EnumAsByte(EDrawLoopType)       type, uintptr_t data);
	bool DrawLoopError(EnumAsByte(EDrawLoopType)         type, uintptr_t data);
	bool DrawLoopCommandHis(EnumAsByte(EDrawLoopType)    type, uintptr_t data);
	bool DrawLoopMenu(EnumAsByte(EDrawLoopType)          type, uintptr_t data);

private:

	DrawFunction _curretDraw = nullptr;

	EnumAsByte(ERotaryFocus) _rotaryFocus = RotaryMainPage;

	uint8_t _currentPage;

	int8_t _screenSaveX     = 2;
	int8_t _screenSaveY     = 0;
	int8_t _screenSaveXDiff = 1;
	int8_t _screenSaveYDiff = 1;

	uint32_t _screenSaveTime = 0;

	uint8_t _SDFileCount  = 255;
	int8_t  _addMenuItems = 0;

	CRingBufferQueue<char, 128> _commandHis;

	bool IsScreenSaver() const;

protected:

	void SetRotaryPin(pin_t pin1, pin_t pin2, pin_t pinPush, uint8_t onValuePush);

	void        CallRotaryButtonTick();
	static void CallRotaryButtonTickISR() { static_cast<CU8GLcd*>(CLcd::GetInstance())->CallRotaryButtonTick(); }

	CRotaryButton<rotarypos_t, CU8GLcd_ROTARY_ACCURACY> _rotarybutton;
	CPushButton                                         _rotarypushbutton;

	uint32_t _rotaryEventTime = 0;

	uint8_t _lcd_numaxis = NUM_AXIS;
	uint8_t _charHeight  = 10;
	uint8_t _charWidth   = 6;

	const uint8_t* _font = u8g_font_6x10;

	uint8_t ToRow(uint8_t row) { return (row + 1) * (_charHeight); }
	uint8_t ToCol(uint8_t col) { return (col) * (_charWidth); }

	uint8_t TotalRows() { return CU8GLcd_LCD_GROW / _charHeight; }
	uint8_t TotalCols() { return CU8GLcd_LCD_GCOL / _charWidth; }

	uint8_t HeadLineOffset() { return 2; }
	uint8_t PosLineOffset() { return (_lcd_numaxis > 5 ? 0 : 1); }

	static char* DrawPos(axis_t axis, mm1000_t pos, char* tmp, uint8_t precision); // draw mm100 or inch

#if defined(__AVR_ARCH__)

    static ButtonFunction GetButtonPress(const void* adr);
    static DrawFunction GetDrawFunction(const void* adr);

#else
	static ButtonFunction GetButtonPress(const ButtonFunction* adr) { return *adr; }
	static DrawFunction   GetDrawFunction(const DrawFunction*  adr) { return *adr; };

#endif
};

////////////////////////////////////////////////////////
