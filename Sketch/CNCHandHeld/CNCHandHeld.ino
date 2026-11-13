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

////////////////////////////////////////////////////////

#include <StepperLib.h>
#include <CNCLib.h>

#include <ReadPinIOControl.h>
#include <PushButton.h>
#include <RotaryButton.h>

#define ROTA_PIN  2
#define ROTB_PIN  3
#define ROTARY_ACCURACY	4

#define SPEED_X1_PIN  4
#define SPEED_X10_PIN  5
#define SPEED_X100_PIN  6

#define ESTOP_PIN  7

#define AXIS_X_PIN  8
#define AXIS_Y_PIN  9
#define AXIS_Z_PIN  10
#define AXIS_A_PIN  11
#define AXIS_B_PIN  12
#define AXIS_C_PIN  13

#define LED_ACTIVE_PIN A0

#define USE_AS_GND_PIN1 A1
#define USE_AS_GND_PIN2 A2
#define USE_AS_GND_PIN3 A3
#define USE_AS_GND_PIN4 A4

#define INTERVALL 100

////////////////////////////////////////////////////////

CPushButton btnX(AXIS_X_PIN, LOW);
CPushButton btnY(AXIS_Y_PIN, LOW);
CPushButton btnZ(AXIS_Z_PIN, LOW);
CPushButton btnA(AXIS_A_PIN, LOW);
CPushButton btnB(AXIS_B_PIN, LOW);
CPushButton btnC(AXIS_C_PIN, LOW);

CPushButton* axisBtn[] = { 
  &btnX,
  &btnY,
  &btnZ,
  &btnA,
  &btnB,
  &btnC
};

////////////////////////////////////////////////////////

CPushButton btnX1(SPEED_X1_PIN, LOW);
CPushButton btnX10(SPEED_X10_PIN, LOW);
CPushButton btnX100(SPEED_X100_PIN, LOW);

CPushButton* speedBtn[] = { 
  &btnX1,
  &btnX10,
  &btnX100
};

int speedMult[] =  {1,10,100};

////////////////////////////////////////////////////////

CPushButton btnEStop(ESTOP_PIN, LOW);

////////////////////////////////////////////////////////

typedef signed int rotarypos_t;
CRotaryButton<rotarypos_t, ROTARY_ACCURACY> rotarybutton;
uint32_t rotaryEventTime = 0;

char lastAxis = (char)0;
char lastSpeedMult = 0;

mm1000_t startPos = 0;
mm1000_t lastPos = 0;

bool lastEStop=false;

int intervall = INTERVALL;      // ms

char buffer[64];
unsigned char bufferidx = 0;

const char* axisname = "XYZABC"; 

unsigned long timeNext = 0;

int gndPins[] = {USE_AS_GND_PIN1,USE_AS_GND_PIN2,USE_AS_GND_PIN3,USE_AS_GND_PIN4 };

////////////////////////////////////////////////////////
	
static void CallRotaryButtonTickISR() { CallRotaryButtonTick(); }

void CallRotaryButtonTick()
{
	// called within TimerInterrupt() or attachedInterrupt

	switch (rotarybutton.Tick())
	{
		case CRotaryButton<rotarypos_t, ROTARY_ACCURACY>::Nothing: break;
		default:
		{
			rotaryEventTime = millis();
			break;
		}
	}
}

void setup()
{
  Serial.begin(250000);

  rotarybutton.SetPin(ROTA_PIN, ROTB_PIN);
  rotarybutton.SetMinMax(INT_MIN,INT_MAX,true);

  pinMode(LED_ACTIVE_PIN,OUTPUT);

  for (uint8_t i=0;i<uint8_t(sizeof(gndPins)/sizeof(int));i++)
  {
    pinMode(gndPins[i],OUTPUT);
    digitalWrite(gndPins[i],LOW);
  }

	CHAL::attachInterruptPin(ROTA_PIN, CallRotaryButtonTickISR, CHANGE);
	CHAL::attachInterruptPin(ROTB_PIN, CallRotaryButtonTickISR, CHANGE);

  Serial.println(F(";CNCHandHeld, " __DATE__ ));
}

////////////////////////////////////////////////////////

