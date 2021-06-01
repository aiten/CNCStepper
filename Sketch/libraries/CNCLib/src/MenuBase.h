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

#include "CNCLibTypes.h"
#include "Lcd.h"
#include "MenuNavigator.h"

////////////////////////////////////////////////////////

class CMenuBase
{
public:

	CMenuBase() { SetMenu(nullptr); }

	struct SMenuItemDef;
	struct SMenuDef;

	typedef uint8_t   menupos_t;
	typedef uintptr_t menuparam_t;

	typedef void (CMenuBase::*MenuFunction)(const SMenuItemDef*);

	struct SMenuItemDef
	{
	public:
		const char*  _text;
		MenuFunction _buttonpress;
		menuparam_t  _param1;
		menuparam_t  _param2;
		menuparam_t  _param3;
	public:
		FLSTR        GetText() const { return FLSTR(pgm_read_ptr(&this->_text)); }
		MenuFunction GetButtonPress() const;
		menuparam_t  GetParam1() const { return menuparam_t(pgm_read_ptr(&this->_param1)); }
		menuparam_t  GetParam2() const { return menuparam_t(pgm_read_ptr(&this->_param2)); }
		menuparam_t  GetParam3() const { return menuparam_t(pgm_read_ptr(&this->_param3)); }
	};

	struct SMenuDef
	{
	public:
		const char*         _text;
		const SMenuItemDef* _items;
		menuparam_t         _param1;
		menuparam_t         _param2;

		uint8_t GetItemCount() const
		{
			const SMenuItemDef* items = GetItems();

			for (uint8_t x = 0;; x++)
			{
				if (items[x].GetText() == nullptr) return x;
			}
		}

		uint8_t FindMenuIdx(uintptr_t param, bool (*check)(const SMenuItemDef*, uintptr_t)) const
		{
			const SMenuItemDef* item = &GetItems()[0];

			for (uint8_t x = 0; item->GetText() != nullptr; x++, item++)
			{
				if (check(item, param)) return x;
			}

			return 255;
		}

	public:
		FLSTR               GetText() const { return FLSTR(pgm_read_ptr(&this->_text)); }
		const SMenuItemDef* GetItems() const { return static_cast<const SMenuItemDef*>(pgm_read_ptr(&this->_items)); }
		menuparam_t         GetParam1() const { return menuparam_t(pgm_read_ptr(&this->_param1)); }
		menuparam_t         GetParam2() const { return menuparam_t(pgm_read_ptr(&this->_param2)); }
	};

public:

	menupos_t GetMenuItemCount() const;
	FLSTR     GetItemText(menupos_t idx) const;
	FLSTR     GetText() const;

	CMenuNavigator& GetNavigator() { return _menuNavigator; }

	void SetMainMenu() { SetMenu(_main); }

	bool         Select(menupos_t idx);
	virtual void Changed() =0;

	void OKBeep() { return CLcd::GetInstance()->OKBeep(); };
	void ErrorBeep() { return CLcd::GetInstance()->ErrorBeep(); };

	const SMenuDef* GetMenuDef() const { return _current; }
	const SMenuDef* GetMainMenuDef() const { return _main; }

protected:

	//////////////////////////////////////////

	bool PostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, FLSTR cmd, Stream* output = nullptr) { return CLcd::GetInstance()->PostCommand(syntaxtype, cmd, output); }
	bool PostCommand(char*                        cmd, Stream*      output              = nullptr) { return CLcd::GetInstance()->PostCommand(cmd, output); }

	uint8_t InitPostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, char* cmd) { return CLcd::GetInstance()->InitPostCommand(syntaxtype, cmd); }

	//////////////////////////////////////////

	const SMenuDef* _main = nullptr;

private:

	CMenuNavigator  _menuNavigator;
	const SMenuDef* _current;

public:

	void    MenuButtonPressSetMenu(const SMenuItemDef* ); // param1 => new menu, param2 != 0 => SetPosition, MenuFunction must be MenuButtonPressSetMenu & Menu = param2
	void    MenuButtonPressMenuBack(const SMenuItemDef*); // param1 => new menu, find and set position to "this" menu in new menu
	uint8_t FindMenuIndexBack();                          // find MenuBack in this(current) menu

	void MenuButtonPressSetCommand(const SMenuItemDef* def); // param1 => const char* to command

	void SetMenu(const SMenuDef* pMenu)
	{
		_current = pMenu;
		_menuNavigator.Clear();
	};

	////////////////////////////////////////////////////////

	void MenuButtonPressHomeA(axis_t                 axis);
	void MenuButtonPressHome(const SMenuItemDef*     ); // param1 : axis
	void MenuButtonPressProbe(axis_t                 axis);
	void MenuButtonPressProbe(const SMenuItemDef*    ); // param1 : axis
	void MenuButtonPressSpindle(const SMenuItemDef*  );
	void MenuButtonPressCoolant(const SMenuItemDef*  );
	void MenuButtonPressMoveG92(const SMenuItemDef*  );
	void MenuButtonPressMove(const SMenuItemDef*     ); // param1 : enum EMoveType, TODO=>String
	void MenuButtonPressRotate(const SMenuItemDef*   );
	void MenuButtonPressResurrect(const SMenuItemDef*);
	void MenuButtonPressHold(const SMenuItemDef*     );
	void MenuButtonPressResume(const SMenuItemDef*   );


	enum EMoveType
	{
		MoveP100,
		MoveP10,
		MoveP1,
		MoveP01,
		MoveP001,
		MoveM100,
		MoveM10,
		MoveM1,
		MoveM01,
		MoveM001,
		MoveHome
	};

	enum ERotateType
	{
		RotateClear,
		RotateOffset,
		RotateSetYZ,
		RotateSetX
	};
};

////////////////////////////////////////////////////////
