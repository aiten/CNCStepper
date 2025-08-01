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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "ConfigurationCNCLib.h"
#include "ExpressionParser.h"

////////////////////////////////////////////////////////////
//
// Expression Parser

void CExpressionParser::Parse()
{
	Answer = 0;

	GetNextToken();
	if (GetTokenType() == EndOfLineSy)
	{
		ErrorAdd(MESSAGE_EXPR_EMPTY_EXPR);
		return;
	}

	Answer = ParseLevel1();

	if (IsError()) return;

	// check for garbage at the end of the expression
	// an expression ends with a character '\0' and GetMainTokenType() = delimiter
	if (GetTokenType() != EndOfLineSy)
	{
		ErrorAdd(MESSAGE_EXPR_FORMAT);
		return;
	}
}

////////////////////////////////////////////////////////////

void CExpressionParser::GetNextToken()
{
	_state._detailToken = NothingSy;
	if (IsError()) return;

	char ch = _reader->SkipSpaces();

	if (ch == '\0')
	{
		_state._detailToken = EndOfLineSy;
		return;
	}

	ScanNextToken();
}

////////////////////////////////////////////////////////////
// Get next token in the current string expr.

void CExpressionParser::ScanNextToken()
{
	char ch = _reader->GetChar();

	// @formatter:off — disable formatter after this line
	if (IsToken(F("||"), false, false))	{ _state._detailToken = XOrSy;			 return; }
	if (IsToken(F("<<"), false, false))	{ _state._detailToken = BitShiftLeftSy;  return; }
	if (IsToken(F(">>"), false, false))	{ _state._detailToken = BitShiftRightSy; return; }
	if (IsToken(F("=="), false, false))	{ _state._detailToken = EqualSy;		 return; }
	if (IsToken(F("!="), false, false))	{ _state._detailToken = UnEqualSy;		 return; }
	if (IsToken(F(">="), false, false))	{ _state._detailToken = GreaterEqualSy;  return; }
	if (IsToken(F("<="), false, false))	{ _state._detailToken = LessEqualSy;	 return; }

	if (ch == _leftParenthesis)			{ _state._detailToken = LeftParenthesisSy;	 _reader->GetNextChar(); return; }
	if (ch == _rightParenthesis)		{ _state._detailToken = RightParenthesisSy;	 _reader->GetNextChar(); return; }

	switch (ch)
	{
		case '>': _state._detailToken = GreaterSy;		_reader->GetNextChar(); return;
		case '<': _state._detailToken = LessSy;			_reader->GetNextChar(); return;
		case '&': _state._detailToken = AndSy;			_reader->GetNextChar(); return;
		case '|': _state._detailToken = OrSy;			_reader->GetNextChar(); return;
		case '-': _state._detailToken = MinusSy;		_reader->GetNextChar(); return;
		case '+': _state._detailToken = PlusSy;			_reader->GetNextChar(); return;
		case '*': _state._detailToken = MultiplySy;		_reader->GetNextChar(); return;
		case '/': _state._detailToken = DivideSy;		_reader->GetNextChar(); return;
		case '%': _state._detailToken = ModuloSy;		_reader->GetNextChar(); return;
		case '^': _state._detailToken = PowSy;			_reader->GetNextChar(); return;
		case '!': _state._detailToken = FactorialSy;	_reader->GetNextChar(); return;
		case '=': _state._detailToken = AssignSy;		_reader->GetNextChar(); return;
		default: break;
	}
	// @formatter:on — enable formatter after this line

	// check for a value
	if (CStreamReader::IsDigitDot(ch))
	{
		_state._detailToken = FloatSy;
		_state._number      = GetDouble();
		return;
	}

	// check for variables or functions
	if (IsIdentStart(ch))
	{
		auto start = const_cast<char*>(_reader->GetBuffer());

		ReadIdent();

		auto end = const_cast<char*>(_reader->GetBuffer());
		ch       = _reader->SkipSpaces();

		// temporary terminate buffer with '\00'
		CStreamReader::CSetTemporary terminate(end);

		// check if this is a variable or a function.
		// a function has a parenthesis '(' open after the name

		if (ch == _leftParenthesis)
		{
			// @formatter:off — disable formatter after this line
			if (TryToken(start, F("ABS"), true))		{ _state._detailToken = AbsSy; return; }
			if (TryToken(start, F("EXP"), true))		{ _state._detailToken = ExpSy; return; }
			if (TryToken(start, F("SIGN"), true))		{ _state._detailToken = SignSy; return; }
			if (TryToken(start, F("SQRT"), true))		{ _state._detailToken = SqrtSy; return; }
			if (TryToken(start, F("LOG"), true))		{ _state._detailToken = LogSy; return; }
			if (TryToken(start, F("LOG10"), true))		{ _state._detailToken = Log10Sy; return; }
			if (TryToken(start, F("SIN"), true))		{ _state._detailToken = SinSy; return; }
			if (TryToken(start, F("COS"), true))		{ _state._detailToken = CosSy; return; }
			if (TryToken(start, F("TAN"), true))		{ _state._detailToken = TanSy; return; }
			if (TryToken(start, F("ASIN"), true))		{ _state._detailToken = AsinSy; return; }
			if (TryToken(start, F("ACOS"), true))		{ _state._detailToken = AcosSy; return; }
			if (TryToken(start, F("ATAN"), true))		{ _state._detailToken = AtanSy; return; }
			if (TryToken(start, F("FACTORIAL"), true))	{ _state._detailToken = FactorialFncSy; return; }

			if (TryToken(start, F("FIX"), true))		{ _state._detailToken = FixSy; return; }
			if (TryToken(start, F("FUP"), true))		{ _state._detailToken = FupSy; return; }
			if (TryToken(start, F("ROUND"), true))		{ _state._detailToken = RoundSy; return; }
			// @formatter:on — enable formatter after this line

			Error(MESSAGE_EXPR_UNKNOWN_FUNCTION);
			return;
		}
		else
		{
			_state._detailToken = VariableSy;
			_state._variableOK  = EvalVariable(start, _state._number);
		}
		return;
	}

	// something unknown is found, wrong characters -> a syntax error
	_state._detailToken = UnknownSy;
	Error(MESSAGE_EXPR_SYNTAX_ERROR);
	return;
}

