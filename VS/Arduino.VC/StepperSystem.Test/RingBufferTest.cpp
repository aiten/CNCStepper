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
	struct SRingBuffer
	{
		double d = 0;
		int    i = 0;
	};

	TEST_CLASS(CRingBufferTest)
	{
	public:

		TEST_METHOD(RingBufferTest)
		{
			CRingBufferQueue<SRingBuffer, 128> buffer;

			Assert::AreEqual(true, buffer.IsEmpty());

			buffer.NextTail().i = 4711;
			buffer.NextTail().d = 4711.4711;

			Assert::AreEqual(true, buffer.IsEmpty());

			buffer.Enqueue();

			Assert::AreEqual(false, buffer.IsEmpty());
			Assert::AreEqual(static_cast<uint8_t>(1), buffer.Count());

			buffer.NextTail().i = 4712;
			buffer.NextTail().d = 4712.4712;

			buffer.Enqueue();

			Assert::AreEqual(false, buffer.IsEmpty());
			Assert::AreEqual(uint8_t(2), buffer.Count());
		}

		TEST_METHOD(RingBufferInsertHeadTest)
		{
			TestRingBufferInsert(10, 60, 0);		// insert at head
		}

		TEST_METHOD(RingBufferInsertTailTest)
		{
			TestRingBufferInsert(10, 60, 60);		// insert at tail (simple enqueue)
		}

		TEST_METHOD(RingBufferInsertTailM1Test)
		{
			TestRingBufferInsert(10, 60, 59);		// insert at tail-1
		}

		TEST_METHOD(RingBufferInsertTail2Test)
		{
			TestRingBufferInsert(0, 60, 30);
		}

		TEST_METHOD(RingBufferOverrunTest)
		{
			TestRingBufferInsert(128 - 10, 60, 30);	// buffer overrun
		}

		void TestRingBufferInsert(uint8_t startIdx, uint8_t bufferSize, uint8_t insertOffset) const
		{
			CRingBufferQueue<SRingBuffer, 128> buffer;

			uint8_t i;

			for (i = 0; i < startIdx; i++)
			{
				buffer.Enqueue();
				buffer.Dequeue();
			}

			for (i = 0; i < bufferSize; i++)
			{
				buffer.NextTail().i = i;
				buffer.Enqueue();
			}

			Assert::AreEqual(bufferSize, buffer.Count());

			uint8_t insertAt = buffer.NextIndex(buffer.GetHeadPos(), insertOffset);

			buffer.InsertTail(insertAt)->i = 2000;

			Assert::AreEqual(uint8_t(bufferSize + 1), buffer.Count());

			int expect = 0;

			for (uint8_t idx = buffer.H2TInit(); buffer.H2TTest(idx); idx = buffer.H2TInc(idx))
			{
				if (idx != insertAt)
					Assert::AreEqual(expect++, buffer.Buffer[idx].i);
				else
					Assert::AreEqual(2000, buffer.Buffer[idx].i);
			}
		}
	};
}
