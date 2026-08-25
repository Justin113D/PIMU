namespace J113D.Pimu.Desktop.Connector.Internal
{
	internal readonly struct Message
	{
		public readonly MessageDestination Destination;
		public readonly byte DataType;
		public readonly byte[] Data;

		public Message(MessageDestination destination, byte dataType, byte[] data)
		{
			Destination = destination;
			DataType = dataType;
			Data = data;
		}
	}
}