////////////////////////////////////////////////////////////

void CExpressionParser::ReadIdent()
{
	char ch = _reader->GetChar();

	while (CStreamReader::IsAlpha(ch) || CStreamReader::IsDigit(ch))
	{
		ch = _reader->GetNextChar();
	}
}

////////////////////////////////////////////////////////////
// assignment of variable or function

expr_t CExpressionParser::ParseLevel1()
{
	if (GetTokenType() == VariableSy)
	{
		// copy current state
		const char*  e_now     = _reader->GetBuffer();
		SParserState state_now = _state;

		GetNextToken();
		if (GetTokenType() == AssignSy)
		{
			// assignment
			GetNextToken();
			expr_t ans = ParseLevel2();

			AssignVariable(state_now._varName, ans);

			return ans;
		}
		else
		{
			if (!_state._variableOK)
			{
				// unknown variable
				ErrorAdd(MESSAGE_EXPR_UNKNOWN_VARIABLE);
				return 0;
			}

			// go back to previous token
			_reader->ResetBuffer(e_now);
			_state = state_now;
		}
	}

	return ParseLevel2();
}

////////////////////////////////////////////////////////////
// conditional operators and bit shift

expr_t CExpressionParser::ParseLevel2()
{
	expr_t                 ans        = ParseLevel3();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == AndSy || operatorSy == OrSy || operatorSy == BitShiftLeftSy || operatorSy == BitShiftRightSy)
	{
		GetNextToken();
		ans        = EvalOperator(operatorSy, ans, ParseLevel3());
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// conditional operators

expr_t CExpressionParser::ParseLevel3()
{
	expr_t                 ans        = ParseLevel4();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == EqualSy || operatorSy == UnEqualSy || operatorSy == LessSy || operatorSy == LessEqualSy || operatorSy == GreaterSy || operatorSy == GreaterEqualSy)
	{
		GetNextToken();
		ans        = EvalOperator(operatorSy, ans, ParseLevel4());
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// add or subtract

expr_t CExpressionParser::ParseLevel4()
{
	expr_t                 ans        = ParseLevel5();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == PlusSy || operatorSy == MinusSy)
	{
		GetNextToken();
		ans        = EvalOperator(operatorSy, ans, ParseLevel5());
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// multiply, divide, modulus, xor

expr_t CExpressionParser::ParseLevel5()
{
	expr_t                 ans        = ParseLevel6();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == MultiplySy || operatorSy == DivideSy || operatorSy == ModuloSy || operatorSy == XOrSy)
	{
		GetNextToken();
		ans        = EvalOperator(operatorSy, ans, ParseLevel6());
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// power

expr_t CExpressionParser::ParseLevel6()
{
	expr_t                 ans        = ParseLevel7();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == PowSy)
	{
		GetNextToken();
		ans        = EvalOperator(operatorSy, ans, ParseLevel7());
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// Factorial

expr_t CExpressionParser::ParseLevel7()
{
	expr_t                 ans        = ParseLevel8();
	EnumAsByte(ETokenType) operatorSy = GetTokenType();

	while (operatorSy == FactorialSy)
	{
		GetNextToken();
		// factorial does not need a value right from the
		// operator, so zero is filled in.
		ans        = EvalOperator(operatorSy, ans, 0.0);
		operatorSy = GetTokenType();
	}

	return ans;
}

////////////////////////////////////////////////////////////
// Unary minus

expr_t CExpressionParser::ParseLevel8()
{
	if (GetTokenType() == MinusSy)
	{
		GetNextToken();
		return -ParseLevel9();
	}

	return ParseLevel9();
}

////////////////////////////////////////////////////////////
// functions

expr_t CExpressionParser::ParseLevel9()
{
	if (GetTokenType() >= FirstFunctionSy && GetTokenType() <= LastFunctionSy)
	{
		EnumAsByte(ETokenType) functionSy = GetTokenType();
		GetNextToken();
		return EvalFunction(functionSy, ParseLevel10());
	}
	return ParseLevel10();
}

////////////////////////////////////////////////////////////
// parenthesized expression or value

expr_t CExpressionParser::ParseLevel10()
{
	// check if it is a parenthesized expression
	if (GetTokenType() == LeftParenthesisSy)
	{
		GetNextToken();
		expr_t ans = ParseLevel2();
		if (GetTokenType() != RightParenthesisSy)
		{
			ErrorAdd(MESSAGE_EXPR_MISSINGRPARENTHESIS);
			return 0;
		}
		GetNextToken();
		return ans;
	}

	// if not parenthesized then the expression is a value
	return ParseNumber();
}

////////////////////////////////////////////////////////////

expr_t CExpressionParser::ParseNumber()
{
	expr_t ans;

	switch (GetTokenType())
	{
		case FloatSy:
		case IntegerSy:
		case VariableSy:
			// this is a number
			ans = _state._number;
			GetNextToken();
			break;
		default:
			// syntax error or unexpected end of expression
			ErrorAdd(MESSAGE_EXPR_SYNTAX_ERROR);
			return 0;
	}

	return ans;
}

////////////////////////////////////////////////////////////
// evaluate an operator for given values

expr_t CExpressionParser::EvalOperator(EnumAsByte(ETokenType) operatorSy, const expr_t& lhs, const expr_t& rhs)
{
	// @formatter:off — disable formatter after this line
	switch (operatorSy)
	{
		// level 2
		case AndSy:           return expr_t(static_cast<long>(lhs)& static_cast<long>(rhs));
		case OrSy:            return expr_t(static_cast<long>(lhs) | static_cast<long>(rhs));
		case BitShiftLeftSy:  return expr_t(static_cast<long>(lhs) << static_cast<long>(rhs));
		case BitShiftRightSy: return expr_t(static_cast<long>(lhs) >> static_cast<long>(rhs));

			// level 3
		case EqualSy:			return lhs == rhs;
		case UnEqualSy:			return lhs != rhs;
		case LessSy:			return lhs < rhs;
		case GreaterSy:			return lhs > rhs;
		case LessEqualSy:		return lhs <= rhs;
		case GreaterEqualSy:	return lhs >= rhs;

			// level 4
		case PlusSy:			return lhs + rhs;
		case MinusSy:			return lhs - rhs;

			// level 5
		case MultiplySy:		return lhs * rhs;
		case DivideSy:			return lhs / rhs;
		case ModuloSy:			return expr_t(static_cast<long>(lhs) % static_cast<long>(rhs));
		case XOrSy:				return expr_t(static_cast<long>(lhs) ^ static_cast<long>(rhs));

			// level 6
		case PowSy:				return pow(lhs, rhs);

			// level 7
		case FactorialSy:		return Factorial(lhs);

		default: break;
	}
	// @formatter:on — enable formatter after this line

	ErrorAdd(MESSAGE_EXPR_ILLEGAL_OPERATOR);
	return 0;
}

////////////////////////////////////////////////////////////
// evaluate a function

expr_t CExpressionParser::EvalFunction(EnumAsByte(ETokenType) operatorSy, const expr_t& value)
{
	// @formatter:off — disable formatter after this line
	switch (operatorSy)
	{
			// arithmetic
		case AbsSy:  return abs(value);
		case ExpSy:  return exp(value);
		case SignSy:  return Sign(value);
		case SqrtSy:  return sqrt(value);
		case LogSy:  return log(value);
		case Log10Sy:  return log10(value);

			// trigonometric
		case SinSy:  return sin(value);
		case CosSy:  return cos(value);
		case TanSy:  return tan(value);
		case AsinSy:  return asin(value);
		case AcosSy:  return acos(value);
		case AtanSy:  return atan(value);

			// probability
		case FactorialFncSy:  return Factorial(value);

			// cnc
		case FixSy:  return floor(value);
		case FupSy:  return ceil(value);
		case RoundSy:  return round(value);
	
		default: break;
	}
	// @formatter:on — enable formatter after this line

	ErrorAdd(MESSAGE_EXPR_ILLEGAL_FUNCTION);
	return 0;
}

////////////////////////////////////////////////////////////

bool CExpressionParser::EvalVariable(const char* var_name, expr_t& answer)
{
	_state._varName = var_name;

	// check for built-in variables
	// @formatter:off — disable formatter after this line
	if (TryToken(var_name, F("E"), true))  { answer = expr_t(2.7182818284590452353602874713527); return true; }
	if (TryToken(var_name, F("PI"), true)) { answer = expr_t(3.1415926535897932384626433832795); return true; }
	// @formatter:on — enable formatter after this line

	return false;
}

////////////////////////////////////////////////////////////

expr_t CExpressionParser::Factorial(expr_t value)
{
	int v = static_cast<int>(value);

	if (value != static_cast<expr_t>(v))
	{
		ErrorAdd(MESSAGE_EXPR_FRACTORIAL);
		return 0;
	}

	auto res = expr_t(v);
	v--;
	while (v > 1)
	{
		res *= v;
		v--;
	}

	if (res == 0) res = 1;        // 0! is per definition 1
	return res;
}

////////////////////////////////////////////////////////////

expr_t CExpressionParser::Sign(expr_t value)
{
	if (value > 0) return 1;
	if (value < 0) return -1;
	return 0;
}
