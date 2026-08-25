using System;

namespace J113D.Pimu.Desktop.Connector.Event
{
	public class DisconnectedEventArgs : EventArgs
	{
		internal DisconnectedEventArgs() { }

	}

	public delegate void DisconnectedEventHandler(string port, DisconnectedEventArgs args);
}
