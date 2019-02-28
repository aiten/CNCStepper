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

#pragma once

typedef uint8_t u8g_fntpgm_uint8_t;

#define clrscr() system("cls")

class U8GLIBCommon
{
	char _buffer[4048];
	char _tmp[1024];

public:
	U8GLIBCommon() = default;

	void begin() {};
	void firstPage() { _buffer[0] = 0; };

	bool nextPage() const
	{
		char fileName[_MAX_PATH];
		::GetTempPathA(_MAX_PATH, fileName);
		strcat_s(fileName, "\\CNCLib_LCD.txt");

		FILE* fout;
		fopen_s(&fout, fileName, "wt");
		if (fout)
		{
			fwrite(_buffer, strlen(_buffer), 1, fout);
			fclose(fout);
		}

		return false;
	}

	void drawStr(int x, int y, const char* text)
	{
		setPrintPos(x, y);
		print(text);
	}

	void setFont(const u8g_fntpgm_uint8_t*) { };

	void setPrintPos(int x, int y)
	{
		sprintf_s(_tmp, "SP:%i:%i\n", x, y);
		strcat_s(_buffer, _tmp);
	};

	void print(const char ch)
	{
		sprintf_s(_tmp, "P:%c\n", ch);
		strcat_s(_buffer, _tmp);
	};

	void print(const char* text)
	{
		sprintf_s(_tmp, "P:%s\n", text);
		strcat_s(_buffer, _tmp);
	};

	void println(const char* text)
	{
		sprintf_s(_tmp, "PL:%s\n", text);
		strcat_s(_buffer, _tmp);
	};
};
