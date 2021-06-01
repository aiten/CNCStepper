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

template <class T> class CMatrix3x3
{
private:

	T _v[3][3];

public:

	static void Invert(const T src[3][3], T dest[3][3])
	{
		T determinant =
			src[0][0] * (src[1][1] * src[2][2] - src[2][1] * src[1][2]) -
			src[0][1] * (src[1][0] * src[2][2] - src[1][2] * src[2][0]) +
			src[0][2] * (src[1][0] * src[2][1] - src[1][1] * src[2][0]);

		dest[0][0] = (src[1][1] * src[2][2] - src[2][1] * src[1][2]) / determinant;
		dest[0][1] = -(src[1][0] * src[2][2] - src[1][2] * src[2][0]) / determinant;
		dest[0][2] = (src[1][0] * src[2][1] - src[2][0] * src[1][1]) / determinant;
		dest[1][0] = -(src[0][1] * src[2][2] - src[0][2] * src[2][1]) / determinant;
		dest[1][1] = (src[0][0] * src[2][2] - src[0][2] * src[2][0]) / determinant;
		dest[1][2] = -(src[0][0] * src[2][1] - src[2][0] * src[0][1]) / determinant;
		dest[2][0] = (src[0][1] * src[1][2] - src[0][2] * src[1][1]) / determinant;
		dest[2][1] = -(src[0][0] * src[1][2] - src[1][0] * src[0][2]) / determinant;
		dest[2][2] = (src[0][0] * src[1][1] - src[1][0] * src[0][1]) / determinant;
		/*
				T determinant=0;
				uint8_t i,j;
		 
				for(i=0;i<3;i++)
					determinant = determinant + (src[0][i]*(src[1][(i+1)%3]*src[2][(i+2)%3] - src[1][(i+2)%3]*src[2][(i+1)%3]));
		 
				for(i=0;i<3;i++)
				{
					for(j=0;j<3;j++)
					{
						dest [i][j] = ((src[(i+1)%3][(j+1)%3] * src[(i+2)%3][(j+2)%3]) - (src[(i+1)%3][(j+2)%3]*src[(i+2)%3][(j+1)%3]))/ determinant;
					}
				}
		*/
	}

	void Invert(T dest[3][3]) const
	{
		Invert(_v, dest);
	}

	static void Mul(const T src[3][3], const T srcV[3], T dest[3])
	{
		dest[0] = src[0][0] * srcV[0] + src[0][1] * srcV[1] + src[0][2] * srcV[2];
		dest[1] = src[1][0] * srcV[0] + src[1][1] * srcV[1] + src[1][2] * srcV[2];
		dest[2] = src[2][0] * srcV[0] + src[2][1] * srcV[1] + src[2][2] * srcV[2];
	}
};
