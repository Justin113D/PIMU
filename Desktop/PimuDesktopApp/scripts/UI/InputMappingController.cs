using Godot;
using Godot.Collections;
using J113D.Pimu.Desktop.App.scripts.Input.Config;

namespace J113D.Pimu.Desktop.App.UI
{
	public partial class InputMappingController : Control
	{
		[Export]
		private Texture2D? KeyboardIcon { get; set; }

		[Export]
		private Texture2D? MouseIcon { get; set; }

		[Export]
		private Texture2D? RecordIcon { get; set; }

		[Export]
		private Texture2D? RecordingIcon { get; set; }

		[Export]
		private Label? LabelMapping { get; set; }

		[Export]
		private Label? LabelInput { get; set; }

		[Export]
		private TextureRect? TextureRectInputIcon { get; set; }

		[Export]
		private Button? ButtonRecord { get; set; }

		[Signal]
		public delegate void StartRecordingEventHandler(InputMappingController controller);

		public string? Label { get; set; }
		public StringName? Action { get; set; }

		public override void _Ready()
		{
			base._Ready();
			LabelMapping!.Text = Label;
			Refresh();
		}

		public void Refresh()
		{
			Array<InputEvent> events = InputMap.ActionGetEvents(Action);
			if(events.Count == 0 || (events[0] is not InputEventKey and not InputEventMouseButton))
			{
				TextureRectInputIcon!.Visible = false;
				LabelInput!.Visible = false;
			}
			else
			{
				TextureRectInputIcon!.Visible = true;
				LabelInput!.Visible = true;

				if(events[0] is InputEventKey keyEvent)
				{
					TextureRectInputIcon!.Texture = KeyboardIcon;
					LabelInput!.Text = OS.GetKeycodeString(DisplayServer.KeyboardGetKeycodeFromPhysical(keyEvent.PhysicalKeycode));
					if(keyEvent.Location != KeyLocation.Unspecified)
					{
						LabelInput.Text += $" ({keyEvent.AsTextLocation()})";
					}
				}
				else if (events[0] is InputEventMouseButton mouseButtonEvent)
				{
					TextureRectInputIcon!.Texture = MouseIcon;
					LabelInput!.Text = mouseButtonEvent.ButtonIndex.ToString();
				}
			}
		}

		public void Record()
		{
			ButtonRecord!.Icon = RecordingIcon;
			EmitSignalStartRecording(this);
		}

		public void StopRecording(InputEvent? newEvent)
		{
			ButtonRecord!.Icon = RecordIcon;

			if(newEvent != null)
			{
				InputMappingHelper.SetAction(Action!, newEvent);
				Refresh();
			}
		}

		public void Clear()
		{
			InputMappingHelper.ClearAction(Action!);
			Refresh();
		}

		public void Reset()
		{
			InputMappingHelper.ResetAction(Action!);
			Refresh();
		}
	}
}
