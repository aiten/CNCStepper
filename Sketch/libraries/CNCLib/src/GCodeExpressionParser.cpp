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
*/
////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "CNCLib.h"
#include "MotionControlBase.h"

#include "GCodeExpressionParser.h"

////////////////////////////////////////////////////////////
//
// Expression Parser

void CGCodeExpressionParser::ReadIdent()
{
	// read variable name of gcode : #1 or #<_x>

	char ch = _reader->GetChar();
	if (ch == '#')
	{
		// start of GCODE variable => format #1 or #<_x>
		_reader->GetNextChar();
		_state._number = _gcodeParser->ParseParamNo();

		if (_gcodeParser->IsError())
		{
			Error(_gcodeParser->GetError());
			return;
		}
	}
	else
	{
		super::ReadIdent();
	}
}

////////////////////////////////////////////////////////////

void CGCodeExpressionParser::ScanNextToken()
{
	char ch = _reader->GetChar();
	while (ch)
	{
		if (ch == ';' || ch == '(') // comment
		{
			ch = _gcodeParser->SkipSpacesOrComment();
		}
		else
		{
			break;
		}
	}
	if (ch == '\0')
	{
		_state._detailToken = EndOfLineSy;
		return;
	}
	super::ScanNextToken();
}

////////////////////////////////////////////////////////////

bool CGCodeExpressionParser::EvalVariable(const char* var_name, expr_t& answer)
{
	if (var_name[0] == '#')
	{
		// assigned in ReadIdent
		answer = CMm1000::ConvertTo(_gcodeParser->GetParamValue(param_t(_state._number), false));
		return true;
	}
	return super::EvalVariable(var_name, answer);
}
