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

#include "CNCLibTypes.h"

////////////////////////////////////////////////////////

class CMenuNavigator
{
public:

	typedef uint8_t menupos_t;

public:

	menupos_t GetPosition() const { return _position; }

	void SetPosition(menupos_t itemIdx, menupos_t position)
	{
		_itemIdx  = itemIdx;
		_position = position;
		_empty    = false;
	}

	void SetPosition(menupos_t itemIdx) { SetPosition(itemIdx, itemIdx); }

	menupos_t GetItemIdx() const { return _itemIdx; }

	menupos_t GetOffset() const { return _offset; }

	void    AdjustOffset(menupos_t menuEntries, menupos_t firstLine, menupos_t lastLine);
	uint8_t ToPrintLine(menupos_t  firstLine, menupos_t   lastLine, menupos_t  i);		// return 255 if not to print

	uintptr_t GetParam() const { return _param; }
	void      SetParam(uintptr_t param) { _param = param; }

	void Clear()
	{
		_itemIdx  = 0;
		_offset   = 0;
		_position = 0;
		_empty    = true;
	}

	bool IsCleared() const { return _empty; }

protected:

	//////////////////////////////////////////

	void SetOffset(menupos_t offset) { _offset = offset; }
	void AddOffset(menupos_t offset) { _offset += offset; }
	void SubOffset(menupos_t offset) { _offset -= offset; }

private:

	menupos_t _itemIdx;									// current "itemIdx" in MenuDef
	menupos_t _position;								// current selected menu
	menupos_t _offset;									// start of menuitem to draw 
	bool      _empty;									// empty - => cleared

	uintptr_t _param;									// "user" value of current position  
};

////////////////////////////////////////////////////////
