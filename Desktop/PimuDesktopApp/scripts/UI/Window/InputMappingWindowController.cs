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
		private PackedScene? InputMappingRow { get; set; }

		[Export]
		private Control? ControlRecordOverlay { get; set; }

		[Export]
		private Label? LabelRecordAction { get; set; }


		[Export]
		private LineEdit? HMouseSensitivityEdit { get; set; }

		[Export]
		private Slider? HMouseSensitivitySlider { get; set; }

		[Export]
		private LineEdit? VMouseSensitivityEdit { get; set; }

		[Export]
		private Slider? VMouseSensitivitySlider { get; set; }


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

			HMouseSensitivitySlider!.ValueChanged += HorizontalSliderChanged;
			HMouseSensitivityEdit!.TextSubmitted += HorizonalTextChanged;
			HMouseSensitivityEdit!.FocusExited += UpdateSensitivies;

			VMouseSensitivitySlider!.ValueChanged += VerticalSliderChanged;
			VMouseSensitivityEdit!.TextSubmitted += VerticalTextChanged;
			VMouseSensitivityEdit!.FocusExited += UpdateSensitivies;

			UpdateSensitivies();
		}

		private void UpdateSensitivies()
		{
			HMouseSensitivitySlider!.SetValueNoSignal(InputMappingConfig.MouseSensitivity.X);
			HMouseSensitivityEdit!.Text = InputMappingConfig.MouseSensitivity.X.ToString("F3", System.Globalization.CultureInfo.InvariantCulture);
			VMouseSensitivitySlider!.SetValueNoSignal(InputMappingConfig.MouseSensitivity.Y);
			VMouseSensitivityEdit!.Text = InputMappingConfig.MouseSensitivity.Y.ToString("F3", System.Globalization.CultureInfo.InvariantCulture);
		}

		private void HorizonalTextChanged(string newText)
		{
			if(float.TryParse(newText, System.Globalization.CultureInfo.InvariantCulture, out float value))
			{
				InputMappingConfig.MouseSensitivity = new((float)value, InputMappingConfig.MouseSensitivity.Y);
				UpdateSensitivies();
			}
		}

		private void HorizontalSliderChanged(double value)
		{
			InputMappingConfig.MouseSensitivity = new((float)value, InputMappingConfig.MouseSensitivity.Y);
			UpdateSensitivies();
		}

		private void VerticalTextChanged(string newText)
		{
			if (float.TryParse(newText, System.Globalization.CultureInfo.InvariantCulture, out float value))
			{
				InputMappingConfig.MouseSensitivity = new(InputMappingConfig.MouseSensitivity.X, (float)value);
				UpdateSensitivies();
			}
		}

		private void VerticalSliderChanged(double value)
		{
			InputMappingConfig.MouseSensitivity = new(InputMappingConfig.MouseSensitivity.X, (float)value);
			UpdateSensitivies();
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

			InputMappingConfig.SaveToFile();
			Visible = false;
		}

		public void Copy()
		{
			string text = InputMappingConfig.ToConfigFile().EncodeToText();
			DisplayServer.ClipboardSet(text);
		}

		public void Paste()
		{
			string text = DisplayServer.ClipboardGet();
			ConfigFile config = new();
			if(config.Parse(text) == Error.Ok)
			{
				InputMappingConfig.Load(config);
				RefreshAll();
			}
		}

		public void ResetAll()
		{
			InputMappingConfig.ResetAll();
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
			UpdateSensitivies();
		}
	
		
	}
}
