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

        protected override void OnRender(DrawingContext drawingContext)
        {
            var printinfo = ReadLCDFile();

            double x=0;
            double y =0;
            int chsizeX=10;
            int chsizeY=6;

            int sizeX = 128;
            int sizeY = 64;

            double scaleX = ActualWidth / sizeX;
            double scaleY = ActualHeight / sizeY;

            double chscaleSizeX = scaleX * chsizeX;
            double chscaleSizeY= scaleY * chsizeY;

            Typeface tf = new Typeface("Courier");

            foreach (var s in printinfo)
            {
                if (s.StartsWith("SP:"))
                {
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
                    var text = s.Substring(2);
                    FormattedText formattedText = new FormattedText(
                        text,
                        CultureInfo.GetCultureInfo("en-us"),
                        FlowDirection.LeftToRight,
                        tf,
                       ActualHeight/ (chsizeY+1),
                        Brushes.Black);
                    drawingContext.DrawText(formattedText, new Point(x* scaleX, y* scaleY - chscaleSizeY));
                    x += text.Length * chsizeX;
                }
            }
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
