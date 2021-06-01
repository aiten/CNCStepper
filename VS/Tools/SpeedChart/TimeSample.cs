////////////////////////////////////////////////////////
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

using System;

namespace SpeedChart
{
    public class TimeSample
	{
		public int Index { get; set; }
		public int TimerMS { get; set; }
		public int Dist { get; set; }
		public int SysSpeed { get; set; }
		public int XSpeed { get; set; }
		public int YSpeed { get; set; }
		public int ZSpeed { get; set; }
		public int ASpeed { get; set; }
		public int BSpeed { get; set; }
		public string Info { get; set; }

		public static TimeSample Parse(string line)
		{
			char[] delimiterChars = { ';' };
			string[] col = line.Split(delimiterChars, StringSplitOptions.None);

		    var ts = new TimeSample
		    {
		        Index = int.Parse(col[0]),
		        TimerMS = int.Parse(col[1]),
		        Dist = int.Parse(col[2]),
		        SysSpeed = int.Parse(col[3])
		    };

		    if (!string.IsNullOrEmpty(col[4])) ts.XSpeed = int.Parse(col[4]);
			if (!string.IsNullOrEmpty(col[5])) ts.YSpeed = int.Parse(col[5]);
			if (!string.IsNullOrEmpty(col[6])) ts.ZSpeed = int.Parse(col[6]);
			if (!string.IsNullOrEmpty(col[7])) ts.ASpeed = int.Parse(col[7]);
			if (!string.IsNullOrEmpty(col[8])) ts.BSpeed = int.Parse(col[8]);

			if (col.Length >= 20 && !string.IsNullOrEmpty(col[19])) ts.Info = col[19];

			return ts;
		}
	}
}
