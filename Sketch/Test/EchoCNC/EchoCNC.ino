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

char _buffer[128];
uint8_t _bufferidx;

void setup()
{
  Serial.begin(250000);
  _bufferidx = 0;
}

bool IsEndOfCommandChar(char ch)
{
  return ch == '\n' || ch == (char) - 1;
}

void Command(char* buffer)
{
  Serial.println(buffer);
  Serial.println("ok");
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
