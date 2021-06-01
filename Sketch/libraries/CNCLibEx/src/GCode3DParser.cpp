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

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <CNCLib.h>
#include <CNCLibEx.h>

#include <MotionControl.h>

#include "GCode3DParser.h"
#include "MessageCNCLibEx.h"
#include <Control3D.h>

////////////////////////////////////////////////////////////

struct CGCode3DParser::GCodeState CGCode3DParser::_state;

////////////////////////////////////////////////////////////

bool CGCode3DParser::InitParse()
{
	if (!super::InitParse())
	{
		return false;
	}

	if (_state._isM28)
	{
		const char* lineStart = _reader->GetBuffer();

		// ignore lineNumbers
		if (_reader->SkipSpacesToUpper() == 'N')
		{
			if (IsUInt(_reader->GetNextChar()))
			{
				GetInt32();
				_reader->SkipSpaces();
			}
		}

		// m28 writes all subsequent commands to the sd file
		// m29 ends the writing => we have to check first
		if (!TryToken(F("M29"), false, true))
		{
			GetExecutingFile().println(lineStart);
			_reader->MoveToEnd();
			return false;
		}
		_reader->ResetBuffer(lineStart);
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CGCode3DParser::MCommand(mcode_t mcode)
{
	if (super::MCommand(mcode))
	{
		return true;
	}

	switch (mcode)
	{
		// @formatter:off — disable formatter after this line
		case 20: M20Command();	return true;
		case 21: M21Command();	return true;
		case 22: M22Command();	return true;
		case 23: M23Command();	return true;
		case 24: M24Command();	return true;
		case 25: M25Command();	return true;
		case 26: M26Command();	return true;
		case 27: M27Command();	return true;
		case 28: M28Command();	return true;
		case 29: M29Command();	return true;
		case 30: M30Command();	return true;
		case 115: _OkMessage = PrintVersion; return true;
		default: break;
			// @formatter:on — enable formatter after this line
	}

	return false;
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M20Command()
{
	char fileNameBuffer[MAXPATHNAME];
	strcpy(fileNameBuffer, "/");

	File     root  = SD.open(fileNameBuffer);
	uint16_t count = 0;

	if (root)
	{
		root.rewindDirectory();
		StepperSerial.println(MESSAGE_PARSER3D_BEGIN_FILE_LIST);
		PrintSDFileListRecurse(root, 0, count, fileNameBuffer, '\n');
		if (count > 0)
		{
			StepperSerial.println();
		}
		StepperSerial.println(MESSAGE_PARSER3D_END_FILE_LIST);
	}
	root.close();
}

////////////////////////////////////////////////////////////

void CGCode3DParser::PrintSDFileListRecurse(File& dir, uint8_t depth, uint16_t& count, char* fileNameBuffer, char separatorChar)
{
#ifdef _MSC_VER
#pragma warning (suppress: 4127)
#endif
	while (true)
	{
		File entry = dir.openNextFile();
		if (!entry)
		{
			break;
		}

		if (entry.isDirectory())
		{
			unsigned int lastIdx = strlen(fileNameBuffer);
			strcat(fileNameBuffer, entry.name());
			strcat_P(fileNameBuffer, MESSAGE_PARSER3D_SLASH);
			PrintSDFileListRecurse(entry, depth + 1, count, fileNameBuffer, separatorChar);
			fileNameBuffer[lastIdx] = 0;
		}
		else
		{
			if (entry.name()[9] != '~')
			{
				if (count != 0)
				{
					StepperSerial.print(separatorChar);
				}
				StepperSerial.print(fileNameBuffer);
				StepperSerial.print(entry.name());
				count++;
			}
		}
		entry.close();
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M21Command()
{
	static_cast<CControl3D*>(CControl::GetInstance())->ReInitSD();
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M22Command() {}

////////////////////////////////////////////////////////////

void CGCode3DParser::M23Command()
{
	char fileName[MAXPATHNAME];
	if (!CheckSD() || !GetPathName(fileName))
	{
		return;
	}

	GetExecutingFile() = SD.open(fileName, FILE_READ);
	if (!GetExecutingFile())
	{
		Error(MESSAGE_PARSER3D_ERROR_READING_FILE);
		return;
	}

	strcpy(_state._printFileName, fileName); //8.3
	_state._printFilePos  = 0;
	_state._printFileLine = 1;
	_state._printFileSize = GetExecutingFile().size();

	StepperSerial.print(MESSAGE_PARSER3D_FILE_OPENED);
	StepperSerial.print(fileName);
	StepperSerial.print(MESSAGE_PARSER3D_SIZE);
	StepperSerial.println(_state._printFileSize);

	StepperSerial.println(MESSAGE_PARSER3D_FILE_SELECTED);
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M24Command()
{
	if (GetExecutingFile())
	{
		CControl::GetInstance()->StartPrintFromSD();
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M25Command() {}

////////////////////////////////////////////////////////////

void CGCode3DParser::M26Command()
{
	// set sd pos

	if (!GetExecutingFile() || CControl::GetInstance()->PrintFromSDRunning())
	{
		Error(MESSAGE_PARSER3D_NO_FILE_SELECTED);
		return;
	}

	if (_reader->SkipSpacesToUpper() == 'S')
	{
		_reader->GetNextChar();
		_state._printFilePos  = GetUInt32();
		_state._printFileLine = 1; // TO DO => count line 
		if (IsError())
		{
			return;
		}

		GetExecutingFile().seek(_state._printFilePos);
	}
	else if (_reader->GetCharToUpper() == 'L')
	{
		_reader->GetNextChar();
		uint32_t lineNr = GetUInt32();
		if (IsError())
		{
			return;
		}

		if (lineNr < 1)
		{
			Error(MESSAGE_PARSER3D_LINE_SEEK_ERROR);
			return;
		}

		GetExecutingFile().seek(0);

		for (uint32_t line = 1; line < lineNr; line++)
		{
			// read line until \n
			char ch;
			do
			{
				if (GetExecutingFile().available() == 0)
				{
					Error(MESSAGE_PARSER3D_LINE_SEEK_ERROR);
					return;
				}

				ch = GetExecutingFile().read();
			}
			while (ch != '\n');
		}

		_state._printFileLine = lineNr;
		_state._printFilePos  = GetExecutingFile().position();
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M27Command()
{
	if (GetExecutingFile())
	{
		StepperSerial.print(MESSAGE_PARSER3D_SD_PRINTING_BYTE);
		StepperSerial.print(_state._printFilePos);
		StepperSerial.print(':');
		StepperSerial.print(_state._printFileSize);
		StepperSerial.print(MESSAGE_PARSER3D_SD_PRINTING_LINE);
		StepperSerial.println(_state._printFileLine);
	}
	else
	{
		StepperSerial.println(MESSAGE_PARSER3D_NOT_SD_PRINTING);
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M28Command()
{
	if (!_state._isM28)
	{
		char fileName[MAXPATHNAME];
		if (!CheckSD() || !GetPathName(fileName))
		{
			return;
		}

		// create folders
		char* lastSlash = strrchr(fileName, '/');

		if (lastSlash != nullptr && lastSlash != fileName)
		{
			*lastSlash = 0;
			if (!SD.exists(fileName))
			{
				if (!SD.mkdir(fileName))
				{
					Error(MESSAGE_PARSER3D_ERROR_CREATING_FILE);
					return;
				}
			}
			*lastSlash = '/';
		}

		if (!DeleteSDFile(fileName, false))
		{
			return;
		}

		_state._file = SD.open(fileName, FILE_WRITE);
		if (!GetExecutingFile())
		{
			Error(MESSAGE_PARSER3D_ERROR_CREATING_FILE);
			return;
		}

		StepperSerial.print(MESSAGE_PARSER3D_WRITING_TO_FILE);
		StepperSerial.println(fileName);

		_state._isM28 = true;
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M29Command()
{
	if (_state._isM28)
	{
		if (GetExecutingFile())
		{
			GetExecutingFile().close();
		}

		_state._isM28 = false;
		StepperSerial.println(MESSAGE_PARSER3D_DONE_SAVE_FILE);
	}
}

////////////////////////////////////////////////////////////

void CGCode3DParser::M30Command()
{
	char fileName[MAXPATHNAME];
	if (!CheckSD() || !GetPathName(fileName))
	{
		return;
	}

	if (DeleteSDFile(fileName, true))
	{
		StepperSerial.print(MESSAGE_PARSER3D_FILE_DELETED);
		StepperSerial.println(fileName);
	}
}

////////////////////////////////////////////////////////////

bool CGCode3DParser::CheckSD()
{
	if (GetExecutingFile())
	{
		Error(MESSAGE_PARSER3D_FILE_OCCUPIED);
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////

bool CGCode3DParser::DeleteSDFile(char* fileName, bool errorIfNotExists)
{
	GetExecutingFile() = SD.open(fileName);
	if (GetExecutingFile())
	{
		if (GetExecutingFile().isDirectory())
		{
			GetExecutingFile().close();
			Error(MESSAGE_PARSER3D_DIRECOTRY_SPECIFIED);
			return false;
		}
		GetExecutingFile().close();
		if (!SD.remove(fileName))
		{
			Error(MESSAGE_PARSER3D_CANNOT_DELETE_FILE);
			return false;
		}
		return true;
	}
	else if (errorIfNotExists)
	{
		Error(MESSAGE_PARSER3D_FILE_NOT_EXIST);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////

bool CGCode3DParser::AddPathChar(char ch, char*& buffer, uint8_t& pathLength)
{
	pathLength++;
	if (pathLength >= MAXPATHNAME)
	{
		Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
		return false;
	}
	*(buffer++) = ch;
	return true;
}

bool CGCode3DParser::GetPathName(char* buffer)
{
	char    ch         = _reader->SkipSpaces();
	bool    first      = true;
	uint8_t pathLength = 0;

	while (!CStreamReader::IsSpaceOrEnd(ch) && !IsCommentStart(ch))
	{
		if (ch == '/')
		{
			if (!AddPathChar(ch, buffer, pathLength))
			{
				return false;
			}

			ch = _reader->GetNextChar();
			if (CStreamReader::IsSpaceOrEnd(ch) || IsCommentStart(ch))
			{
				// must not end with /
				Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
				return false;
			}
		}
		else if (first)
		{
			// nothing
		}
		else
		{
			Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
			return false;
		}
		first = false;

		if (!GetFileName(buffer, pathLength))
		{
			return false;
		}

		ch = _reader->GetChar();
	}

	return AddPathChar(0, buffer, pathLength);
}

////////////////////////////////////////////////////////////

bool CGCode3DParser::GetFileName(char*& buffer, uint8_t& pathLength)
{
	uint8_t dotIdx = 0;
	uint8_t length = 0;

	char ch = _reader->GetChar();
	while (true)
	{
		if (ch == '.')
		{
			length = 0;
			dotIdx++;
			if (dotIdx > 1)
			{
				// only one dot is allowed
				Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
				return false;
			}
			if (!AddPathChar(ch, buffer, pathLength))
			{
				return false;
			}
		}
		else if (CStreamReader::IsDigit(ch) || CStreamReader::IsAlpha(ch) || ch == '_' || ch == '~')
		{
			if (!AddPathChar(ch, buffer, pathLength))
			{
				return false;
			}

			length++;

			if ((dotIdx == 0 && length > MAXFILENAME) ||
				(dotIdx == 1 && length > MAXEXTNAME))
			{
				Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
				return false;
			}
		}
		else if (ch == '/' || CStreamReader::IsSpaceOrEnd(ch) || IsCommentStart(ch))
		{
			break;
		}
		else
		{
			Error(MESSAGE_PARSER3D_ILLEGAL_FILENAME);
			return false;
		}
		ch = _reader->GetNextChar();
	}

	return true;
}

////////////////////////////////////////////////////////////

void CGCode3DParser::PrintVersion()
{
	StepperSerial.print(MESSAGE_PARSER3D_VERSION);
}
