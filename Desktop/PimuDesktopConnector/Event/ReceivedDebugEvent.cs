using J113D.Pimu.Desktop.Connector.Internal;
using J113D.Pimu.Desktop.Connector.Structs;
using System;
using System.Text;

namespace J113D.Pimu.Desktop.Connector.Event
{
	public class ReceivedDebugEventArgs : EventArgs
	{
		public FirmwareConfigDebugFeature Features { get; }

		public string Debug { get; }

		private ReceivedDebugEventArgs(FirmwareConfigDebugFeature features, string debug)
		{
			Features = features;
			Debug = debug;
		}

		internal static ReceivedDebugEventArgs? DeserializeMessage(Message message)
		{
			if(message.Destination != MessageDestination.DesktopDebug
				|| message.Data.Length < 2) // should at least have the feature byte and one byte of actual data
			{
				return null;
			}

			string debug;

			switch ((MessageDesktopDebugDataType)message.DataType)
			{
				case MessageDesktopDebugDataType.String:
					debug = Encoding.ASCII.GetString(message.Data.AsSpan(1));
					break;
				case MessageDesktopDebugDataType.Data:
					debug = string.Empty;
					for(int i = 1; i < message.Data.Length; i++)
					{
						debug += message.Data[i].ToString("X2") + (i % 16 == 15 ? '\n' : ' ');
					}
					break;
				default:
					return null;
			}

			return new((FirmwareConfigDebugFeature)message.Data[0], debug.TrimEnd());
		}
	}

	public delegate void ReceivedDebugEventHandler(string port, ReceivedDebugEventArgs args);
}
