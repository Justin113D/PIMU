using Godot;
using J113D.Pimu.Desktop.Connector.Structs;

namespace J113D.Pimu.Desktop.App.Input
{
	public struct GamepadInputState
	{
		public GamepadButtons Buttons { get; set; }

		public Vector2 StickLeft { get; set; }

		public Vector2 StickRight { get; set; }

		public Vector2 PointerDelta { get; set; }

		public float Pitch
		{
			get;
			set => field = float.Clamp(value, float.Pi * -0.5f, float.Pi * 0.5f);
		}

		public float YawDelta
		{
			get;
			set => field = float.Clamp(value, -2000, 2000);
		}


		public GamepadInputs ToInputs()
		{
			Vector3 accel = new Vector3(0, 0, 1).Rotated(new Vector3(1, 0, 0), Pitch);
			
			return new()
			{
				Buttons = Buttons,
				StickLeftX = StickLeft.X,
				StickLeftY = StickLeft.Y,
				StickRightX = StickRight.X,
				StickRightY = StickRight.Y,
				AccelX = accel.X,
				AccelY = accel.Y,
				AccelZ = accel.Z,
				GyroZ = YawDelta
			};
		}
	}

	public partial class GamepadInputStateChangedEvent : Resource
	{
		public GamepadInputState State { get; init; }

		public GamepadInputStateChangedEvent(GamepadInputState state)
		{
			State = state;
		}
	}
}
