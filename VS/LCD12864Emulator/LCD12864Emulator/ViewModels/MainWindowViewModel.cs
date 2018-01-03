
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using LCD12864Emulator.Helpers;
using Microsoft.Win32;

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
                    System.IO.File.WriteAllText(fileName, value ? "1" : "0");
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
                if (!System.IO.File.Exists(filename))
                    break;
                await Task.Delay(10);
            }
        }

        private static string GetFileName(uint pin)
        {
            return $"{System.IO.Path.GetTempPath()} \\CNCLib_digitalReadFor_{pin}.txt";
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
        public ICommand RottaryLeftCommand => new DelegateCommand(async () => await RotarySetPin(31,33,500), () => true);
        public ICommand RottaryRightCommand => new DelegateCommand(async () => await RotarySetPin(33,31, 500), () => true);

        #endregion
    }
}
