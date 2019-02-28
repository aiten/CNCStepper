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

////////////////////////////////////////////////////////
// 
// class for scanning an CommandStream
//

class CStreamReader
{
public:

	void Init(const char* buffer)
	{
		_buffer = buffer;
		_error  = nullptr;
	}

	bool    IsError() const { return _error != nullptr; }
	error_t GetError() const { return _error; }

	char SkipSpaces();
	char SkipSpacesToUpper() { return Toupper(SkipSpaces()); }

	char GetNextCharSkipScaces()
	{
		_buffer++;
		return SkipSpaces();
	} // move to next and skip spaces

	char GetCharToUpper() { return char(Toupper(*_buffer)); }
	char GetChar() { return *_buffer; }
	char GetNextChar() { return *(++_buffer); }                  // skip current and move to next
	char GetNextCharToUpper() { return Toupper(GetNextChar()); } // skip current and move to next

	bool IsNextChar(const char ch)
	{
		if (ch != GetChar()) return false;
		GetNextChar();
		return true;
	}

	void MoveToEnd() { while (*_buffer) _buffer++; } // move to "no more char in stream"

	const char* GetBuffer() { return _buffer; }
	void        ResetBuffer(const char* buffer) { _buffer = buffer; }

	static char Toupper(char ch) { return IsLowerAZ(ch) ? ch + 'A' - 'a' : ch; }
	static bool IsEOC(char   ch) { return ch == 0 || ch == ';'; } // is EndOfCommand

	static bool IsSpace(char      ch) { return ch == ' ' || ch == '\t' || ch == '\r'; }
	static bool IsSpaceOrEnd(char ch) { return ch == 0 || IsSpace(ch); }

	static bool IsMinus(char    ch) { return ch == '-'; }
	static bool IsDot(char      ch) { return ch == '.'; }
	static bool IsDigit(char    ch) { return ch >= '0' && ch <= '9'; }
	static bool IsDigitDot(char ch) { return IsDigit(ch) || IsDot(ch); }

	static bool IsAlpha(char ch)
	{
		ch = Toupper(ch);
		return ch == '_' || IsUpperAZ(ch);
	}

	static bool IsLowerAZ(char ch) { return ch >= 'a' && ch <= 'z'; }
	static bool IsUpperAZ(char ch) { return ch >= 'A' && ch <= 'Z'; }

	void Error(error_t error)
	{
		_error = error;
		MoveToEnd();
	}

	const char* _buffer;
	error_t     _error;

	class CSetTemporary
	{
	private:
		char* _buffer;
		char  _oldch;

	public:
		CSetTemporary(const char* buffer)
		{
			_buffer  = const_cast<char*>(buffer);
			_oldch   = *_buffer;
			*_buffer = 0;
		}

		CSetTemporary(const char* buffer, char ch)
		{
			_buffer  = const_cast<char*>(buffer);
			_oldch   = *_buffer;
			*_buffer = ch;
		}

		~CSetTemporary() { *_buffer = _oldch; }
	};
};

////////////////////////////////////////////////////////
