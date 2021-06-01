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

#pragma once

////////////////////////////////////////////////////////

#include <GCodeParser.h>
#include <SPI.h>
#include <SD.h>
#include "ConfigurationCNCLibEx.h"

////////////////////////////////////////////////////////
//
// GCode Parser for 3d printer extensions
//
class CGCode3DParser : public CGCodeParser
{
private:

	typedef CGCodeParser super;

public:

	CGCode3DParser(CStreamReader* reader, Stream* output) : super(reader, output) { }

	static File& GetExecutingFile() { return _state._file; }

	static uint32_t    GetExecutingFilePosition() { return _state._printFilePos; }
	static uint32_t    GetExecutingFileLine() { return _state._printFileLine; }
	static uint32_t    GetExecutingFileSize() { return _state._printFileSize; }
	static const char* GetExecutingFileName() { return _state._printFileName; }

	static void SetExecutingFilePosition(uint32_t pos) { _state._printFilePos   = pos; }
	static void SetExecutingFileLine(uint32_t     line) { _state._printFileLine = line; }
	static void SetExecutingFileName(char*        fileName) { strcpy(_state._printFileName, fileName); }

	static void Init()
	{
		super::Init();
		_state.Init();
	}

protected:

	virtual bool InitParse() override;
	virtual bool MCommand(mcode_t mcode) override;

private:

	////////////////////////////////////////////////////////
	// global State

	struct GCodeState
	{
		uint32_t _printFilePos;
		uint32_t _printFileLine;
		uint32_t _printFileSize;
		File     _file;

		bool _isM28; // SD write mode
		char _printFileName[MAXFILEEXTNAME + 1];

		void Init()
		{
			_printFileSize    = 0;
			_printFilePos     = 0;
			_printFileLine    = 0;
			_isM28            = false;
			_printFileName[0] = 0;
		}
	};

	static struct GCodeState _state;

	////////////////////////////////////////////////////////

	void M20Command(); // List content of SD card
	void M21Command(); // Init SD card
	void M22Command(); // Release SD card
	void M23Command(); // Select file on SD card
	void M24Command(); // Start file on SD card
	void M25Command();
	void M26Command(); // Set file position on SD card
	void M27Command(); // Status of SD print - position/size 
	void M28Command(); // Start write to SD file
	void M29Command(); // Stop write to SD file
	void M30Command(); // Delete file on SD

	bool GetPathName(char*  buffer);
	bool GetFileName(char*& buffer, uint8_t& pathLength);

	bool AddPathChar(char ch, char*& buffer, uint8_t& pathLength);

	bool CheckSD();
	bool DeleteSDFile(char* fileName, bool errorIfNotExists);

	static void PrintSDFileListRecurse(class File& dir, uint8_t depth, uint16_t& count, char* fileNameBuffer, char separatorChar);

protected:

	static void PrintVersion();
};

////////////////////////////////////////////////////////
