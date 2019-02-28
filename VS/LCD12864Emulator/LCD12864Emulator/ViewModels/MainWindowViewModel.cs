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

using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Input;
using LCD12864Emulator.Helpers;

namespace LCD12864Emulator.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        public MainWindowViewModel()
        {
        }

        #region Properties

        #endregion

        #region Operations

        void WriteFile(string fileName, bool value)
        {
            while (true)
            {
                try
                {
                    File.WriteAllText(fileName, value ? "1" : "0");
                    break;
                }
                catch (IOException)
                {
                }
            }
        }

        async Task WaitFile(string filename, int timeout)
        {
            var sw = new Stopwatch();
            sw.Start();
            while (sw.ElapsedMilliseconds < timeout)
            {
                if (!File.Exists(filename))
                    break;
                await Task.Delay(10);
            }
        }

        private static string GetFileName(uint pin)
        {
            return $"{Path.GetTempPath()}\\CNCLib_digitalReadFor_{pin}.txt";
        }

        async Task SetPin(uint pin, bool value, int timeout)
        {
            string filename = GetFileName(pin);
            WriteFile(filename, value);
            await WaitFile(filename,timeout);
            WriteFile(filename, !value);
        }
        async Task RotarySetPin(uint pin1, uint pin2, int timeout)
        {
            string filename1 = GetFileName(pin1);
            string filename2 = GetFileName(pin2);
            WriteFile(filename1, true);
            await WaitFile(filename1, timeout);
            WriteFile(filename2, true);
            await WaitFile(filename2, timeout);
            WriteFile(filename1, false);
            await WaitFile(filename1, timeout);
            WriteFile(filename2, false);
            await WaitFile(filename2, timeout);
        }

        #endregion

        #region Commands

        public ICommand RottaryButtonCommand => new DelegateCommand(async () => await SetPin(35,false, 500), () => true);
        public ICommand RottaryLeftCommand => new DelegateCommand(async () => await RotarySetPin(33, 31, 500), () => true);
        public ICommand RottaryRightCommand => new DelegateCommand(async () => await RotarySetPin(31,33,500), () => true);

        #endregion
    }
}
