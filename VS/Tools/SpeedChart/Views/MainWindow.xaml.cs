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

using System.Windows;
using System.Windows.Input;

namespace SpeedChart
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
	{
		public MainWindow()
		{

			InitializeComponent();

            var vm = DataContext as ViewModels.MainWindowViewModel;
            vm.ViewWindow = this;
        }

        public SpeedChartControl SpeedChart => _chart;

	    protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);
			Point pt = e.GetPosition(this);

			TimeSample ts = _chart.HitTest(pt);

			if (ts != null)
			{
                (DataContext as ViewModels.MainWindowViewModel).Content = ts.Index + ": " + ts.Info;
			}
		}
/*
		protected async override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);
			Point pt = e.GetPosition(this);

			var ts = Task<TimeSample>.Factory.StartNew(() => _chart.HitTest(pt));
			await ts;

			if (ts.Result != null)
			{
				_pos.Content = ts.Result.Index + ": " + ts.Result.Info;
			}
		}
 */
	}
}
