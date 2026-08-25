using Godot;
using J113D.Pimu.Desktop.Connector.Structs;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.App.UI.Window
{
	public partial class GamepadColorWindowController : Godot.Window
	{
		private GamepadConnector? _connector;

		private Task? _savingTask;
		private bool IsSaving => _savingTask?.Status == TaskStatus.Running;

		private int _selectedColorSlot = -1;

		private GamepadColors _savingColors;

		#region Exports

		[Export]
		Godot.Collections.Array<Button>? ColorButtons { get; set; }

		[Export]
		Godot.Collections.Array<TextureRect>? IconTextures { get; set; }

		[Export]
		private ColorPicker? ColorPicker { get; set; }

		[Export]
		private AcceptDialog? DialogSaved { get; set; }

		[Export]
		private BusyIndicatorController? BusyIndicator { get; set; }

		#endregion

		private static Color FromGamepadColor(GamepadColor color)
		{
			return new()
			{
				R8 = color.Red,
				G8 = color.Green,
				B8 = color.Blue,
				A8 = byte.MaxValue
			};
		}

		private static GamepadColor ToGamepadColor(Color color)
		{
			return new()
			{
				Red = (byte)color.R8,
				Green = (byte)color.G8,
				Blue = (byte)color.B8,
			};
		}


		public override void _Ready()
		{
			base._Ready();
			CloseRequested += OnCloseRequested;
		}

		public void Open(GamepadConnector connector)
		{
			_connector = connector;
			BusyIndicator!.Visible = false;

			UpdateColor(0, FromGamepadColor(connector.Colors.GripColor));
			UpdateColor(1, FromGamepadColor(connector.Colors.HighlightColor));
			UpdateColor(2, FromGamepadColor(connector.Colors.BodyColor));
			UpdateColor(3, FromGamepadColor(connector.Colors.ButtonsColor));

			SelectSlot(0);
			Show();
		}

		private void OnCloseRequested()
		{
			if (!IsSaving)
			{
				Visible = false;
			}
		}


		private void UpdateColor(int index, Color color)
		{
			ColorButtons![index].Modulate = color;
			IconTextures![index].Modulate = color;
		}

		public void SelectSlot(int index)
		{
			if(_selectedColorSlot == index)
			{
				return;
			}

			_selectedColorSlot = index;
			ColorPicker!.Color = ColorButtons![index].Modulate;
		}

		private void OnColorChanged(Color color)
		{
			UpdateColor(_selectedColorSlot, color);
		}

		public void Save()
		{
			if (IsSaving)
			{
				return;
			}

			BusyIndicator!.Visible = true;

			_savingColors = new()
			{
				GripColor = ToGamepadColor(ColorButtons![0].Modulate),
				HighlightColor = ToGamepadColor(ColorButtons![1].Modulate),
				BodyColor = ToGamepadColor(ColorButtons![2].Modulate),
				ButtonsColor = ToGamepadColor(ColorButtons![3].Modulate),
			};

			_savingTask = Task.Run(SaveAsync);
		}

		private async void SaveAsync()
		{
			await _connector!.SetColors(_savingColors);
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
