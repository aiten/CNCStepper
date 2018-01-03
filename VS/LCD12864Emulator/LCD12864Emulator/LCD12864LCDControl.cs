using System;
using System.Globalization;
using System.Text;
using System.Threading.Tasks;
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

        int _chsizeX=10;
        int _chsizeY=6;

        int _sizeX = 128;
        int _sizeY = 64;

        double ScaleX => ActualWidth / _sizeX;
        double ScaleY => ActualHeight / _sizeY;

        double ChScaleSizeX => ScaleX * _chsizeX;
        double ChScaleSizeY => ScaleY * _chsizeY;

        Typeface _tf = new Typeface("Courier");

        protected override void OnRender(DrawingContext drawingContext)
        {
            var printinfo = ReadLCDFile();

            double x=0;
            double y =0;
            string text=null;

            foreach (var s in printinfo)
            {
                if (s.StartsWith("SP:"))
                {
                    if (!string.IsNullOrEmpty(text))
                    {
                        DrawText(drawingContext, text, x, y);
                        text = null;
                    }
                    // SP: 0:21
                    var cord = s.Split(':');
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
            FormattedText formattedText = new FormattedText(
                text,
                CultureInfo.GetCultureInfo("en-us"),
                FlowDirection.LeftToRight,
                _tf,
                ActualHeight / (_chsizeY + 1),
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
