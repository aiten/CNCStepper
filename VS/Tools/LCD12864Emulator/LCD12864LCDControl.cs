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
using System.Text;
using System.Timers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace LCD12864Emulator
{
    public partial class LCD12864LCDControl :Control
    {
        public LCD12864LCDControl()
        {
            _timer = new Timer(50);
            _timer.Elapsed += (sender, e) => HandleTimer();
            _timer.Start();
        }

        private Timer _timer;
        private DateTime _lastLCDFileDate;

        private void HandleTimer()
        {
            DateTime lcdFileDate = System.IO.File.GetLastWriteTime(_fileName);
            if (_lastLCDFileDate != lcdFileDate)
            {
                Dispatcher.Invoke(InvalidateVisual);
                _lastLCDFileDate = lcdFileDate;
            }
        }

        private string _fileName = System.IO.Path.GetTempPath() + @"\CNCLib_LCD.txt";

        static LCD12864LCDControl()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(LCD12864LCDControl), new FrameworkPropertyMetadata(typeof(LCD12864LCDControl)));
        }

        int _chSizeX=10;
        int _chSizeY=6;

        int _sizeX = 128;
        int _sizeY = 64;

        double ScaleX => ActualWidth / _sizeX;
        double ScaleY => ActualHeight / _sizeY;

        double ChScaleSizeX => ScaleX * _chSizeX;
        double ChScaleSizeY => ScaleY * _chSizeY;

        Typeface _tf = new Typeface("Courier New");

        protected override void OnRender(DrawingContext drawingContext)
        {
            string[] printInfo = ReadLCDFile();

            double x=0;
            double y =0;
            string text=null;

            foreach (string s in printInfo)
            {
                if (s.StartsWith("SP:"))
                {
                    if (!string.IsNullOrEmpty(text))
                    {
                        DrawText(drawingContext, text, x, y);
                        text = null;
                    }
                    // SP: 0:21
                    string[] cord = s.Split(':');
                    if (cord.Length == 3)
                    {
                        x = int.Parse(cord[1]);
                        y = int.Parse(cord[2]);
                    }
                }
                else if (s.StartsWith("P:"))
                {
                    // SP:0:21
                    if (string.IsNullOrEmpty(text))
                    {
                        text = s.Substring(2);
                    }
                    else
                    {
                        text = text+s.Substring(2);
                    }
                }
            }
            if (!string.IsNullOrEmpty(text))
            {
                DrawText(drawingContext, text, x, y);
            }
        }

        private void DrawText(DrawingContext drawingContext, string text, double x, double y)
        {
            var formattedText = new FormattedText(
                text,
                CultureInfo.GetCultureInfo("en-us"),
                FlowDirection.LeftToRight,
                _tf,
                ActualHeight / (_chSizeY + 1),
                Brushes.Black);
            drawingContext.DrawText(formattedText, new Point(x * ScaleX, y * ScaleY - ChScaleSizeY));
        }

        private string[] ReadLCDFile()
        {
            try
            {
                return System.IO.File.ReadAllLines(_fileName, new ASCIIEncoding());
            }
            catch (Exception)
            {
                return new string[0];
            }
        }
    }
}
