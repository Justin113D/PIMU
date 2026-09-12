using Godot;
using J113D.Pimu.Desktop.Connector.Structs;
using System;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.App.UI.Window
{
	public partial class FirmwareConfigWindowController : Godot.Window
	{
		private GamepadConnector? _connector;

		private Task? _savingTask;
		private bool IsSaving => _savingTask?.Status == TaskStatus.Running;

		private FirmwareConfig _savingConfig;

		#region Exports

		[Export]
		private CheckBox? CheckBoxDisableBluetooth { get; set; }

		[Export]
		private OptionButton? OptionButtonDebugMode { get; set; }

		[Export]
		private CheckBox? CheckBoxDFBluetooth { get; set; }

		[Export]
		private CheckBox? CheckBoxDFUSB { get; set; }

		[Export]
		private CheckBox? CheckBoxDFConnector { get; set; }

		[Export]
		private CheckBox? CheckBoxDFGamepad { get; set; }

		[Export]
		private AcceptDialog? DialogSaved { get; set; }

		[Export]
		private BusyIndicatorController? BusyIndicator { get; set; }

		#endregion

		public override void _Ready()
		{
			base._Ready();
			CloseRequested += OnCloseRequested;

			foreach (FirmwareConfigDebugMode mode in Enum.GetValues<FirmwareConfigDebugMode>())
			{
				OptionButtonDebugMode!.AddItem(mode.ToString(), (int)mode);
			}

			OptionButtonDebugMode!.Selected = 0;
		}

		public void Open(GamepadConnector connector)
		{
			_connector = connector;
			BusyIndicator!.Visible = false;

			FirmwareConfig config = connector.Config;

			CheckBoxDisableBluetooth!.ButtonPressed = config.BluetoothDisabled;
			OptionButtonDebugMode!.Selected = (int)config.DebugMode;
			CheckBoxDFBluetooth!.ButtonPressed = config.DebugFeatures.HasFlag(FirmwareConfigDebugFeature.Bluetooth);
			CheckBoxDFUSB!.ButtonPressed = config.DebugFeatures.HasFlag(FirmwareConfigDebugFeature.USB);
			CheckBoxDFConnector!.ButtonPressed = config.DebugFeatures.HasFlag(FirmwareConfigDebugFeature.Connector);
			CheckBoxDFGamepad!.ButtonPressed = config.DebugFeatures.HasFlag(FirmwareConfigDebugFeature.Gamepad);

			Show();
		}

		private void OnCloseRequested()
		{
			if (!IsSaving)
			{
				Visible = false;
			}
		}

		public void Save()
		{
			if (IsSaving)
			{
				return;
			}

			BusyIndicator!.Visible = true;

			_savingConfig = new()
			{
				BluetoothDisabled = CheckBoxDisableBluetooth!.ButtonPressed,
				DebugMode = (FirmwareConfigDebugMode)OptionButtonDebugMode!.Selected,
				DebugFeatures =
					(CheckBoxDFBluetooth!.ButtonPressed ? FirmwareConfigDebugFeature.Bluetooth : default)
					| (CheckBoxDFUSB!.ButtonPressed ? FirmwareConfigDebugFeature.USB : default)
					| (CheckBoxDFConnector!.ButtonPressed ? FirmwareConfigDebugFeature.Connector : default)
					| (CheckBoxDFGamepad!.ButtonPressed ? FirmwareConfigDebugFeature.Gamepad : default)
			};

			_savingTask = Task.Run(SaveAsync);
		}

		private async void SaveAsync()
		{
			await _connector!.SetConfig(_savingConfig);
			CallDeferred(MethodName.SaveFinish);
		}

		private void SaveFinish()
		{
			_savingTask = null;
			BusyIndicator!.Visible = false;
			DialogSaved!.Show();
		}
	}
}
