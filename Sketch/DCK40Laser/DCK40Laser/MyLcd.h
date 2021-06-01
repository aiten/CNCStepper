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

////////////////////////////////////////////////////////

#include "Configuration.h"

////////////////////////////////////////////////////////

#include <U8gLcd.h>
#include "MyMenu.h"

////////////////////////////////////////////////////////

class CMyLcd : public CU8GLcd
{
private:

	typedef CU8GLcd super;

public:

	virtual void Init() override;
	virtual void Beep(const SPlayTone*, bool) override;

protected:

	virtual class U8G2&    GetU8G() override;
	virtual class CMenu3D& GetMenu() override { return _menu; }

	virtual bool DrawLoopDefault(EnumAsByte(EDrawLoopType) type, uintptr_t data) override;

private:

	CMyMenu _menu;
};

////////////////////////////////////////////////////////

extern CMyLcd Lcd;
