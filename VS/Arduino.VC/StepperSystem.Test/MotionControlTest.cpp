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

#include "CppUnitTest.h"

#include "..\MsvcStepper\MsvcStepper.h"

////////////////////////////////////////////////////////

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StepperSystemTest
{
	TEST_CLASS(CMotionControlTest)
	{
	public:
		CMsvcStepper Stepper;

		TEST_METHOD(MotionControlTest)
		{
			Stepper.Init();
			Stepper.UseSpeedSign = true;

			for (axis_t x = 0; x < NUM_AXIS; x++)
			{
				Stepper.SetLimitMax(x, 0x100000);
			}
			Stepper.SetWaitFinishMove(false);

			FeedRateTest();
		}

		void FeedRateTest() const
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return static_cast<mm1000_t>(val); },
				[](axis_t, mm1000_t val) { return sdist_t(val); }
			);

			mm1000_t to1[NUM_AXIS] = { 1000, 0, 0 };
			mm1000_t to2[NUM_AXIS] = { 1000, 2000, 0 };
			mm1000_t to3[NUM_AXIS] = { 1000, 2000, 3000 };

			feedrate_t feedrate = 1234 * 60;

			Assert::AreEqual(long(feedrate), long(mc.CalcFeedRate(to1, feedrate)));
			Assert::AreEqual(long(66225), long(mc.CalcFeedRate(to2, feedrate)));		// 2000/sqrt(1000*1000 + 2000*2000) * 1234
			Assert::AreEqual(long(59359), long(mc.CalcFeedRate(to3, feedrate)));
		}

		TEST_METHOD(FeedRateDistOverrunTest)
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return mm1000_t(val); },
				[](axis_t, mm1000_t val) { return sdist_t(val); }
			);

			mm1000_t to1[] = { 100000, 0, 0 };
			mm1000_t to2[] = { 100000, 20000, 0 };
			mm1000_t to3[] = { 100000, 20000, 30000 };

			feedrate_t feedrate = 1234 * 60;

			Assert::AreEqual(long(feedrate), long(mc.CalcFeedRate(to1, feedrate)));
			Assert::AreEqual(long(72552), long(mc.CalcFeedRate(to2, feedrate)));
			Assert::AreEqual(long(69580), long(mc.CalcFeedRate(to3, feedrate)));
		}

		TEST_METHOD(FeedRateOverrun16BitTest)
		{
			CMotionControlBase mc;
			mc.UnitTest();
			mc.InitConversion(
				[](axis_t, sdist_t  val) { return mm1000_t(val) * 10; },
				[](axis_t, mm1000_t val) { return sdist_t(val) / 10; }
			);

			mm1000_t to1[] = { 100, 0, 0 };
			mm1000_t to2[] = { 100, 200, 0 };
			mm1000_t to3[] = { 100, 200, 300 };

			feedrate_t feedrate = 123456 * 60;

			Assert::AreEqual(long(feedrate), long(mc.CalcFeedRate(to1, 123456 * 60)));
			Assert::AreEqual(long(6613714), long(mc.CalcFeedRate(to2, 123456 * 60)));
			Assert::AreEqual(long(5941732), long(mc.CalcFeedRate(to3, 123456 * 60)));
		}
	};
}
