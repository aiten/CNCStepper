////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////

unsigned int background_count = 0;

void HandleBackGround()
{
  Serial.println(F("in bkg"));

  background_count++;

  for (uint32_t i=0; i < 10000000;i++)
  {
    // wait
  }
  Serial.println(F("out bkg"));
}

void setup()
{
  Serial.begin(250000);
  Serial.println(F("Start Background on SAM"));

  CHAL::InitBackground(HandleBackGround);

  Serial.println(F("Setup done"));
}

void loop()
{
  static unsigned int mybackground_count = 0;
  static long starttime = millis();

  // dummy
  delay(250);
  CHAL::BackgroundRequest();

  static int dotcount = 0;

  if (mybackground_count != background_count)
  {
    if (dotcount > 0)
      Serial.println();
    Serial.print("Timer=");
    mybackground_count = background_count;
    Serial.print(mybackground_count);
    Serial.print("(");
    Serial.print((millis() - starttime) / 1000.0 / mybackground_count, 6);
    Serial.println(")");
  }
  else
  {
    dotcount++;
    Serial.print('+');
    if (dotcount > 40)
    { 
      dotcount = 0;
      Serial.println();
    }
  }
}

