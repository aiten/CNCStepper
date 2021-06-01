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

//////////////////////////////////////////

#include "HAL.h"

//////////////////////////////////////////

#define RINGBUFFER_NOIDX uint8_t(255)

template <class T, const uint8_t maxSize> // do not use maxSize > 254 (255 is used internal)
class CRingBufferQueue                    // maxSize should be 2^n (because of % operation)
{
public:

	CRingBufferQueue()
	{
		Clear();
	}

	void Dequeue()
	{
		CCriticalRegion criticalRegion;
		_head  = NextIndex(_head);
		_empty = _head == _nextTail;
	}

	void Enqueue()
	{
		CCriticalRegion criticalRegion;
		_nextTail = NextIndex(_nextTail);
		_empty    = false;
	}

	void Enqueue(T value)
	{
		Buffer[GetNextTailPos()] = value;
		Enqueue();
	}

	void EnqueueCount(uint8_t cnt)
	{
		CCriticalRegion criticalRegion;
		_nextTail = NextIndex(_nextTail, cnt);
		_empty    = false;
	}

	void RemoveTail()
	{
		CCriticalRegion criticalRegion;
		_nextTail = PrevIndex(_nextTail);
		_empty    = _head == _nextTail;
	}

	void RemoveTail(uint8_t tail)
	{
		CCriticalRegion criticalRegion;
		_nextTail = NextIndex(tail);
		_empty    = _head == _nextTail;
	}

	bool IsEmpty() const
	{
		return _empty;
	}

	bool IsFull() const
	{
		return !_empty && _head == _nextTail;
	}

	uint8_t Count() const
	{
		if (_empty)
		{
			return 0;
		}

		if (_head < _nextTail)
		{
			return _nextTail - _head;
		}
		return maxSize - _head + _nextTail;
	}

	uint8_t FreeCount() const
	{
		return maxSize - Count();
	}

	T* InsertTail(uint8_t insertAt)
	{
		if (IsInQueue(insertAt))
		{
			for (uint8_t idx = T2HInit(); T2HTest(idx); idx = T2HInc(idx))
			{
				Buffer[NextIndex(idx)] = Buffer[idx];
				if (insertAt == idx)
				{
					break;
				}
			}
		}
		else
		{
			// add tail
			insertAt = GetNextTailPos();
		}

		Enqueue();
		return &Buffer[insertAt];
	}


	// next functions no check if empty or full

	T& Head() { return Buffer[GetHeadPos()]; }
	T& Tail() { return Buffer[GetTailPos()]; }
	T& NextTail() { return Buffer[GetNextTailPos()]; }
	T& NextTail(uint8_t ofs) { return Buffer[NextIndex(GetNextTailPos(), ofs)]; }

	T* SaveTail() { return IsEmpty() ? 0 : &Tail(); }
	T* SaveHead() { return IsEmpty() ? 0 : &Head(); }

	T* GetNext(uint8_t idx)
	{
		idx = NextIndex(idx);
		return idx != _nextTail && IsInQueue(idx) ? &Buffer[idx] : NULL;
	}

	T* GetPrev(uint8_t idx)
	{
		if (idx == _head)
		{
			return nullptr;
		}
		idx = PrevIndex(idx);
		return IsInQueue(idx) ? &Buffer[idx] : NULL;
	}

	uint8_t GetHeadPos() const { return _head; }
	uint8_t GetNextTailPos() const { return _nextTail; }
	uint8_t GetTailPos() const { return PrevIndex(_nextTail); }

	bool IsInQueue(uint8_t idx) const
	{
		if (_empty)
		{
			return false;
		}
		if (_nextTail == _head)
		{
			return true;
		}
		if (_nextTail > _head)
		{
			return idx >= _head && idx < _nextTail;
		}
		return idx >= _head || idx < _nextTail;
	}

	// iteration from head to tail (H2T)
	uint8_t H2TInit() const { return _empty ? RINGBUFFER_NOIDX : _head; }
	bool    H2TTest(uint8_t idx) const { return idx != RINGBUFFER_NOIDX; }

	uint8_t H2TInc(uint8_t idx) const
	{
		idx = NextIndex(idx);
		return idx == _nextTail ? RINGBUFFER_NOIDX : idx;
	}

	// iteration from tail to head (T2H)
	uint8_t T2HInit() const { return _empty ? RINGBUFFER_NOIDX : GetTailPos(); }
	bool    T2HTest(uint8_t idx) const { return idx != RINGBUFFER_NOIDX; }
	uint8_t T2HInc(uint8_t  idx) const { return idx == _head ? RINGBUFFER_NOIDX : PrevIndex(idx); }

	void Clear()
	{
		CCriticalRegion criticalRegion;
		_head     = 0;
		_nextTail = 0;
		_empty    = true;
	}

private:
	// often accessed members first => is faster

	volatile uint8_t _head;     // index of head of queue
	volatile uint8_t _nextTail; // index of next free tail (NOT tail position)
	volatile bool    _empty;    // distinguish between full and empty

public:

	T Buffer[maxSize];

	////////////////////////////////////////////////////////

	static uint8_t NextIndex(uint8_t idx)
	{
		return uint8_t((idx + 1)) % (maxSize);
	}

	static uint8_t NextIndex(uint8_t idx, uint8_t count)
	{
		return uint8_t((idx + count)) % (maxSize);
	}

	static uint8_t PrevIndex(uint8_t idx)
	{
		return idx == 0 ? (maxSize - 1) : (idx - 1);
	}

	static uint8_t PrevIndex(uint8_t idx, uint8_t count)
	{
		return (idx >= count) ? idx - count : (maxSize) - (count - idx);
	}
};
