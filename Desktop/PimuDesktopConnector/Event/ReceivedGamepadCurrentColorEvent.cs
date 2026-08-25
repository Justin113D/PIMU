using J113D.Pimu.Desktop.Connector.Internal;
using J113D.Pimu.Desktop.Connector.Structs;
using System;

namespace J113D.Pimu.Desktop.Connector.Event
{
	public class ReceivedGamepadCurrentColorsEventArgs : EventArgs
	{
		public GamepadColors Colors { get; }

		private ReceivedGamepadCurrentColorsEventArgs(GamepadColors colors)
		{
			Colors = colors;
		}

		internal static ReceivedGamepadCurrentColorsEventArgs? DeserializeMessage(Message message)
		{
			if (message.Destination != MessageDestination.DesktopGamepad
				|| message.DataType != (byte)MessageDesktopGamepadDataType.CurrentColors
				|| message.Data.Length < 4 * 3)
			{
				return null;
			}

			return new(GamepadColors.FromBytes(message.Data));
		}
	}

	public delegate void ReceivedGamepadCurrentColorsEventHandler(string port, ReceivedGamepadCurrentColorsEventArgs args);
}
