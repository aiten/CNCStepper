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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

#include <CNCLib.h>
#include <CNCLibEx.h>

#include "GCode3DParser.h"
#include "Control3D.h"

////////////////////////////////////////////////////////////

void CControl3D::Init()
{
	super::Init();
	CGCode3DParser::Init();
	ClearPrintFromSD();
}

////////////////////////////////////////////////////////////

void CControl3D::Initialized()
{
	super::Initialized();
	char tmp[8 + 3 + 1];
	strcpy_P(tmp, PSTR("startup.nc"));

	CGCode3DParser::GetExecutingFile() = SD.open(tmp, FILE_READ);

	if (CGCode3DParser::GetExecutingFile())
	{
		StepperSerial.println(MESSAGE_CONTROL3D_ExecutingStartupNc);
		StartPrintFromSD();
	}
	else
	{
		StepperSerial.println(MESSAGE_CONTROL3D_NoStartupNcFoundOnSD);
	}
}

////////////////////////////////////////////////////////////

void CControl3D::InitSD(pin_t sdEnablePin)
{
	StepperSerial.print(MESSAGE_CONTROL3D_InitializingSDCard);

	ClearPrintFromSD();

	CHAL::pinModeOutput(sdEnablePin);
	CHAL::digitalWrite(sdEnablePin, HIGH);

	_sdEnablePin = sdEnablePin;
	ReInitSD();
}

////////////////////////////////////////////////////////////

void CControl3D::ReInitSD()
{
	if (!SD.begin(_sdEnablePin))
	{
		StepperSerial.println(MESSAGE_CONTROL3D_initializationFailed);
	}
	else
	{
		StepperSerial.println(MESSAGE_CONTROL3D_initializationDone);
	}
}

////////////////////////////////////////////////////////////

bool CControl3D::Parse(CStreamReader* reader, Stream* output)
{
	CGCode3DParser gcode(reader, output);
	return ParseAndPrintResult(&gcode, output);
}

////////////////////////////////////////////////////////////

void CControl3D::ReadAndExecuteCommand()
{
	super::ReadAndExecuteCommand();

	File file = CGCode3DParser::GetExecutingFile();
	if (PrintFromSDRunning() && file)
	{
		if (IsKilled())
		{
			ClearPrintFromSD();
			file.close();
		}
		else
		{
			FileReadAndExecuteCommand(&file, nullptr); // one line!!! Output goes to NULL

			if (file.available() == 0)
			{
				ClearPrintFromSD();
				file.close();
				StepperSerial.println(MESSAGE_CONTROL3D_ExecutingStartupNcDone);
			}
			else
			{
				CGCode3DParser::SetExecutingFilePosition(file.position());
				CGCode3DParser::SetExecutingFileLine(CGCode3DParser::GetExecutingFileLine() + 1);
			}
		}
	}
}

////////////////////////////////////////////////////////////
