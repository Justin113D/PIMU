using Godot;
using J113D.Pimu.Desktop.Connector.Structs;
using System;

namespace J113D.Pimu.Desktop.App.Input
{
	public struct GamepadInputState
	{
		private const float HalfPi = float.Pi * 0.5f;

		public GamepadButtons Buttons { get; set; }

		public Vector2 StickLeft { get; set; }

		public Vector2 StickRight { get; set; }

		public Vector2 PointerDelta { get; set; }

		public bool ForceRightStickUp { get; set; }

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

		public readonly Quaternion Quaternion => Basis.FromEuler(new(Pitch, Yaw, 0), EulerOrder.Zyx).GetRotationQuaternion();

		public GamepadInputs ToInputs()
		{
			Quaternion quat = Quaternion * new Quaternion(Vector3.Right, float.Pi * -0.5f);

			return new()
			{
				Buttons = Buttons,
				StickLeftX = StickLeft.X,
				StickLeftY = StickLeft.Y,
				StickRightX = ForceRightStickUp ? 0 : StickRight.X,
				StickRightY = ForceRightStickUp ? 1 : StickRight.Y,
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
