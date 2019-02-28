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

#include "Parser.h"

#define EXPRPARSER_MAXTOKENLENGTH 16

////////////////////////////////////////////////////////
//
// Expression Parser, 
//	read char from CStreamReader and calculate the result(=Answer)
//
class CExpressionParser : public CParser
{
private:

	typedef CParser super;

public:

	CExpressionParser(CStreamReader* reader, Stream* output) : super(reader, output)
	{
		_LeftParenthesis  = '(';
		_RightParenthesis = ')';
	};

	virtual void Parse() override;

	expr_t Answer;

protected:

	char _LeftParenthesis;
	char _RightParenthesis;

	void         GetNextToken();
	virtual void ScanNextToken();
	virtual void ReadIdent();
	virtual bool IsIdentStart(char ch) { return CStreamReader::IsAlpha(ch); }	// start of function or variable

	virtual bool EvalVariable(const char*  var_name, expr_t& answer);
	virtual void AssignVariable(const char*, expr_t          ) { };

	enum ETokenType
	{
		UnknownSy,
		NothingSy,
		EndOfLineSy,

		AssignSy,
		LeftParenthesisSy,
		RightParenthesisSy,

		// Operator
		// Level2
		AndSy,
		OrSy,
		BitShiftLeftSy,
		BitShiftRightSy,
		// Level3
		EqualSy,
		UnEqualSy,
		LessSy,
		GreaterSy,
		LessEqualSy,
		GreaterEqualSy,
		// Level 4
		PlusSy,
		MinusSy,
		// Level 5
		MultiplySy,
		DivideSy,
		ModuloSy,
		XOrSy,
		// Level 6
		PowSy,
		// Level 7
		FactorialSy,

		IntegerSy,
		FloatSy,

		VariableSy,

		// Functions
		FirstFunctionSy,
		AbsSy = FirstFunctionSy,
		ExpSy,
		SignSy,
		SqrtSy,
		LogSy,
		Log10Sy,
		SinSy,
		CosSy,
		TanSy,
		AsinSy,
		AcosSy,
		AtanSy,
		FixSy,
		FupSy,
		RoundSy,

		FactorialFncSy,
		LastFunctionSy = FactorialFncSy
	};

	struct SParserState
	{
		expr_t      _number;					// number if parsed integer or float or variable(content)
		const char* _varName;
		bool        _variableOK;				// _number = variable with content

		EnumAsByte(ETokenType) _detailToken;
	};

	SParserState _state;

	EnumAsByte(ETokenType) GetTokenType() const { return _state._detailToken; }

	expr_t ParseLevel1();
	expr_t ParseLevel2();
	expr_t ParseLevel3();
	expr_t ParseLevel4();
	expr_t ParseLevel5();
	expr_t ParseLevel6();
	expr_t ParseLevel7();
	expr_t ParseLevel8();
	expr_t ParseLevel9();
	expr_t ParseLevel10();
	expr_t ParseNumber();

	expr_t EvalOperator(EnumAsByte(ETokenType) operatorSy, const expr_t& lhs, const expr_t& rhs);
	expr_t EvalFunction(EnumAsByte(ETokenType) operatorSy, const expr_t& value);

	expr_t Factorial(expr_t value);
	expr_t Sign(expr_t      value);
};

////////////////////////////////////////////////////////
