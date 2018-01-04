////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

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
