////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

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
