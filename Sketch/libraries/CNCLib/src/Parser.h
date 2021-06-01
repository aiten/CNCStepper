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

#include "StreamReader.h"
#include <ConfigurationStepperLib.h>
#include <MessageStepperLib.h>
#include "MessageCNCLib.h"

////////////////////////////////////////////////////////

typedef float expr_t;

////////////////////////////////////////////////////////
//
// Base class for command parser/Scanner
//
class CParser
{
public:

	CParser(CStreamReader* reader, Stream* output)
	{
		_reader    = reader;
		_output    = output;
		_error     = nullptr;
		_OkMessage = nullptr;
	};

	void ParseCommand();

	bool    IsError() const { return _error != nullptr; };
	error_t GetError() const { return _error; }

	typedef void (*PrintMessage)();

	PrintMessage GetOkMessage() const { return _OkMessage; }

	CStreamReader* GetReader() const { return _reader; }
	Stream*        GetOutput() const { return _output; }

	static void Init() { }

protected:

	////////////////////////////////////////////////////////

	virtual void Parse() = 0;
	virtual bool InitParse() { return true; }; // begin parsing of a command (override for pre checks), return true to continue
	virtual void CleanupParse() { }; // called after call to Parse()

	bool         CheckError();
	virtual char SkipSpacesOrComment() { return _reader->SkipSpaces(); }
	void         SkipCommentSingleLine() { _reader->MoveToEnd(); };

	bool ExpectEndOfCommand();
	void ErrorNotImplemented() { Error(MESSAGE(MESSAGE_PARSER_NotImplemented)); }
	void ErrorNotANumber() { Error(MESSAGE(MESSAGE_PARSER_NotANumber)); };
	void ErrorNumberRange() { Error(MESSAGE(MESSAGE_PARSER_OverrunOfNumber)); };
	void Error() { Error(MESSAGE_UNKNOWNERROR); }

	////////////////////////////////////////////////////////

	void ErrorAdd(error_t error) { if (!IsError()) Error(error); }

	void Error(error_t error)
	{
		_error = error;
		_reader->MoveToEnd();
	}

	void Info(error_t s1)
	{
		if (_output)
		{
			_output->print(MESSAGE_INFO);
			_output->println(s1);
		}
	}

	void Warning(error_t s1)
	{
		if (_output)
		{
			_output->print(MESSAGE_WARNING);
			_output->println(s1);
		}
	}

	Stream*        _output;
	CStreamReader* _reader;
	error_t        _error;
	PrintMessage   _OkMessage;

public:

	int32_t GetInt32Scale(int32_t minvalue, int32_t maxvalue, uint8_t scale, uint8_t maxScale); // get "float" e.g. 1.234 => 1234 or 12 => 12000, limit with scale
	expr_t  GetDouble();

	uint8_t  GetUInt8();
	uint16_t GetUInt16();
	uint32_t GetUInt32();
	char     GetInt8();
	int16_t  GetInt16();
	int32_t  GetInt32();
	sdist_t  GetSDist();

	static bool IsUInt(char ch) { return CStreamReader::IsDigit(ch); }
	static bool IsInt(char  ch) { return CStreamReader::IsMinus(ch) || CStreamReader::IsDigit(ch); }

protected:

	//////////////////////////////////////////////////////
	// TextSearch  (=Tokens)

	// Content of "b": 
	//	  \001	=> any digit (last command char)
	//	  \002	=> space or end or not digit (last command char), e.g. g0\002 will find g0 and not g01
	//	  \003	=> \000 (end of line)

	bool IsToken(FLSTR  b, bool expectDel, bool ignoreCase);
	bool TryToken(FLSTR b, bool expectDel, bool ignoreCase) { return TryToken(_reader->GetBuffer(), b, expectDel, ignoreCase); }

	bool TryToken(const char* buffer, FLSTR b, bool expectDel, bool ignoreCase); // scan from different location, but do not remove it
	bool TryToken(const char* buffer, FLSTR b, bool ignoreCase); // same as stricmp (with PROGMEM)	

	//////////////////////////////////////////////////////

private:

	static char unsigned ConvertChar(const char ch, bool ignoreCase) { return ignoreCase ? CStreamReader::Toupper(ch) : ch; }

	bool TryToken(const char* buffer, FLSTR b, bool expectDel, bool ignoreCase, const char*& nextchar); // scan, but do not remove it

	////////////////////////////////////////////////////////

private:

	template <class T> T GetUInt()
	{
		if (!CStreamReader::IsDigit(_reader->GetChar()))
		{
			ErrorNotANumber();
			return 0;
		}
		T value = 0;
		while (CStreamReader::IsDigit(_reader->GetChar()))
		{
			T old = value;
			value *= T(10);
			value += _reader->GetChar() - '0';
			if (old > value)
			{
				ErrorNumberRange();
				return 0;
			}
			_reader->GetNextChar();
		}
		return value;
	}

	template <class T> T GetInt()
	{
		bool negativ;
		if ((negativ = CStreamReader::IsMinus(_reader->GetChar())) != 0)
			_reader->GetNextChar();
		auto value = T(GetUInt<T>());
		return negativ ? -value : value;
	}

	template <class T> T GetUInt(T minvalue, T maxvalue)
	{
		T value = GetUInt<T>();
		if (value < minvalue) Error(MESSAGE_PARSER_ValueLessThanMin);
		else if (value > maxvalue) Error(MESSAGE_PARSER_ValueGreaterThanMax);
		return value;
	}

	template <class T> T GetInt(T minvalue, T maxvalue)
	{
		T value = GetInt<T>();
		if (value < minvalue) Error(MESSAGE_PARSER_ValueLessThanMin);
		else if (value > maxvalue) Error(MESSAGE_PARSER_ValueGreaterThanMax);
		return value;
	}
};

////////////////////////////////////////////////////////
