using Godot;
using J113D.Pimu.Desktop.Connector.Structs;

namespace J113D.Pimu.Desktop.App.Input
{
	public struct GamepadInputState
	{
		private const float HalfPi = float.Pi * 0.5f;

		public GamepadButtons Buttons { get; set; }

		public Vector2 StickLeft { get; set; }

		public Vector2 StickRight { get; set; }

		public Vector2 PointerDelta { get; set; }

		public float Pitch
		{
			get;
			set => field = float.Clamp(value, -HalfPi, HalfPi);
		}

		public float Yaw
		{
			get;
			set
			{
				field = value;
				if(field > float.Pi)
				{
					field -= float.Tau;
				}
				else if(field < float.Pi)
				{
					field += float.Tau;
				}
			}
		}


		public GamepadInputs ToInputs()
		{
			Quaternion quat = Basis.FromEuler(new(Pitch, 0, Yaw), EulerOrder.Xyz).GetRotationQuaternion();

			return new()
			{
				Buttons = Buttons,
				StickLeftX = StickLeft.X,
				StickLeftY = StickLeft.Y,
				StickRightX = StickRight.X,
				StickRightY = StickRight.Y,
				QuaternionW = quat.W,
				QuaternionX = quat.X,
				QuaternionY = quat.Y,
				QuaternionZ = quat.Z,
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