bool SendCommand(char axis, mm1000_t pos, mm1000_t diffPos)
{
    Serial.print(F("g91 g0"));
    Serial.print(axis);
    WritePosition(diffPos);
    Serial.print(F(" g90"));

    Serial.print(F(" ; "));
    WritePosition(pos);

    Serial.println();
    return true;
}

void WritePosition(mm1000_t pos)
{
	int16_t  div = pos / 1000;
	uint16_t rem = abs(int16_t(pos % 1000));

  if (pos < 0 && div == 0)
  {
  	Serial.print('-');
  }

	Serial.print(div);

  if (rem != 0)
  {
    Serial.print('.');

    if (rem < 100)
    {
      Serial.print('0');
      if (rem < 10)
      {
        Serial.print('0');
      }
    }
  	Serial.print(rem);
  }
}

////////////////////////////////////////////////////////

void Error()
{
  Serial.println(F(";ERROR"));
  PrintVar();
}

////////////////////////////////////////////////////////

void PrintVar()
{
    Serial.print(F(";intervall="));Serial.println(intervall);
}

////////////////////////////////////////////////////////

void InCommand(char*b)
{
  char*col = strchr(b,'=');
  if (col!=NULL)
  {
    *col = 0;
    col++;

    int varvalue = atoi(col);

    if (strcmp_P(b,PSTR("intervall"))==0)     intervall = varvalue;
    else
    {
      Error();
      return;
    }
    PrintVar();
  }
  else
  {
    Error();
  }  
}

////////////////////////////////////////////////////////

char GetActiveAxis()
{
  for (uint8_t i=0;i<uint8_t(sizeof(axisBtn)/sizeof(CPushButton*));i++)
  {
    if (axisBtn[i]->IsPressed())
    {
      return axisname[i];
    }
  }
  return (char) 0;
}

////////////////////////////////////////////////////////

int GetSpeedMult()
{
  for (uint8_t i=0;i<sizeof(speedBtn)/sizeof(CPushButton*);i++)
  {
    if (speedBtn[i]->IsPressed())
    {
      return speedMult[i];
    }
  }
  return 0;
}

////////////////////////////////////////////////////////

void loop()
{
  if (Serial.available() > 0)
  {
    char ch = buffer[bufferidx] = Serial.read();

    if (ch == '\n')
    {
      buffer[bufferidx] = 0;
      InCommand(buffer);
      bufferidx = 0;
    }
    else
    {
      bufferidx++;
      if (bufferidx >= sizeof(buffer))
      {
        Serial.println(F("buffer overrun"));
      }
    }
  }

  bool eStop = !btnEStop.IsPressed();

  if (eStop)
  {
    Serial.println(F("!"));
    lastEStop = eStop;
    delay(500);
    return;
  }

  if (lastEStop)
  {
    Serial.println(F("; EStop released"));
    lastEStop = eStop;
  }

  char axis = GetActiveAxis();
  int speedMult = GetSpeedMult();
/*
  Serial.print(rotarybutton.GetPos());
  Serial.print(' ');
  Serial.print(axis);
  Serial.print(' ');
  Serial.println(speedMult);
  delay(10);
*/
  digitalWrite(LED_ACTIVE_PIN, speedMult!=0);

  if (axis != lastAxis || speedMult != lastSpeedMult)
  {
    if (axis == 0 && speedMult == 0)
    {
      Serial.print(F("; off"));
      Serial.println();
    }
    else if (axis != 0 && speedMult != 0)
    {
      Serial.print(F("; on "));
      Serial.print(axis);
      Serial.print(F(" "));
      Serial.print(speedMult);
      Serial.println();
    }

    lastAxis = axis;
    lastSpeedMult = speedMult;
    startPos = 0;
    lastPos = 0;
    rotarybutton.SetPos(0);
    timeNext = millis() - 1;
  }

  if (axis != 0 && speedMult != 0 && millis() > timeNext)
  {
    mm1000_t pos = ((mm1000_t) rotarybutton.GetPos()) * 10 * speedMult;
    mm1000_t diffPos = lastPos - pos;

    if (diffPos != 0)
    {
      lastPos = pos;
    
      if (SendCommand(axis,lastPos,diffPos))
      {
          timeNext = millis() + INTERVALL;
      }
    }
  }
}

////////////////////////////////////////////////////////
