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

#pragma once

#include "LCD.h"

////////////////////////////////////////////////////////

class CMenuBase
{

public:

	CMenuBase()														{ SetMenu(NULL); }

	struct SMenuItemDef;
	struct SMenuDef;

	typedef uint8_t menupos_t;
	typedef void(CMenuBase::*MenuFunction)(const SMenuItemDef*);
	typedef ptr_t menuparam_t;

	struct SMenuItemDef
	{
	public:
		const char* _text;
		MenuFunction _buttonpress;
		menuparam_t _param1;
		menuparam_t _param2;
		menuparam_t _param3;
	public:
		const __FlashStringHelper* GetText() const					{ return (const __FlashStringHelper*)pgm_read_ptr(&this->_text); }
		MenuFunction GetButtonPress()const;
		menuparam_t GetParam1()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param1); }
		menuparam_t GetParam2()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param2); }
		menuparam_t GetParam3()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param3); }
	};

	struct SMenuDef
	{
	public:
		const char* _text;
		const SMenuItemDef* _items;
		menuparam_t _param1;
		menuparam_t _param2;

		uint8_t GetItemCount() const
		{
			const SMenuItemDef* items = GetItems();
			for (uint8_t x = 0;; x++)
			{
				if (items[x].GetText() == NULL) return x;
			}
		}

		uint8_t FindMenuIdx(ptr_t param, bool(*check)(const SMenuItemDef*, ptr_t param)) const
		{
			const SMenuItemDef* item = &GetItems()[0];
			for (uint8_t x = 0; item->GetText() != NULL; x++, item++)
			{
				if (check(item, param)) return x;
			}

			return 0;
		}

	public:
		const __FlashStringHelper* GetText() const					{ return (const __FlashStringHelper*)pgm_read_ptr(&this->_text); }
		const SMenuItemDef* GetItems() const						{ return (const SMenuItemDef*)pgm_read_ptr(&this->_items); }
		menuparam_t GetParam1()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param1); }
		menuparam_t GetParam2()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param2); }
	};

	static char* AddAxisName(char*buffer, axis_t axis);

public:

	menupos_t GetPosition()											{ return _position; }
	void SetPosition(menupos_t position)							{ _position = position; }

	menupos_t GetOffset()											{ return _offset; }

	void AdjustOffset(menupos_t firstline, menupos_t lastline);
	uint8_t ToPrintLine(menupos_t firstline, menupos_t lastline, menupos_t i);		// return 255 if not to print

	const SMenuDef*GetMenuDef()										{ return _current; }
	const SMenuDef*GetMainMenuDef()									{ return _main; }

	void SetMainMenu()												{ SetMenu(_main); }

	bool Select();
	virtual void Changed()=0;

	void OKBeep()													{ return CLcd::GetInstance()->OKBeep(); };
	void ErrorBeep()												{ return CLcd::GetInstance()->ErrorBeep(); };

protected:

	//////////////////////////////////////////

	bool PostCommand(const __FlashStringHelper* cmd, Stream* output=NULL)	{ return CLcd::GetInstance()->PostCommand(cmd,output); }
	bool PostCommand(char* cmd, Stream* output=NULL)						{ return CLcd::GetInstance()->PostCommand(cmd,output); }

	//////////////////////////////////////////

	void SetOffset(menupos_t offset)								{ _offset = offset; }
	void AddOffset(menupos_t offset)								{ _offset += offset; }
	void SubOffset(menupos_t offset)								{ _offset -= offset; }

	const SMenuDef*		_main=NULL;

private:

	menupos_t			_position;									// current selected menu
	menupos_t			_offset;									// start of menuitem to draw  
	const SMenuDef*		_current;

public:

	void MenuButtonPressSetMenu(const SMenuItemDef*);				// param1 => new menu, param2 != 0 => SetPosition, MenuFunction must be MenuButtonPressSetMenu & Menu = param2
	void MenuButtonPressMenuBack(const SMenuItemDef*);				// param1 => new menu, find and set position to "this" menu in new menu

	void MenuButtonPressSetCommand(const SMenuItemDef*def);			// param1 => const char* to command

	void SetMenu(const SMenuDef* pMenu)								{ _current = pMenu; _position = 0; _offset = 0; };

	////////////////////////////////////////////////////////

	void MenuButtonPressHomeA(axis_t axis);
	void MenuButtonPressHome(const SMenuItemDef*);					// param1 : axis
	void MenuButtonPressProbe(const SMenuItemDef*);					// param1 : axis
	void MenuButtonPressSpindle(const SMenuItemDef*);
	void MenuButtonPressCoolant(const SMenuItemDef*);
	void MenuButtonPressMoveG92(const SMenuItemDef*);
	void MenuButtonPressMove(const SMenuItemDef*);					// param1 : enum EMoveType, TODO=>String
	void MenuButtonPressRotate(const SMenuItemDef*);
	void MenuButtonPressResurrect(const SMenuItemDef*);
	void MenuButtonPressHold(const SMenuItemDef*);
	void MenuButtonPressResume(const SMenuItemDef*);

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
