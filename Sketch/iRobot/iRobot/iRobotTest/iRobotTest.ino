
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

#include <Adafruit_TiCoServo.h>
#define Servo Adafruit_TiCoServo

#define NUM_AXIS 4
#define DEFAULT_PULSE_WIDTH 1500
  
Servo _servo[NUM_AXIS];

////////////////////////////////////////////////////////////

unsigned int pos=1300;
#define STEPPERRANGE 1800							// MAX_PULSE_WIDTH - MIN_PULSE_WIDTH
#define CENTERPOSOPPSET ((2000-STEPPERRANGE)/2)		

void setup()
{
  Serial.begin(115200);
	_servo[0].attach(5);                    // do not change, see Adafruit_TiCoServo for available pins
	_servo[1].attach(6);
	_servo[2].attach(7);
	_servo[3].attach(8);

	for (unsigned char i = 0; i<NUM_AXIS; i++)
	{
  	  _servo[i].write(pos);
	} 
}

////////////////////////////////////////////////////////////

//int diff = 500;
int diff = 0;

void loop()
{
  _servo[0].write(pos);
   delay(5000);

   pos += diff;
   if (pos > 1700 || pos < 1000) 
   {
     diff = -diff;
     pos += diff;
     pos += diff;
   }
}

////////////////////////////////////////////////////////////
  
