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
		public float QuaternionW { get; init; }
		public float QuaternionX { get; init; }
		public float QuaternionY { get; init; }
		public float QuaternionZ { get; init; }

		internal readonly byte[] ToBytes()
		{
			byte[] result = new byte[36];
			Span<byte> span = result.AsSpan();

			BitConverter.TryWriteBytes(span, (uint)Buttons);
			BitConverter.TryWriteBytes(span[4..], StickLeftX);
			BitConverter.TryWriteBytes(span[8..], StickLeftY);
			BitConverter.TryWriteBytes(span[12..], StickRightX);
			BitConverter.TryWriteBytes(span[16..], StickRightY);
			BitConverter.TryWriteBytes(span[20..], QuaternionW);
			BitConverter.TryWriteBytes(span[24..], QuaternionX);
			BitConverter.TryWriteBytes(span[28..], QuaternionY);
			BitConverter.TryWriteBytes(span[32..], QuaternionZ);

			return result;
		}
	}
}
