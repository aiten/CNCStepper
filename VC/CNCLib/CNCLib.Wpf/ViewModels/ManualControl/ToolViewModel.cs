﻿////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2016 Herbert Aitenbichler

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

using System;
using System.Windows.Input;
using Framework.Wpf.Helpers;
using Framework.Arduino;

namespace CNCLib.Wpf.ViewModels.ManualControl
{
	public class ToolViewModel : DetailViewModel , IDisposable
	{
		#region ctr

		public ToolViewModel(IManualControlViewModel vm)
			: base(vm)
		{
			Com.CommandQueueChanged += new ArduinoSerialCommunication.CommandEventHandler(CommandQueueChanged);
		}

		public void Dispose()
		{
			Com.CommandQueueChanged -= new ArduinoSerialCommunication.CommandEventHandler(CommandQueueChanged);
		}

		#endregion

		#region Properties

		public int PendingCommandCount
		{
			get	{ return Com.CommandsInQueue; }
		}

		private void CommandQueueChanged(object sender, ArduinoSerialCommunicationEventArgs arg)
		{
			OnPropertyChanged(() => PendingCommandCount);
		}

		#endregion

		#region Commands / CanCommands
		public bool CanSendSpindle()
		{
			return CanSend() && Global.Instance.Machine.Spindle;
		}
		public bool CanSendCoolant()
		{
			return CanSend() && Global.Instance.Machine.Coolant;
		}
        public bool CanSendLaser()
        {
            return CanSend() && Global.Instance.Machine.Laser;
        }

        public void SendInfo() { AsyncRunCommand(() => { Com.SendCommand("?"); }); }
		public void SendAbort() { AsyncRunCommand(() => { Com.AbortCommands(); Com.ResumeAfterAbort(); Com.SendCommand("!"); }); }
		public void SendResurrect() { AsyncRunCommand(() => { Com.AbortCommands(); Com.ResumeAfterAbort(); Com.SendCommand("!!!"); }); }
		public void ClearQueue() { AsyncRunCommand(() => { Com.AbortCommands(); Com.ResumeAfterAbort(); }); }
		public void SendCNCLibCommand(string command) { AsyncRunCommand(() => { Com.SendCommand(command); }); }
		public void SendM03SpindelOn() { AsyncRunCommand(() => { Com.SendCommand("m3"); }); }
		public void SendM05SpindelOff() { AsyncRunCommand(() => { Com.SendCommand("m5"); }); }
		public void SendM07CoolandOn() { AsyncRunCommand(() => { Com.SendCommand("m7"); }); }
		public void SendM09CoolandOff() { AsyncRunCommand(() => { Com.SendCommand("m9"); }); }
        public void SendM106LaserOn() { AsyncRunCommand(() => { Com.SendCommand("m106 s255"); }); }
        public void SendM106LaserOnMin() { AsyncRunCommand(() => { Com.SendCommand("m106 s1"); }); }
        public void SendM107LaserOff() { AsyncRunCommand(() => { Com.SendCommand("m107"); }); }
        public void SendM114PrintPos()
		{
			AsyncRunCommand(() =>
			{
				string message = Com.SendCommandAndRead("m114");

				if (!string.IsNullOrEmpty(message))
				{
					message = message.Replace("ok", "");
					message = message.Replace(" ", "");
					SetPositions(message.Split(':'),0);
				}

				message = Com.SendCommandAndRead("m114 s1");

				if (!string.IsNullOrEmpty(message))
				{
					message = message.Replace("ok", "");
					message = message.Replace(" ", "");
					SetPositions(message.Split(':'),1);
				}
			});
		}
		public void WritePending() { AsyncRunCommand(() => { Com.WritePendingCommandsToFile(System.IO.Path.GetTempPath() + "PendingCommands.nc"); }); }

		#endregion

		#region ICommand
		public ICommand SendInfoCommand { get { return new DelegateCommand(SendInfo, CanSend); } }
		public ICommand SendAbortCommand { get { return new DelegateCommand(SendAbort, CanSend); } }
		public ICommand SendResurrectCommand { get { return new DelegateCommand(SendResurrect, CanSend); } }
		public ICommand SendClearQueue { get { return new DelegateCommand(ClearQueue, CanSend); } }
		public ICommand SendM03SpindelOnCommand { get { return new DelegateCommand(SendM03SpindelOn, CanSendSpindle); } }
		public ICommand SendM05SpindelOffCommand { get { return new DelegateCommand(SendM05SpindelOff, CanSendSpindle); } }
		public ICommand SendM07CoolandOnCommand { get { return new DelegateCommand(SendM07CoolandOn, CanSendCoolant); } }
		public ICommand SendM09CoolandOffCommand { get { return new DelegateCommand(SendM09CoolandOff, CanSendCoolant); } }
        public ICommand SendM106LaserOnCommand { get { return new DelegateCommand(SendM106LaserOn, CanSendLaser); } }
        public ICommand SendM106LaserOnMinCommand { get { return new DelegateCommand(SendM106LaserOnMin, CanSendLaser); } }
        public ICommand SendM107LaserOffCommand { get { return new DelegateCommand(SendM107LaserOff, CanSendLaser); } }
        public ICommand SendM114Command { get { return new DelegateCommand(SendM114PrintPos, CanSend); } }
		public ICommand WritePendingCommands { get { return new DelegateCommand(WritePending, CanSend); } }

		#endregion
	}
}
