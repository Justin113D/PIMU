using Godot;

namespace J113D.Pimu.Desktop.App.UI
{
    public partial class GamepadCaptureMask : TextureRect
    {
		public bool MouseOver { get; private set; }
		public bool CaptureInput { get; private set; }

		[Signal]
		public delegate void MouseOverChangedEventHandler(bool mouseOver);

		[Signal]
		public delegate void CaptureInputChangedEventHandler(bool captureInput);

		public override void _Ready()
		{
			base._Ready();
			MouseEntered += OnMouseEntered;
			MouseExited += OnMouseExited;
		}

		private void OnMouseExited()
		{
			MouseOver = false;
			EmitSignalMouseOverChanged(MouseOver);
		}

		private void OnMouseEntered()
		{
			MouseOver = true;
			EmitSignalMouseOverChanged(MouseOver);
		}

		public override void _Input(InputEvent @event)
		{
			base._Input(@event);

			if(CaptureInput
				&& @event is InputEventKey keyEvent
				&& keyEvent.Keycode == Key.Escape
				&& keyEvent.Pressed)
			{
				CaptureInput = false;
				GetViewport().SetInputAsHandled();
				EmitSignalCaptureInputChanged(CaptureInput);
			}
		}

		public override void _GuiInput(InputEvent @event)
		{
			base._GuiInput(@event);

			if (!CaptureInput
				&& @event is InputEventMouseButton mouseInput
				&& mouseInput.ButtonIndex == MouseButton.Left
				&& mouseInput.Pressed)
			{
				CaptureInput = true;
				GetViewport().SetInputAsHandled();
				EmitSignalCaptureInputChanged(CaptureInput);
			}
		}

        public override bool _HasPoint(Vector2 point)
        {
            return OpacityMask.CheckInsideOpaque(point, this);
		}
    }
}
