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

#include <StepperLib.h>
#include <CNCLib.h>
#include <CNCLibEx.h>

#include <SPI.h>
#include <SD.h>

#include "MyControl.h"
#include "GCodeParser.h"

#include <U8g2lib.h>

////////////////////////////////////////////////////////

// u8g2 only with Version 2.24.3 => HARDWARETYPE_KK1000S does not work with newer version

////////////////////////////////////////////////////////////

CMyStepper Stepper;

////////////////////////////////////////////////////////////

void setup()
{
	StepperSerial.begin(USBBAUDRATE);
}

////////////////////////////////////////////////////////////

void loop()
{
	Control.Run();
}

////////////////////////////////////////////////////////////
