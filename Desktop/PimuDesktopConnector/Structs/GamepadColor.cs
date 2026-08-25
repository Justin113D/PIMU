using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	public readonly struct GamepadColor
	{
		public readonly byte Red { get; init; }

		public readonly byte Green { get; init; }

		public readonly byte Blue { get; init; }

		public GamepadColor(byte red, byte green, byte blue)
		{
			Red = red;
			Green = green;
			Blue = blue;
		}
	}

	public readonly struct GamepadColors
	{
		public GamepadColor BodyColor { get; init; }
		public GamepadColor ButtonsColor { get; init; }
		public GamepadColor HighlightColor { get; init; }
		public GamepadColor GripColor { get; init; }

		internal static GamepadColors FromBytes(ReadOnlySpan<byte> bytes)
		{
			GamepadColor[] colors = new GamepadColor[4];
			for (int i = 0; i < colors.Length; i++)
			{
				colors[i] = new()
				{
					Red = bytes[i * 3],
					Green = bytes[i * 3 + 1],
					Blue = bytes[i * 3 + 2],
				};
			}

			return new()
			{
				BodyColor = colors[0],
				ButtonsColor = colors[1],
				HighlightColor = colors[2],
				GripColor = colors[3]
			};
		}

		internal readonly byte[] ToBytes()
		{
			byte[] result = new byte[4 * 3];

			GamepadColor[] colors = [
				BodyColor,
				ButtonsColor,
				HighlightColor,
				GripColor,
			];

			for (int i = 0; i < colors.Length; i++)
			{
				GamepadColor color = colors[i];
				result[i * 3] = color.Red;
				result[i * 3 + 1] = color.Green;
				result[i * 3 + 2] = color.Blue;
			}

			return result;
		}
	}
}
