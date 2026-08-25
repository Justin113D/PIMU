using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	public enum FirmwareConfigDebugMode : byte
	{
		None,
		UARTOnly,
		Connector
	}

	[Flags]
	public enum FirmwareConfigDebugFeature : byte
	{
		Bluetooth = 0x01,
		USB = 0x02,
		Connector = 0x04,
		Gamepad = 0x08
	}

	public readonly struct FirmwareConfig
	{
		public FirmwareConfigDebugMode DebugMode { get; init; }
		public FirmwareConfigDebugFeature DebugFeatures { get; init; }
		public bool BluetoothDisabled { get; init; }

		internal static FirmwareConfig FromBytes(byte[] data)
		{
			return new()
			{
				DebugMode = (FirmwareConfigDebugMode)data[0],
				DebugFeatures = (FirmwareConfigDebugFeature)data[1],
				BluetoothDisabled = data[2] != 0
			};
		}

		internal readonly byte[] ToBytes()
		{
			return [
				(byte)DebugMode,
				(byte)DebugFeatures,
				(byte)(BluetoothDisabled ? 1 : 0)
			];
		}
	}
}
