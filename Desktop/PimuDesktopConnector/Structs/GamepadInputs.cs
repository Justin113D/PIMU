using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	public readonly struct GamepadInputs
	{
		public GamepadButtons Buttons { get; init; }

		public float StickLeftX { get; init; }
		public float StickLeftY { get; init; }
		public float StickRightX { get; init; }
		public float StickRightY { get; init; }
		public float AccelX { get; init; }
		public float AccelY { get; init; }
		public float AccelZ { get; init; }
		public float GyroZ { get; init; }

		internal readonly byte[] ToBytes()
		{
			byte[] result = new byte[36];
			Span<byte> span = result.AsSpan();

			BitConverter.TryWriteBytes(span, (uint)Buttons);
			BitConverter.TryWriteBytes(span[4..], StickLeftX);
			BitConverter.TryWriteBytes(span[8..], StickLeftY);
			BitConverter.TryWriteBytes(span[12..], StickRightX);
			BitConverter.TryWriteBytes(span[16..], StickRightY);
			BitConverter.TryWriteBytes(span[20..], AccelX);
			BitConverter.TryWriteBytes(span[24..], AccelY);
			BitConverter.TryWriteBytes(span[28..], AccelZ);
			BitConverter.TryWriteBytes(span[32..], GyroZ);

			return result;
		}
	}
}
