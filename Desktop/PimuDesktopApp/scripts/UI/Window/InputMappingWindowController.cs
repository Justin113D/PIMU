using Godot;
using J113D.Pimu.Desktop.App.Input.Mapping;
using J113D.Pimu.Desktop.App.scripts.Input.Config;
using System.Collections.Generic;
using System.Runtime.InteropServices.Marshalling;

namespace J113D.Pimu.Desktop.App.UI.Window
{
	public partial class InputMappingWindowController : Godot.Window
	{
		private InputMappingController[] _inputMappingControllers = [];
		private InputMappingController? _recordingController;


		[Export]
		private InputMapping? InputMapping { get; set; }

		[Export]
		private Control? InputMappingContainer { get; set; }

		[Export]
		public PackedScene? InputMappingRow { get; set; }

		[Export]
		public Control? ControlRecordOverlay { get; set; }

		[Export]
		public Label? LabelRecordAction { get; set; }


		public override void _Ready()
		{
			base._Ready();

			CloseRequested += OnCloseRequested;

			StringName categoryLabelThemeVariation = "HeaderMedium";

			List<InputMappingController> controllers = [];

			foreach (InputMappingCategory category in InputMapping!.ActionCategories!)
			{
				InputMappingContainer!.AddChild(new Label()
				{
					Text = category.Label,
					ThemeTypeVariation = categoryLabelThemeVariation
				});

				foreach (InputMappingAction action in category.Actions!)
				{
					InputMappingController controller = InputMappingRow!.Instantiate<InputMappingController>();
					controller.Label = action.Label;
					controller.Action = action.Action;
					controller.Connect(InputMappingController.SignalName.StartRecording, new(this, MethodName.OnStartRecording));

					InputMappingContainer!.AddChild(controller);
					controllers.Add(controller);
				}
			}

			_inputMappingControllers = [.. controllers];
		}

		public override void _Input(InputEvent @event)
		{
			base._Input(@event);

			if(_recordingController == null)
			{
				return;
			}

			if(@event is InputEventKey keyEvent
				&& keyEvent.Keycode == Key.Escape
				&& keyEvent.Pressed)
			{
				StopRecording(null);
			}
			else if(@event is InputEventKey or InputEventMouseButton
					&& @event.IsPressed())
			{
				StopRecording(@event);
			}
			else
			{
				return;
			}

			GetViewport().SetInputAsHandled();
		}

		private void OnCloseRequested()
		{
			if(_recordingController != null)
			{
				StopRecording(null);
			}

			InputMappingHelper.SaveToFile();
			Visible = false;
		}

		public void Copy()
		{
			string text = InputMappingHelper.ToConfigFile().EncodeToText();
			DisplayServer.ClipboardSet(text);
		}

		public void Paste()
		{
			string text = DisplayServer.ClipboardGet();
			ConfigFile config = new();
			if(config.Parse(text) == Error.Ok)
			{
				InputMappingHelper.Load(config);
				RefreshAll();
			}
		}

		public void ResetAll()
		{
			InputMappingHelper.ResetAll();
			RefreshAll();
		}

		public void OnStartRecording(InputMappingController controller)
		{
			_recordingController = controller;

			ControlRecordOverlay!.Visible = true;
			LabelRecordAction!.Text = controller.Label;
		}
	
		private void StopRecording(InputEvent? @event)
		{
			_recordingController!.StopRecording(@event);
			_recordingController = null;

			ControlRecordOverlay!.Visible = false;
		}
	
		private void RefreshAll()
		{
			foreach (InputMappingController controller in _inputMappingControllers)
			{
				controller.Refresh();
			}
		}
	}
}
