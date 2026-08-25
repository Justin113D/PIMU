using Godot;

namespace J113D.Pimu.Desktop.App.Input
{
	public static class GamepadActions
	{
		public static StringName A { get; } = "Gamepad_A";
		public static StringName B { get; } = "Gamepad_B";
		public static StringName X { get; } = "Gamepad_X";
		public static StringName Y { get; } = "Gamepad_Y";
		public static StringName DPadUp { get; } = "Gamepad_DPad_Up";
		public static StringName DPadRight { get; } = "Gamepad_DPad_Right";
		public static StringName DPadDown { get; } = "Gamepad_DPad_Down";
		public static StringName DPadLeft { get; } = "Gamepad_DPad_Left";
		public static StringName L { get; } = "Gamepad_L";
		public static StringName ZL { get; } = "Gamepad_ZL";
		public static StringName GL { get; } = "Gamepad_GL";
		public static StringName R { get; } = "Gamepad_R";
		public static StringName ZR { get; } = "Gamepad_ZR";
		public static StringName GR { get; } = "Gamepad_GR";
		public static StringName Plus { get; } = "Gamepad_Plus";
		public static StringName Minus { get; } = "Gamepad_Minus";
		public static StringName Home { get; } = "Gamepad_Home";
		public static StringName Capture { get; } = "Gamepad_Capture";
		public static StringName Chat { get; } = "Gamepad_Chat";
		public static StringName LeftStickPress { get; } = "Gamepad_LeftStick_Press";
		public static StringName LeftStickUp { get; } = "Gamepad_LeftStick_Up";
		public static StringName LeftStickRight { get; } = "Gamepad_LeftStick_Right";
		public static StringName LeftStickDown { get; } = "Gamepad_LeftStick_Down";
		public static StringName LeftStickLeft { get; } = "Gamepad_LeftStick_Left";
		public static StringName LeftStickReduce { get; } = "Gamepad_LeftStick_Reduce";
		public static StringName RightStickPress { get; } = "Gamepad_RightStick_Press";
		public static StringName RightStickUp { get; } = "Gamepad_RightStick_Up";
		public static StringName RightStickRight { get; } = "Gamepad_RightStick_Right";
		public static StringName RightStickDown { get; } = "Gamepad_RightStick_Down";
		public static StringName RightStickLeft { get; } = "Gamepad_RightStick_Left";
		public static StringName RightStickReduce { get; } = "Gamepad_RightStick_Reduce";

		public static readonly StringName[] All = [
			A,
			B,
			X,
			Y,
			DPadUp,
			DPadRight,
			DPadDown,
			DPadLeft,
			L,
			ZL,
			GL,
			R,
			ZR,
			GR,
			Plus,
			Minus,
			Home,
			Capture,
			Chat,
			LeftStickPress,
			LeftStickUp,
			LeftStickRight,
			LeftStickDown,
			LeftStickLeft,
			LeftStickReduce,
			RightStickPress,
			RightStickUp,
			RightStickRight,
			RightStickDown,
			RightStickLeft,
			RightStickReduce,
		];
	}
}
