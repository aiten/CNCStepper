////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
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
