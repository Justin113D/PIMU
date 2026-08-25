using J113D.Pimu.Desktop.Connector.Internal;
using J113D.Pimu.Desktop.Connector.Structs;
using System;

namespace J113D.Pimu.Desktop.Connector.Event
{
	public class ReceivedFirmwareCurrentConfigEventArgs : EventArgs
	{
		public FirmwareConfig Config { get; }

		private ReceivedFirmwareCurrentConfigEventArgs(FirmwareConfig config)
		{
			Config = config;
		}

		internal static ReceivedFirmwareCurrentConfigEventArgs? DeserializeMessage(Message message)
		{
			if (message.Destination != MessageDestination.DesktopFirmware
				|| message.DataType != (byte)MessageDesktopFirmwareDataType.CurrentConfig
				|| message.Data.Length < 3)
			{
				return null;
			}

			return new(FirmwareConfig.FromBytes(message.Data));
		}
	}

	public delegate void ReceivedFirmwareCurrentConfigEventHandler(string port, ReceivedFirmwareCurrentConfigEventArgs args);
}
