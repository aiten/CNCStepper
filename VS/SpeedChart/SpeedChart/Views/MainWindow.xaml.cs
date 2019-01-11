////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
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
