
using System;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Input;
using Microsoft.Win32;

namespace SpeedChart.ViewModels
{
   public class MainWindowViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        TimeSampleList _list = new TimeSampleList();
        string _filename;
        string _directory;
        int _fileNo = 1;
        bool _loaded;

        string _content;

        public MainWindowViewModel()
        {
            _directory = Path.GetTempPath();
            _filename = _directory + @"ProxxonMF70.csv";
        }

        #region Environment

        public class DelegateCommand : ICommand
        {
            private readonly Action _command;
            private readonly Func<bool> _canExecute;

            public event EventHandler CanExecuteChanged
            {
                add => CommandManager.RequerySuggested += value;
                remove => CommandManager.RequerySuggested -= value;
            }

            public DelegateCommand(Action command, Func<bool> canExecute = null)
            {
                _canExecute = canExecute;
                _command = command ?? throw new ArgumentNullException();
            }
            
            public async void Execute(object parameter)
            {
                _command();
            }

            public bool CanExecute(object parameter)
            {
                return _canExecute == null || _canExecute();
            }
        }


        protected void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public MainWindow ViewWindow { get; set; }

        #endregion

        #region Properties

        public int FileNo
        {
            get => _fileNo;
            set { _fileNo = value; OnPropertyChanged(); }
        }
        public string Content
        {
            get => _content;
            set { _content = value; OnPropertyChanged(); }
        }

        #endregion

        #region Operations

        #region LoadFile

        public void Browse()
        {
            // Configure open file dialog box
            var dlg = new OpenFileDialog
            {
                InitialDirectory = _directory
            };
            //dlg.FileName = "Document"; // Default file name
            //dlg.DefaultExt = ".txt"; // Default file extension
            //dlg.Filter = "Text documents (.txt)|*.txt"; // Filter files by extension

            // Show open file dialog box
            bool? result = dlg.ShowDialog();

            // Process open file dialog box results
            if (result == true)
            {
                // Open document
                _filename = dlg.FileName;
                _directory = Path.GetDirectoryName(_filename) + @"\";
                LoadFile();
            }
        }

       public bool CanBrowse()
        {
            return true;
        }

        void Load()
        {
			FileNo = 1;
			LoadFile();
		}

		private void LoadFile()
		{
			try
			{
				string filename = GetFilename(_fileNo);
				_list.ReadFiles(filename);
                ViewWindow.SpeedChart.List = _list;
                ViewWindow.SpeedChart.InvalidateVisual();
                _loaded=true;
			}
			catch(FileNotFoundException e)
			{
				MessageBox.Show(e.Message);
			}
			catch (DirectoryNotFoundException e)
			{
				MessageBox.Show(e.Message);
			}
		}

		private string GetFilename(int fileno)
		{
			string filename = _filename;
			if (fileno != 1)
			{
				int dotidx = filename.LastIndexOf('.');
				if (dotidx >= 0)
				{
					filename = filename.Insert(dotidx, "#" + fileno);
				}
			}
			return filename;
		}

        bool CanLoad()
        {
            return true;
        }

        public void LoadNextFile()
        {
            if (CanLoadNextFile())
            {
                FileNo++;
                LoadFile();
            }
        }

        public bool CanLoadNextFile()
        {
            return _loaded && File.Exists(GetFilename(FileNo + 1));
        }

        public void LoadPrevFile()
        {
            if (CanLoadPrevFile())
            {
                FileNo--;
                LoadFile();
            }
        }

        public bool CanLoadPrevFile()
        {
            return _loaded &&  File.Exists(GetFilename(FileNo - 1));
        }

        #endregion 

        #region Zoom/Offset

        public void ZoomIn()
        {
            if (CanZoomIn())
            {
                ViewWindow.SpeedChart.ScaleX = ViewWindow.SpeedChart.ScaleX * 1.1;
                ViewWindow.SpeedChart.ScaleY = ViewWindow.SpeedChart.ScaleY * 1.1;
                ViewWindow.SpeedChart.InvalidateVisual();
            }
        }

        public bool CanZoomIn()
        {
            return true;
        }

        public void ZoomOut()
        {
            if (CanZoomOut())
            {
                ViewWindow.SpeedChart.ScaleX = ViewWindow.SpeedChart.ScaleX / 1.1;
                ViewWindow.SpeedChart.ScaleY = ViewWindow.SpeedChart.ScaleY / 1.1;
                ViewWindow.SpeedChart.InvalidateVisual();
            }
        }

        public bool CanZoomOut()
        {
            return true;
        }

       public void XOfsPlus()
        {
            if (CanXOfsPlus())
            {
                ViewWindow.SpeedChart.OffsetX += 0.1 * ViewWindow.SpeedChart.ScaleX;
                ViewWindow.SpeedChart.InvalidateVisual();
            }
        }

        public bool CanXOfsPlus()
        {
            return true;
        }
        public void XOfsMinus()
        {
            if (CanXOfsMinus())
            {
                ViewWindow.SpeedChart.OffsetX -= 0.1 * ViewWindow.SpeedChart.ScaleX;
                ViewWindow.SpeedChart.InvalidateVisual();
            }
        }

        public bool CanXOfsMinus()
        {
            return true;
        }

        #endregion

        #endregion

        #region Commands

        public ICommand BrowseCommand => new DelegateCommand(Browse, CanBrowse);
        public ICommand LoadCommand => new DelegateCommand(Load, CanLoad);
        public ICommand LoadNextCommand => new DelegateCommand(LoadNextFile, CanLoadNextFile);
        public ICommand LoadPrevCommand => new DelegateCommand(LoadPrevFile, CanLoadPrevFile);
        public ICommand ZoomInCommand => new DelegateCommand(ZoomIn, CanZoomIn);
        public ICommand ZoomOutCommand => new DelegateCommand(ZoomOut, CanZoomOut);
        public ICommand XOfsPlusCommand => new DelegateCommand(XOfsPlus, CanXOfsPlus);
        public ICommand XOfsMinusCommand => new DelegateCommand(XOfsMinus, CanXOfsMinus);

        #endregion
    }
}
