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

#include "Configuration.h"

////////////////////////////////////////////////////////

#include "Menu3D.h"

////////////////////////////////////////////////////////

class CMyLcd;

class CMyMenu : public CMenu3D
{
private:

	typedef CMenu3D super;

public:

	static CMyLcd* GetLcd() { return reinterpret_cast<CMyLcd*>(CLcd::GetInstance()); }

	CMyMenu()
	{
		_main      = &_mainMenu;
		_SDMenuDef = &_SDMenu;
	}

protected:

	virtual void Changed() override;

	void MenuButtonPressEnd(const SMenuItemDef*         );
	void MenuButtonPressMoveNextAxis(const SMenuItemDef*);
	void MenuButtonPressFuerElise(const SMenuItemDef*   );

	static const SMenuDef _mainMenu PROGMEM;
	static const SMenuDef _moveXMenu PROGMEM;
	static const SMenuDef _moveYMenu PROGMEM;
	static const SMenuDef _moveZMenu PROGMEM;
	static const SMenuDef _moveAMenu PROGMEM;
	static const SMenuDef _moveBMenu PROGMEM;
	static const SMenuDef _moveCMenu PROGMEM;
	static const SMenuDef _rotateMenu PROGMEM;
	static const SMenuDef _SDMenu PROGMEM;
	static const SMenuDef _SDSelectMenu PROGMEM;
	static const SMenuDef _extraMenu PROGMEM;

	static const SMenuItemDef _mainMenuItems[] PROGMEM;
	static const SMenuItemDef _moveMenuItems[] PROGMEM;
	static const SMenuItemDef _rotateMenuItems[] PROGMEM;
	static const SMenuItemDef _SDMenuItems[] PROGMEM;
	static const SMenuItemDef _SDSelectMenuItems[] PROGMEM;
	static const SMenuItemDef _extraMenuItems[] PROGMEM;
};
