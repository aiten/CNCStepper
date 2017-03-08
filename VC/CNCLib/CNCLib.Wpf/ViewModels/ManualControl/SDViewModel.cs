﻿////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

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

using System.Collections.Generic;
using System.Windows.Input;
using Framework.Wpf.Helpers;
using System.IO;
using System;
using CNCLib.Wpf.Helpers;

namespace CNCLib.Wpf.ViewModels.ManualControl
{
    public class SDViewModel : DetailViewModel
	{
		public SDViewModel(IManualControlViewModel vm) : base(vm)
		{
		}

		#region Properties

		private string _fileName = @"%USERPROFILE%\Documents\test.GCode";
		public string FileName
		{
			get { return _fileName; }
			set { SetProperty(ref _fileName, value); }
		}

		private string _SDFileName = @"auto0.g";
		public string SDFileName
		{
			get { return _SDFileName; }
			set { SetProperty(ref _SDFileName, value); }
		}

		#endregion

		#region Commands / CanCommands
		public void SendM20File() { RunAndUpdate(() => { new MachineGCodeHelper().QueueCommand("m20"); }); }
		public void SendM24File() { SendM24File(SDFileName); }
		public void SendM24File(string filename)
		{
			RunAndUpdate(() =>
			{
				var com = new MachineGCodeHelper();
				com.QueueCommand("m23 " + filename);
				com.QueueCommand("m24");
			});
		}

		public void SendM28File() { SendM28File(FileName, SDFileName); }
		public void SendM28File(string filename, string sDFileName)
		{
			RunInNewTask(() =>
			{
				using (StreamReader sr = new StreamReader(filename))
				{
					bool savefileinresponse = false;
					var checkresponse = new Framework.Arduino.ArduinoSerialCommunication.CommandEventHandler((obj, e) =>
					{
						savefileinresponse = e.Info.Contains(sDFileName);
					});
                    var com = new MachineGCodeHelper();
                    var prefix = com.GetCommandPrefix();

                    Com.ReplyUnknown += checkresponse;
					Com.SendCommand(prefix+"m28 " + sDFileName);
					Com.ReplyUnknown -= checkresponse;
					if (savefileinresponse)
					{
						string line;
                        List<string> lines = new List<string>();
                        while ((line = sr.ReadLine()) != null)
                        {
                            lines.Add(prefix + line);
                        }
                        Com.SendCommandsAsync(lines.ToArray()).GetAwaiter().GetResult();
	
						bool filesavednresponse = false;
						checkresponse = new Framework.Arduino.ArduinoSerialCommunication.CommandEventHandler((obj, e) =>
						{
							filesavednresponse = e.Info.Contains("Done");
						});
						Com.ReplyUnknown += checkresponse;
						Com.SendCommand(prefix + "m29");
						Com.ReplyUnknown -= checkresponse;
					}
				}
			});
		}
		public void SendM30File() { SendM30File(SDFileName); }
		public void SendM30File(string filename)
		{
			RunAndUpdate(() =>
			{
				new MachineGCodeHelper().QueueCommand("m30 " + filename);
			});
		}
		public void SendFileDirect() { RunAndUpdate(async () => { await Com.SendFileAsync(FileName); }); }

		public void AddToFile()
		{
			RunAndUpdate(async () =>
			{
				string message = await Com.SendCommandAndReadOKReplyAsync("m114");
				if (!string.IsNullOrEmpty(message))
				{
					message = message.Replace("ok", "");
					message = message.Replace(" ", "");
					string[] positions = message.Split(':');

					using (StreamWriter sw = new StreamWriter(Environment.ExpandEnvironmentVariables(FileName), true))
					{
						sw.Write("g1");
						if (positions.Length >= 1) sw.Write("X"+positions[0]);
						if (positions.Length >= 2) sw.Write("Y"+positions[1]);
						if (positions.Length >= 3) sw.Write("Z"+positions[2]);
						if (positions.Length >= 4) sw.Write("A"+positions[3]);
						if (positions.Length >= 5) sw.Write("B"+positions[4]);
						if (positions.Length >= 6) sw.Write("C"+positions[5]);
						sw.WriteLine();
					}
				}
			});
		}
		public bool CanSendSDCommand()
		{
			return CanSend() && Global.Instance.Machine.SDSupport;
        }

		public bool CanSendFileNameCommand()
		{
			return Connected && File.Exists(FileName);
		}
		public bool CanSendSDFileNameCommand()
		{
			return CanSendSDCommand() && !string.IsNullOrEmpty(SDFileName);
		}
		public bool CanSendFileNameAndSDFileNameCommand()
		{
			return CanSendSDCommand() && CanSendSDFileNameCommand() && CanSendFileNameCommand();
		}


		#endregion

		#region ICommand
		public ICommand SendM20FileCommand => new DelegateCommand(SendM20File, CanSendSDCommand);
		public ICommand SendM24FileCommand => new DelegateCommand(SendM24File, CanSendSDFileNameCommand);
		public ICommand SendM28FileCommand => new DelegateCommand(SendM28File, CanSendFileNameAndSDFileNameCommand);
		public ICommand SendM30FileCommand => new DelegateCommand(SendM30File, CanSendSDFileNameCommand);
		public ICommand SendFileDirectCommand => new DelegateCommand(SendFileDirect, CanSendFileNameCommand);
		public ICommand AddToFileCommand => new DelegateCommand(AddToFile, CanSendFileNameCommand);

		#endregion
	}
}
