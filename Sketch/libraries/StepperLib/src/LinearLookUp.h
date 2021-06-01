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

template <class TInput, class TOutput> class CLinearLookup
{
public:
	struct SLookupTable
	{
		TInput  input;
		TOutput output;
	};

	typedef uint8_t index_t;

	TInput GetInput(index_t i) const
	{
#if defined(__AVR_ARCH__)

		if (TInput(1) / 2 != 0)
		{
			return (TInput)pgm_read_float(&_table[i].input);
		}
		if (sizeof(TInput) == 4)
		{
			return (TInput)pgm_read_dword(&_table[i].input);
		}
		if (sizeof(TInput) == 2)
		{
			return (TInput)pgm_read_word(&_table[i].input);
		}
		return (TInput)pgm_read_byte(&_table[i].input);
#else
		return _table[i].input;
#endif
	}

	TOutput GetOutput(index_t i) const
	{
#if defined(__AVR_ARCH__)
		if (TOutput(1) / 2 != 0)
		{
			return (TOutput)pgm_read_float(&_table[i].output);
		}
		if (sizeof(TOutput) == 4)
		{
			return (TOutput)pgm_read_dword(&_table[i].output);
		}
		if (sizeof(TOutput) == 2)
		{
			return (TOutput)pgm_read_word(&_table[i].output);
		}
		return (TOutput)pgm_read_byte(&_table[i].output);
#else
		return _table[i].output;
#endif
	}

	TOutput LinearInterpolation(TInput input, index_t i) const
	{
		TInput  distInput  = input - GetInput(i);
		TInput  diffInput  = GetInput(i + 1) - GetInput(i);
		TOutput diffOutput = GetOutput(i + 1) - GetOutput(i);

		//return pTable[i].output + ( distInput / diffInput  ) * diffOutput; => OK if TInput is float
		return GetOutput(i) + (distInput * diffOutput) / diffInput;
	}

	TOutput Lookup(TInput input) const
	{
		// table must be sorted!!!!
		// binary search

		index_t left  = 0;
		index_t right = _tableSize - 1;

		if (_tableSize == 0) return TOutput(0);

		while (true)
		{
			index_t c = left + ((right - left) / 2);

			TInput val = GetInput(c);

			if (val == input)
			{
				// no linear approximation (we found the exact value)
				return GetOutput(c);
			}
			else
			{
				if (val > input)
				{
					if (c == 0)
					{
						// no approximation => input < first table entry 
						return GetOutput(c);
					}

					right = c - 1;
					if (left > right)
					{
						// linear approximation between c-1 and c
						return LinearInterpolation(input, c - 1);
					}
				}
				else
				{
					if (c == (_tableSize - 1))
					{
						// no approximation => input > last table entry 
						return GetOutput(c);
					}

					left = c + 1;
					if (left > right)
					{
						// linear approximation between c and c+1
						return LinearInterpolation(input, c);
					}
				}
			}
		}
	}

	CLinearLookup(const SLookupTable* table, index_t tableSize)
	{
		_tableSize = tableSize;
		_table     = table;
	}

private:

	const SLookupTable* _table;
	uint8_t             _tableSize;
};
