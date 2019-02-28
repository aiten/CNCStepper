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

#include "ConfigurationCNCLib.h"
#include "Parser.h"

////////////////////////////////////////////////////////
// Parser for NOT G-Code and HTML 
// use this parser for testing

class CHelpParser : public CParser
{
public:

	CHelpParser(CStreamReader* reader, Stream* output) : CParser(reader, output) { }

protected:

	virtual void Parse() override;

	bool MoveRel();
	bool MoveRel(axis_t axis);
	bool MoveAbs();
	bool MoveAbs(axis_t axis);

	bool SetPosition(axis_t axis);

	bool MyGoToReference(axis_t axis);
	bool SetSpeed();

	bool CheckEOC();
};

////////////////////////////////////////////////////////
