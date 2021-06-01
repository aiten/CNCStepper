﻿////////////////////////////////////////////////////////
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
using System.Windows.Controls;
using System.Windows.Media;

namespace SpeedChart
{
    /// <summary>
    /// Follow steps 1a or 1b and then 2 to use this custom control in a XAML file.
    ///
    /// Step 1a) Using this custom control in a XAML file that exists in the current project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:SpeedChart"
    ///
    ///
    /// Step 1b) Using this custom control in a XAML file that exists in a different project.
    /// Add this XmlNamespace attribute to the root element of the markup file where it is 
    /// to be used:
    ///
    ///     xmlns:MyNamespace="clr-namespace:SpeedChart;assembly=SpeedChart"
    ///
    /// You will also need to add a project reference from the project where the XAML file lives
    /// to this project and Rebuild to avoid compilation errors:
    ///
    ///     Right click on the target project in the Solution Explorer and
    ///     "Add Reference"->"Projects"->[Browse to and select this project]
    ///
    ///
    /// Step 2)
    /// Go ahead and use your control in the XAML file.
    ///
    ///     <MyNamespace:CustomControl221/>
    ///
    /// </summary>
    public class SpeedChartControl : Control
	{
		static SpeedChartControl()
		{
			DefaultStyleKeyProperty.OverrideMetadata(typeof(SpeedChartControl), new FrameworkPropertyMetadata(typeof(SpeedChartControl)));
		}

		public SpeedChartControl()
		{
			ScaleX = 1;
			ScaleY = 1;
			OffsetX = 0;
			OffsetY = 0;
			MSCInfo = "";
		}

		private static readonly Pen SysPointPen = new Pen(new SolidColorBrush(Colors.Orange),  1.0d);
		private static readonly Pen XPointPen = new Pen(new SolidColorBrush(Colors.Red), 1.0d);
		private static readonly Pen YPointPen = new Pen(new SolidColorBrush(Colors.Green), 1.0d);
		private static readonly Pen ZPointPen = new Pen(new SolidColorBrush(Colors.Blue), 1.0d);

		private static readonly Pen GridPen = new Pen(new SolidColorBrush(Colors.LightGray), 0.5d);

		public TimeSampleList List { get; set; }

		public double ScaleX { get; set; }
		public double ScaleY { get; set; }

		public double OffsetX { get; set; }
		public double OffsetY { get; set; }

		public string MSCInfo { get; set; }

		protected override void OnRender(DrawingContext drawingContext)
		{
			DrawWayPoints(drawingContext);
		}

		struct SDrawInfo
		{
			public double distFrom;
			public double aY;
			public double aX;
			public double scaleX;
			public double scaleY;

			public double offsetX;
			public double offsetY;


			public double ToX(double x) { return (x - distFrom) * scaleX + offsetX * aX; }
			public double ToY(double y) { return aY - y * scaleY + offsetY; }
		};

		SDrawInfo GetDrawInfo()
		{
		    var info = new SDrawInfo
		    {
		        distFrom = List.DistFrom,
		        aY = ActualHeight / 2,
		        aX = ActualWidth,
		        scaleX = ActualWidth / (List.DistTo - List.DistFrom) * ScaleX,
		        scaleY = ActualHeight / List.MaxSpeed / 2 * ScaleY,
		        offsetX = OffsetX,
		        offsetY = OffsetY
		    };

		    return info;
		}

		public TimeSample HitTest(Point pt)
		{
			if (List != null)
			{
				SDrawInfo info = GetDrawInfo();

				foreach (TimeSample ts in List)
				{
					if (info.ToX(ts.Dist) > pt.X)
					{
						return ts;
					}
				}
			}
			return null;
		}

		private void DrawWayPoints(DrawingContext context)
		{
			if (List == null) return;

			SDrawInfo drw = GetDrawInfo();

			SysPointPen.Freeze();
			XPointPen.Freeze();
			YPointPen.Freeze();
			ZPointPen.Freeze();
			GridPen.Freeze();

			var pt1 = new Point();
			var pt2 = new Point();

			pt1.X = 0;
			pt1.Y = drw.ToY(0);

			pt2.X = ActualWidth;
			pt2.Y = pt1.Y;

			context.DrawLine(GridPen, pt1,pt2);

			for (int i = 1; i < 5; i++)
			{
				pt1.Y = drw.ToY(-i*100000);
				pt2.Y = pt1.Y;
				context.DrawLine(GridPen, pt1, pt2);

				pt1.Y = drw.ToY(i * 100000);
				pt2.Y = pt1.Y;
				context.DrawLine(GridPen, pt1, pt2);
			}

			// Draw all points
			foreach (TimeSample ts in List)
			{
				pt1.X = drw.ToX(ts.Dist);
				pt2.X = pt1.X + 1;

				if (pt1.X > drw.aX) break;
				if (pt1.X < 0) continue;
/*
				pt1.Y = aY - ts.SysSpeed * scaleY + offsetY;
				pt2.Y = pt1.Y + 1;

				context.DrawLine(SysPointPen, pt1, pt2);
*/
				if (ts.XSpeed != 0)
				{
					pt1.Y = drw.ToY(ts.XSpeed);
					pt2.Y = pt1.Y+1;
					context.DrawLine(XPointPen, pt1, pt2);
				}
				if (ts.YSpeed != 0)
				{
					pt1.Y = drw.ToY(ts.YSpeed);
					pt2.Y = pt1.Y + 1;
					context.DrawLine(YPointPen, pt1, pt2);
				}
				if (ts.ZSpeed != 0)
				{
					pt1.Y = drw.ToY(ts.ZSpeed);
					pt2.Y = pt1.Y + 1;
					context.DrawLine(ZPointPen, pt1, pt2);
				}
			}
		}
	}
}
