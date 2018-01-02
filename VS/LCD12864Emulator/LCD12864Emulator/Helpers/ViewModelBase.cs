using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace LCD12864Emulator.Helpers
{
    public class ViewModelBase : INotifyPropertyChanged
    {
        #region INPC 

        public event PropertyChangedEventHandler PropertyChanged;

        protected void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        #endregion
    }
}
