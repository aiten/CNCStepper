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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Control3D.h"
#include "GCode3DParser.h"
#include "Menu3D.h"
#include "SDDirReader.h"

///////////////////////////////////////////////////////////

void CMenu3D::MenuButtonPressSDStart(const CMenuBase::SMenuItemDef*)
{
	bool isOK = false;
	if (CGCode3DParser::GetExecutingFileName()[0])
	{
		PostCommand(EGCodeSyntaxType::GCode, F("m21")); // Init SD

		char printfileName[MAXFILEEXTNAME + 1 + 10];
		InitPostCommand(EGCodeSyntaxType::GCode, printfileName);
		strcat_P(printfileName, PSTR("m23 "));
		strcat(printfileName, CGCode3DParser::GetExecutingFileName());

		if (PostCommand(printfileName))
		{
			PostCommand(EGCodeSyntaxType::GCode, F("m24"));
			OKBeep();
			isOK = true;
		}
	}
	if (!isOK)
	{
		ErrorBeep();
	}
}

////////////////////////////////////////////////////////////

void CMenu3D::MenuButtonPressSDSelect(const CMenuBase::SMenuItemDef*)
{
	CSDDirReader dirreader([](File* file) -> bool { return file->isDirectory(); });
	auto         currentFileIdx = uint8_t(GetNavigator().GetParam());

	for (uint8_t fileidx = 0; dirreader.MoveNext(); fileidx++)
	{
		if (fileidx == currentFileIdx)
		{
			CGCode3DParser::SetExecutingFileName(dirreader.Current.name());
			MenuButtonPressMenuBack(&GetMenuDef()->GetItems()[FindMenuIndexBack()]);
			break;
		}
	}
}
