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

CSerial      Serial;
CMsvcStepper Stepper;

HardwareSerial& StepperSerial = Serial;

static void setup();
static void loop();

int _tmain(int /* argc */, _TCHAR* /* argv */[])
{
	setup();
	loop();

	Stepper.EndTest("StepperOnly.csv");

	return 0;
}

// Copy From INO

void setup()
{
	Serial.begin(57600);
	Serial.println("Stepper is starting ...");

	// only drive stepper  
	Stepper.Init();
	CHAL::pinModeOutput(13);

	Stepper.SetDefaultMaxSpeed(5000, 100, 150);
	Stepper.SetDefaultMaxSpeed(32000, 800, 800);

	Stepper.SetLimitMax(0, 6950);
	Stepper.SetLimitMax(0, 65535);
	Stepper.SetLimitMax(1, 4000);
	Stepper.SetLimitMax(2, 100);

	//    int dist2 = Stepper.GetLimitSize(2);
	//    int dist0 = Stepper.GetLimitSize(0);
	//    int dist1 = Stepper.GetLimitSize(1);
	//    Stepper.MoveReference(2,-min(dist2,10000),10);
	//    Stepper.MoveReference(0,-min(dist0,10000),10);
	//    Stepper.MoveReference(1,-min(dist1,10000),10);

	Stepper.SetWaitFinishMove(false);

	Stepper.SetJerkSpeed(0, 400);
	Stepper.SetJerkSpeed(1, 400);
	Stepper.SetJerkSpeed(2, 400);
}

void loop()
{
	Stepper.SetLimitMax(0, 400000);
	Stepper.MoveRel(0, 200000, 0);
	Stepper.MoveRel(0, -200000, 0);
	return;
	/*
	  int count = 0;
	
	  Stepper.CStepper::MoveRel(0,3000,30000);count+=3000;
	  Stepper.CStepper::MoveRel(0,8000,6000);count+=8000;
	  Stepper.CStepper::MoveRel(0,15000,9000);count+=15000;
	  Stepper.CStepper::MoveRel(0,20000,15000);count+=32000;
	  Stepper.CStepper::MoveRel(0,3000,1500);count+=3000;
	  Stepper.CStepper::MoveRel(0,5500,6000);count+=5500;
	
	//  Stepper.WaitBusy();
	  Serial.print(F("TimerISRBusy="));Serial.println(Stepper.GetTimerISRBuys());
	  delay(1000);
	  Stepper.CStepper::MoveAbs(0,0,10000);
	//  Stepper.WaitBusy();
	  delay(1000);
	*/
}


void GoToReference(axis_t axis, steprate_t /* steprate */)
{
	Stepper.MoveReference(axis, Stepper.ToReferenceId(axis, true), true, Stepper.GetDefaultVmax() / 4, 0, 0, 0);
}

////////////////////////////////////////////////////////////

void GoToReference()
{
	GoToReference(Z_AXIS, 0);
	GoToReference(Y_AXIS, 0);
	GoToReference(X_AXIS, 0);
}
