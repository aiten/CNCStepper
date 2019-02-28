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

#include "stdafx.h"

#include <LinearLookUp.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

////////////////////////////////////////////////////////

namespace StepperSystemTest
{
	CLinearLookup<int, int>::SLookupTable linearII[] =
	{
		{ 10, 10 },
		{ 20, 100 },
		{ 30, 1000 }
	};

	CLinearLookup<int, float>::SLookupTable linearIF[] =
	{
		{ 10, float(0.1) },
		{ 20, float(0.01) },
		{ 30, float(0.001) }
	};

	CLinearLookup<long, long>::SLookupTable linearLL[255];


	TEST_CLASS(CLinearLookupTests)
	{
	public:

		TEST_METHOD(LinearLookupIntToIntTest)
		{
			CLinearLookup<int, int> ii(linearII, sizeof(linearII) / sizeof(CLinearLookup<int, int>::SLookupTable));

			Assert::AreEqual(10, ii.Lookup(10));
			Assert::AreEqual(100, ii.Lookup(20));
			Assert::AreEqual(1000, ii.Lookup(30));

			Assert::AreEqual(10, ii.Lookup(9));		// less is lower bound
			Assert::AreEqual(1000, ii.Lookup(31));	// more is upper bound

			Assert::AreEqual(10 + (90 * 1 / 10), ii.Lookup(11));
			Assert::AreEqual(10 + (90 * 5 / 10), ii.Lookup(15));
			Assert::AreEqual(10 + (90 * 9 / 10), ii.Lookup(19));
		}

		TEST_METHOD(LinearLookupIntToFloatTest)
		{
			CLinearLookup<int, float> lf(linearIF, sizeof(linearIF) / sizeof(CLinearLookup<int, float>::SLookupTable));

			Assert::AreEqual(float(0.1), lf.Lookup(10));
			Assert::AreEqual(float(0.01), lf.Lookup(20));
			Assert::AreEqual(float(0.001), lf.Lookup(30));

			Assert::AreEqual(float(0.1), lf.Lookup(9));		// less is lower bound
			Assert::AreEqual(float(0.001), lf.Lookup(31));		// more is upper bound

			Assert::AreEqual(float(0.1 - (0.09 * 1 / 10)), lf.Lookup(11));
			Assert::AreEqual(float(0.1 - (0.09 * 5 / 10)), lf.Lookup(15));
			Assert::AreEqual(float(0.1 - (0.09 * 9 / 10)), lf.Lookup(19), 0.001f);
		}

		TEST_METHOD(LinearLookupLongToLongTest)
		{
			// test max Size (255)

			CLinearLookup<long, long> ll(linearLL, sizeof(linearLL) / sizeof(CLinearLookup<long, long>::SLookupTable));

			for (int i = 0; i < 255; i++)
			{
				linearLL[i].input  = i * 256;
				linearLL[i].output = i * 2560;
			}

			Assert::AreEqual(0l, ll.Lookup(-1));
			Assert::AreEqual(254 * 2560l, ll.Lookup(254 * 256 + 1l));
		}
	};
}
