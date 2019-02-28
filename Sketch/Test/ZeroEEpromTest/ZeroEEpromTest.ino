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

void PrintHex(uint32_t val)
{
  if (val < 16)
  {
    Serial.print((int) 0);
   }
    Serial.print(val,HEX);
    Serial.print(':');
}

void setup()
{
  Serial.begin(250000);
  Serial.println(F("Start ZeroEEPromTest"));

  CHAL::InitEeprom();

  if (!CHAL::HaveEeprom())
  {
    Serial.println(F("EEprom is NOT available"));
    return;
  }

   Serial.println(F("Setup done"));
   Serial.println(F("(i)nit, (r)ead, (s)et, (w)rite, (x)reset"));
}

static char _buffer[128];
static uint8_t _bufferidx=0;

bool IsEndOfCommandChar(char ch)
{
  return ch == '\r' || ch == '\n' || ch == (char) - 1;
}

void CommandRead()
{
  Serial.println("Eeprom: ");
  for (int i=0;i<16;i++)
  {
    uint32_t val = CHAL::eeprom_read_dword(CHAL::GetEepromBaseAdr()+i);
   
    PrintHex(val%256);
    PrintHex((val>>8)%256);
    PrintHex((val>>16)%256);
    PrintHex((val>>24)%256);
  }
  Serial.println();
}

void CommandInit()
{
    CHAL::InitEeprom();
    Serial.println("init done");
}

void CommandSet()
{
  for (int i=0;i<16;i++)
  {
    CHAL::eeprom_write_dword(CHAL::GetEepromBaseAdr()+i,CHAL::eeprom_read_dword(CHAL::GetEepromBaseAdr()+i)+i);
  }
  CommandRead();
}

void CommandWrite()
{
    CHAL::FlushEeprom();
}

void Command(char* buffer)
{
  if (buffer[0])
  {
    if (strcmp(buffer,"read") == 0 || strcmp(buffer,"r") == 0)
    {
      CommandRead();
    }
    else if ((strcmp(buffer,"init") == 0 || strcmp(buffer,"i") == 0))
    {
      CommandInit();
    }
    else if ((strcmp(buffer,"set") == 0 || strcmp(buffer,"s") == 0))
    {
      CommandSet();
    }
    else if ((strcmp(buffer,"write") == 0 || strcmp(buffer,"w") == 0))
    {
      CommandWrite();
    }
    else if ((strcmp(buffer,"reset") == 0 || strcmp(buffer,"x") == 0))
    {
      NVIC_SystemReset();
    }
    else
    {
        Serial.println(buffer);
        Serial.println("?");
    }
  }
}

void loop()
{
  if (Serial.available() > 0)
  {
    while (Serial.available() > 0)
    {
      char ch = _buffer[_bufferidx] = Serial.read();

      if (IsEndOfCommandChar(ch))
      {
        _buffer[_bufferidx] = 0;      // remove from buffer
        Command(_buffer);
        _bufferidx = 0;

        return;
      }

      _bufferidx++;
      if (_bufferidx >= sizeof(_buffer))
      {
        Serial.println("MESSAGE_CONTROL_FLUSHBUFFER");
        _bufferidx = 0;
      }
    }
  }
}

