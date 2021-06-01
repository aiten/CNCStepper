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

#include "ConfigurationCNCLib.h"

////////////////////////////////////////////////////////

typedef uint16_t toolnr_t;
#define NOTOOLINDEX 255

////////////////////////////////////////////////////////
// 
// Tools library
//
class CGCodeTools : public CSingleton<CGCodeTools>
{
public:

	enum EToolType
	{
		EndMill,
		BullNose,
		BallNose,
		Vcutter,
		Drill,
		Lathe
	};

	struct STools
	{
		toolnr_t              ToolNr;
		EnumAsByte(EToolType) ToolType;
		mm1000_t              Radius;
		mm1000_t              Height;
	};

	bool IsValidTool(toolnr_t tool) { return GetToolIndex(tool) != NOTOOLINDEX; }

	mm1000_t GetRadius(toolnr_t tool);
	mm1000_t GetHeight(toolnr_t tool);

private:

	uint8_t GetToolIndex(toolnr_t tool);

	static const STools _tools[] PROGMEM;
};
