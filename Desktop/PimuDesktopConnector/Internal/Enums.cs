namespace J113D.Pimu.Desktop.Connector.Internal
{
	internal enum MessageDestination : byte
	{
		None = 0,

		DeviceHandshake = 1,
		DeviceDisconnect = 2,
		DeviceFirmware = 3,
		DeviceGamepad = 4,

		DesktopHandshake = 0x81,
		DesktopDebug = 0x82,
		DesktopFirmware = 0x83,
		DesktopGamepad = 0x84
	}

	/// <summary>
	/// Packet data types for <see cref="MessageDestination.DeviceFirmware"/>
	/// </summary>
	internal enum MessageDeviceFirmwareDataType
	{
		None = 0,
		RequestConfig = 1,
		SetConfig = 2
	}

	/// <summary>
	/// Packet data types for <see cref="MessageDestination.DeviceGamepad"/>
	/// </summary>
	internal enum MessageDeviceGamepadDataType
	{
		None = 0,
		RequestColors = 1,
		SetColors = 2,
		RequestPlayerLEDs = 3,
		Input = 4,
	}

	/// <summary>
	/// Packet data types for <see cref="MessageDestination.DesktopDebug"/>
	/// </summary>
	internal enum MessageDesktopDebugDataType
	{
		None = 0,
		String = 1,
		Data = 2
	}

	/// <summary>
	/// Packet data types for <see cref="MessageDestination.DesktopFirmware"/>
	/// </summary>
	internal enum MessageDesktopFirmwareDataType
	{
		None = 0,
		CurrentConfig = 1
	}

	/// <summary>
	/// Packet data types for <see cref="MessageDestination.DesktopGamepad"/>
	/// </summary>
	internal enum MessageDesktopGamepadDataType
	{
		None = 0,
		CurrentColors = 1,
		PlayerLEDs = 2
	}

}
