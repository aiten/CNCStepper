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


namespace CreateThermistorLookupBeta
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using System.Windows;

    public class ThermistorLookup
    {
        const double SERIESRESISTOR = 10000;
        const double THERMISTORNOMINAL = 10000;
        const double BCOEFFICIENT = 3950;
        const double TEMPERATURENOMINAL = 25.0;
        const double TEMPSHIFT = 273.15;
        const int MAXDIGITAL = 1023;

        public double Calc(double average)
        {
            double current_resistance = SERIESRESISTOR / (MAXDIGITAL / average - 1);
            if (average == 0.0)
                current_resistance = 1; // must not be inv

            double corr = 1.0 / (TEMPERATURENOMINAL + TEMPSHIFT);

            return 1.0 / ((Math.Log(current_resistance / THERMISTORNOMINAL) / BCOEFFICIENT) + corr) - TEMPSHIFT;
        }


        public double Error(Vector<double> pt1, Vector<double> pt2, Vector<double> pt3)
        {
            var l1 = pt1 - pt2;
            var l2 = pt1 - pt3;

            double alpha1 = Math.Atan2(l1[0], l1[1]);
            double alpha2 = Math.Atan2(l2[0], l2[1]);

            double len2 = Math.Sqrt(l2[0] * l2[0] + l2[1] * l2[1]);

            double h1 = Math.Asin(alpha2 - alpha1) * len2;

            return h1;
        }

        public List<Lookup> GetLookup(Func<double, double> maxerror)
        {
            var list = new List<Lookup>();

            int startidx = MAXDIGITAL / 2;
            int lastidx = startidx;
            double lasttemp = Calc(lastidx);

            list.Add(new Lookup() {X = lastidx, Y = lasttemp});

            for (int i = lastidx + 1; i < MAXDIGITAL; i++)
            {
                for (int j = i; j < MAXDIGITAL; j++)
                {
                    double x3 = lastidx + (i - lastidx) / 2.0;
                    double error = Error(
                        new Vector<double>(new double[] {lastidx, lasttemp}),
                        new Vector<double>(new double[] {i, Calc(i)}),
                        new Vector<double>(new double[] {x3, Calc(x3)}));
                    double maxdiff = maxerror(((double) x3) / MAXDIGITAL);
                    if (Math.Abs(error) > maxdiff)
                    {
                        lastidx = i - 1;
                        lasttemp = Calc(lastidx);

                        list.Add(new Lookup() {X = lastidx, Y = lasttemp, Maxdiff = maxdiff});
                        break;
                    }

                    ;
                }
            }

            if (lastidx != MAXDIGITAL)
                list.Add(new Lookup() {X = MAXDIGITAL, Y = Calc(MAXDIGITAL)});

            lastidx = startidx;
            lasttemp = Calc(lastidx);

            for (int i = lastidx - 1; i >= 0; i--)
            {
                for (int j = i; j >= 0; j--)
                {
                    double x3 = lastidx + (i - lastidx) / 2.0;
                    double error = Error(
                        new Vector<double>(new double[] {lastidx, lasttemp}),
                        new Vector<double>(new double[] {i, Calc(i)}),
                        new Vector<double>(new double[] {x3, Calc(x3)}));
                    double maxdiff = maxerror(((double) x3) / MAXDIGITAL);
                    if (Math.Abs(error) > maxdiff)
                    {
                        lastidx = i + 1;
                        lasttemp = Calc(lastidx);

                        list.Insert(0, new Lookup() {X = lastidx, Y = lasttemp, Maxdiff = maxdiff});
                        break;
                    }

                    ;
                }
            }

            if (lastidx != 0)
                list.Insert(0, new Lookup() {X = 0, Y = Calc(0)});

            return list;
        }

        public struct Lookup
        {
            public double X;
            public double Y;
            public double Maxdiff;
        };
    }
}