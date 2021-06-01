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
