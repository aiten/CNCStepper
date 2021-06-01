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
using System.Globalization;

using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace CreateThermistorLookupBeta
{
	class Program
	{

		static void Main(string[] args)
		{
			var c = new ThermistorLookup();

			var list = c.GetLookup((double ofs) => Math.Max(0.05,Math.Abs(Math.Pow(Math.Abs(ofs - 0.5),1.8))));
			foreach(var l in list)
			{
				Console.WriteLine("{{ {0}, {1} }}, // {2}", l.X.ToString(CultureInfo.InvariantCulture), l.Y.ToString(CultureInfo.InvariantCulture), l.Maxdiff.ToString(CultureInfo.InvariantCulture));
			}

			if (false)
			{
				for (int i = 0; i < 1024; i++)
				{
					double temp = c.Calc(i);

					Console.Write(i);
					//				Console.Write(":");
					//				Console.Write(current_resistance);
					Console.Write("=>");
					Console.Write(temp);
					Console.WriteLine();
				}
			}
			Console.ReadLine();
		}
	}
}
