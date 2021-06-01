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

#include "stdafx.h"
#include <math.h>
#include "..\MsvcStepper\MsvcStepper.h"
#include "TestTools.h"
#include "..\..\..\sketch\HARoboter\Global.h"

CSerial Serial;

static void setup();
static void loop();
static void Idle();

int _tmain(int argc, _TCHAR* argv[])
{
	setup();

	while (true)
	{
		loop();
	}
}

CMyCommand Command;
CMsvcStepper MyStepper;
class CStepper& Stepper = MyStepper;
CControl Control;
SSettings Settings;
//CPlotter Plotter;

void setup() 
{     
  Serial.begin(57600);        
  Serial.println("MyStepper is starting ...");

  // only drive stepper  
  Stepper.Init();
  pinMode(13, OUTPUT);     

  Stepper.SetDefaultMaxSpeed(5000, 100 , 150);
  Stepper.SetLimitMax(0,6950);
  Stepper.SetLimitMax(1,4000);
  Stepper.SetLimitMax(2,100);

  MyStepper.InitTest();
  Serial.pIdle = Idle;
//	MyStepper._logISR = true;

}

void loop() 
{
  Control.Run();
}

void drawloop()
{

}

static void Idle()
{
	if (MyStepper.IsBusy())
		MyStepper.DoISR();
}
