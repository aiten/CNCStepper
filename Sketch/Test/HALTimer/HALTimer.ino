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

unsigned int timer1value = 48000;
unsigned int timer2value = 4;

unsigned int irq_countTimer0 = 0;
unsigned int irq_countTimer1 = 0;
uint32_t irq_countTimer2 = 0;

void HandleInterruptTimer0()
{
  irq_countTimer0++;
}

void HandleInterruptTimer1()
{
  irq_countTimer1++;
  CHAL::StartTimer1OneShot(timer1value);
}

void HandleInterruptTimer2()
{
  irq_countTimer2++;
  CHAL::ReStartTimer2OneShot(timer2value);
}

void setup()
{
  Serial.begin(250000);
  Serial.println(F("Start HAL Timer Test"));

  CHAL::InitTimer0(HandleInterruptTimer0);
  CHAL::InitTimer1OneShot(HandleInterruptTimer1);
  CHAL::InitTimer2OneShot(HandleInterruptTimer2);

  CHAL::StartTimer0(10000);

  {
    CCriticalRegion crit;
    // do not wait until finished
    CHAL::StartTimer1OneShot(timer1value);
  }

  {
      CCriticalRegion crit;
      CHAL::StartTimer2OneShot(timer2value);
  }

  Serial.println(F("Setup done"));
}
void loop()
{
  static unsigned int myirq_countTimer0 = 0;
  static unsigned int myirq_countTimer1 = 0;
  static uint32_t myirq_countTimer2 = 0;
  static int32_t starttime0 = millis();
  static int32_t starttime1 = millis();
  static int32_t starttime2 = millis();

  // dummy
  delay(333);

  static int dotcount = 0;

  if (myirq_countTimer0 != irq_countTimer0 || myirq_countTimer1 != irq_countTimer1 || myirq_countTimer2 != irq_countTimer2)
  {
    if (dotcount > 0)
      Serial.println();
    if (myirq_countTimer0 != irq_countTimer0)
    {
      Serial.print("Timer0=");
      myirq_countTimer0 = irq_countTimer0;
      Serial.print(myirq_countTimer0);
      Serial.print("(");
      Serial.print((millis() - starttime0) / 1000.0 / myirq_countTimer0, 6);
      Serial.print(")");
    }
    if (myirq_countTimer1 != irq_countTimer1)
    {
      Serial.print("Timer1=");
      myirq_countTimer1 = irq_countTimer1;
      Serial.print(myirq_countTimer1);
      Serial.print("(");
      Serial.print((millis() - starttime1) / 1000.0 / myirq_countTimer1, 6);
      Serial.print(")");
    }
    if (myirq_countTimer2 != irq_countTimer2)
    {
      Serial.print("Timer2=");
      myirq_countTimer2 = irq_countTimer2;
      Serial.print(myirq_countTimer2);
      Serial.print("(");
      Serial.print((millis() - starttime2) / 1000.0 / myirq_countTimer2, 6);
      Serial.print(")");
    }
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

