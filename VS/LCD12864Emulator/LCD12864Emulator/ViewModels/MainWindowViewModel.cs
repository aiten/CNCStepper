
using System;
using System.ComponentModel;
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

        void WriteFile(uint pin, bool value)
        {
            while (true)
            {
                var fileName = GetFileName(pin);
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

        void DeleteFile(string filename)
        {
            while (true)
            {
                try
                {
                    System.IO.File.Delete(filename);
                    break;
                }
                catch (IOException)
                {
                }
            }
        }

        private static string GetFileName(uint pin)
        {
            return $"{System.IO.Path.GetTempPath()} \\CNCLib_digitalReadFor_{pin}.txt";
        }


        async Task SetPin(uint pin, int timeout)
        {
            WriteFile(pin, true);
            await Task.Delay(timeout);
            DeleteFile(GetFileName(pin));
        }
        async Task RotarySetPin(uint pin1, uint pin2, int timeout)
        {
            WriteFile(pin1, true);
            await Task.Delay(timeout);
            WriteFile(pin2, true);
            await Task.Delay(timeout);
            DeleteFile(GetFileName(pin1));
            await Task.Delay(timeout);
            DeleteFile(GetFileName(pin2));
        }

        #endregion

        #region Commands

        public ICommand RottaryButtonCommand => new DelegateCommand(async () => await SetPin(35,500), () => true);
        public ICommand RottaryLeftCommand => new DelegateCommand(async () => await RotarySetPin(31,33,150), () => true);
        public ICommand RottaryRightCommand => new DelegateCommand(async () => await RotarySetPin(33,31, 150), () => true);

        #endregion
    }
}
