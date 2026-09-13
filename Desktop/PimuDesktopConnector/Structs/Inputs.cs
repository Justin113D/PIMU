using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	public readonly struct Inputs
	{
		[Flags]
		public enum InputFlags : byte
		{
			Buttons = 0x01,
			StickLeft = 0x02,
			StickRight = 0x04,
			Gyro = 0x08
		}

		public InputFlags Flags { get; init; }

		public InputButtons Buttons { get; init; }

		public float StickLeftX { get; init; }
		public float StickLeftY { get; init; }

		public float StickRightX { get; init; }
		public float StickRightY { get; init; }

		public float QuaternionW { get; init; }
		public float QuaternionX { get; init; }
		public float QuaternionY { get; init; }
		public float QuaternionZ { get; init; }

		private static void Pack12BitVector(float x, float y, Span<byte> destination)
		{
			ushort xp = (ushort)((x * 0.5 + 0.5) * 0xFFF);
			ushort yp = (ushort)((y * 0.5 + 0.5) * 0xFFF);

			destination[0] = (byte)(xp & 0xFF);
			destination[1] = (byte)(xp >> 8 | (yp & 0xF) << 4);
			destination[2] = (byte)(yp >> 4);
		}

		private static void PackQuaternion(float[] quaternion, Span<byte> destination)
		{
			byte omittedIndex = 0;
			float abs_quat_max = float.Abs(quaternion[0]);

			for (byte i = 1; i < 4; i++)
			{
				float abs_quat = float.Abs(quaternion[i]);
				if (abs_quat > abs_quat_max)
				{
					abs_quat_max = abs_quat;
					omittedIndex = i;
				}
			}

			float quat_factor = 1 / quaternion[omittedIndex];
			byte extra = 0; // II332211

			for (int i = 0; i < 4; i++)
			{
				if (i == omittedIndex)
				{
					continue;
				}

				uint value = (uint)(((quaternion[i] * quat_factor) * 0.5 + 0.5) * 0xFFFFFFFFu);

				int index = (i + 3 - omittedIndex) % 4;
				destination[index * 2] = (byte)((value >> 16) & 0xFF);
				destination[index * 2 + 1] = (byte)(value >> 24);

				extra |= (byte)(((value >> 14) & 0x3) << (index * 2));
			}

			destination[6] = (byte)(extra | (omittedIndex << 6));
		}

		internal readonly byte[] ToBytes()
		{
			byte[] result = new byte[16];
			Span<byte> span = result.AsSpan();
			int size = 0;

			if(Flags.HasFlag(InputFlags.Buttons))
			{
				span[0] = (byte)(((uint)Buttons) & 0xFF);
				span[1] = (byte)((((uint)Buttons) >> 8) & 0xFF);
				span[2] = (byte)((((uint)Buttons) >> 16) & 0xFF);

				size += 3;
				span = span[3..];
			}

			if (Flags.HasFlag(InputFlags.StickLeft))
			{
				Pack12BitVector(StickLeftX, StickLeftY, span);

				size += 3;
				span = span[3..];
			}

			if (Flags.HasFlag(InputFlags.StickRight))
			{
				Pack12BitVector(StickRightX, StickRightY, span);

				size += 3;
				span = span[3..];
			}

			if (Flags.HasFlag(InputFlags.Gyro))
			{
				PackQuaternion([QuaternionW, QuaternionX, QuaternionY, QuaternionZ], span);

				size += 7;
				//span = span[7..];
			}

			return result[..size];
		}
	}
}
