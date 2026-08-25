using J113D.Pimu.Desktop.Connector.Internal;
using System;

namespace J113D.Pimu.Desktop.Connector.Event
{
	public class ReceivedGamepadPlayerLEDsEventArgs : EventArgs
	{
		public bool[] LEDsEnabled { get; }
		public bool Flashing { get; }

		private ReceivedGamepadPlayerLEDsEventArgs(
			bool[] ledsEnabled,
			bool flashing)
		{
			LEDsEnabled = ledsEnabled;
			Flashing = flashing;
		}

		internal static ReceivedGamepadPlayerLEDsEventArgs? DeserializeMessage(Message message)
		{
			if (message.Destination != MessageDestination.DesktopGamepad
				|| message.DataType != (byte)MessageDesktopGamepadDataType.PlayerLEDs
				|| message.Data.Length < 2)
			{
				return null;
			}

			byte mask = message.Data[0];
			bool[] ledsEnabled = [
				(mask & 0x01) != 0,
				(mask & 0x02) != 0,
				(mask & 0x04) != 0,
				(mask & 0x08) != 0,
			];
			bool flashing = message.Data[1] != 0;

			return new(ledsEnabled, flashing);
		}
	}

	public delegate void ReceivedGamepadPlayerLEDsEventHandler(string port, ReceivedGamepadPlayerLEDsEventArgs args);
}
