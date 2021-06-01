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

#include "stdafx.h"
#include "..\MsvcStepper\MsvcStepper.h"

#include "CppUnitTest.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CToStringTest)
	{
	public:

		TEST_METHOD(ToStringSDistTest)
		{
			char tmp[20];

			Assert::AreEqual("           0", CSDist::ToString(0, tmp, 12));
			Assert::AreEqual("           1", CSDist::ToString(1, tmp, 12));
			Assert::AreEqual("          -1", CSDist::ToString(-1, tmp, 12));
			Assert::AreEqual("          10", CSDist::ToString(10, tmp, 12));
			Assert::AreEqual("         -10", CSDist::ToString(-10, tmp, 12));
			Assert::AreEqual("  2147483647", CSDist::ToString(LONG_MAX, tmp, 12));
			Assert::AreEqual(" -2147483648", CSDist::ToString(LONG_MIN, tmp, 12));
			Assert::AreEqual("2147483647", CSDist::ToString(LONG_MAX, tmp, 10));
			Assert::AreEqual("-2147483648", CSDist::ToString(LONG_MIN, tmp, 11));
			Assert::AreEqual("xxxxxxxxx", CSDist::ToString(LONG_MIN, tmp, 9));
			Assert::AreEqual("xxxxxxxxxx", CSDist::ToString(LONG_MIN, tmp, 10));
		}

		TEST_METHOD(ToStringMm1000Test)
		{
			char tmp[20];

			Assert::AreEqual("    0.000", CMm1000::ToString(0, tmp, 9, 3));
			Assert::AreEqual("    0.001", CMm1000::ToString(1, tmp, 9, 3));
			Assert::AreEqual("    1.000", CMm1000::ToString(1000, tmp, 9, 3));
			Assert::AreEqual("   12.345", CMm1000::ToString(12345, tmp, 9, 3));
			Assert::AreEqual("   -0.001", CMm1000::ToString(-1, tmp, 9, 3));
			Assert::AreEqual("   -1.000", CMm1000::ToString(-1000, tmp, 9, 3));
			Assert::AreEqual("  -12.345", CMm1000::ToString(-12345, tmp, 9, 3));
			Assert::AreEqual("   0.0000", CMm1000::ToString(0, tmp, 9, 4));
			Assert::AreEqual("   0.0010", CMm1000::ToString(1, tmp, 9, 4));
			Assert::AreEqual("   1.0000", CMm1000::ToString(1000, tmp, 9, 4));
			Assert::AreEqual("  12.3450", CMm1000::ToString(12345, tmp, 9, 4));
			Assert::AreEqual("  -0.0010", CMm1000::ToString(-1, tmp, 9, 4));
			Assert::AreEqual("  -1.0000", CMm1000::ToString(-1000, tmp, 9, 4));
			Assert::AreEqual(" -12.3450", CMm1000::ToString(-12345, tmp, 9, 4));
			Assert::AreEqual("     0.00", CMm1000::ToString(0, tmp, 9, 2));
			Assert::AreEqual("     0.00", CMm1000::ToString(1, tmp, 9, 2));
			Assert::AreEqual("     1.00", CMm1000::ToString(1000, tmp, 9, 2));
			Assert::AreEqual("    12.35", CMm1000::ToString(12345, tmp, 9, 2));
			Assert::AreEqual("    -0.00", CMm1000::ToString(-1, tmp, 9, 2));
			Assert::AreEqual("    -1.00", CMm1000::ToString(-1000, tmp, 9, 2));
			Assert::AreEqual("   -12.35", CMm1000::ToString(-12345, tmp, 9, 2));
			Assert::AreEqual("      0.0", CMm1000::ToString(0, tmp, 9, 1));
			Assert::AreEqual("      0.0", CMm1000::ToString(1, tmp, 9, 1));
			Assert::AreEqual("      1.0", CMm1000::ToString(1000, tmp, 9, 1));
			Assert::AreEqual("     12.3", CMm1000::ToString(12345, tmp, 9, 1));
			Assert::AreEqual("     -0.0", CMm1000::ToString(-1, tmp, 9, 1));
			Assert::AreEqual("     -1.0", CMm1000::ToString(-1000, tmp, 9, 1));
			Assert::AreEqual("    -12.3", CMm1000::ToString(-12345, tmp, 9, 1));
			Assert::AreEqual("        0", CMm1000::ToString(0, tmp, 9, 0));
			Assert::AreEqual("        0", CMm1000::ToString(1, tmp, 9, 0));
			Assert::AreEqual("        1", CMm1000::ToString(1000, tmp, 9, 0));
			Assert::AreEqual("       12", CMm1000::ToString(12345, tmp, 9, 0));
			Assert::AreEqual("       -0", CMm1000::ToString(-1, tmp, 9, 0));
			Assert::AreEqual("       -1", CMm1000::ToString(-1000, tmp, 9, 0));
			Assert::AreEqual("      -12", CMm1000::ToString(-12345, tmp, 9, 0));
			Assert::AreEqual("2147483.647", CMm1000::ToString(LONG_MAX, tmp, 11, 3));
			Assert::AreEqual("-2147483.648", CMm1000::ToString(LONG_MIN, tmp, 12, 3));
			Assert::AreEqual("        1", CMm1000::ToString(500, tmp, 9, 0));
			Assert::AreEqual("        1", CMm1000::ToString(501, tmp, 9, 0));
			Assert::AreEqual("        1", CMm1000::ToString(999, tmp, 9, 0));
		}

		TEST_METHOD(ToStringInch100000Test)
		{
			char tmp[20];

			Assert::AreEqual("      0.0", CInch100000::ToString(0, tmp, 9, 1));
			Assert::AreEqual(" 0.000000", CInch100000::ToString(0, tmp, 9, 6));
			Assert::AreEqual(" 1.000000", CInch100000::ToString(100000, tmp, 9, 6));
			Assert::AreEqual("   0.6667", CInch100000::ToString(66666, tmp, 9, 4));
			Assert::AreEqual("  0.98765", CInch100000::ToString(98765, tmp, 9, 5));
			Assert::AreEqual(" -0.98765", CInch100000::ToString(-98765, tmp, 9, 5));
		}
	};
}
