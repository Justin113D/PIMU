using System;
using System.Collections.Generic;
using System.Linq;

namespace J113D.Pimu.Desktop.Connector.Internal
{
	internal static class MessageSerializer
	{
		private const ushort _maxMessageDataSize = 256;

		private const ushort _messageHeaderSize = sizeof(byte) + sizeof(byte) + sizeof(ushort);
		private const ushort _messageSize = _messageHeaderSize + _maxMessageDataSize;

		private const ushort _crcMessageHeaderSize = sizeof(uint);
		private const ushort _crcMessageSize = _crcMessageHeaderSize + _messageSize;

		private const byte _terminationCharacter = 0;
		private const byte _escapeCharacter = 0xFF;

		public class DeserializeBuffer
		{
			public readonly byte[] SerialBuffer = new byte[_crcMessageSize * 2 + 3];
			private readonly byte[] _readBuffer = new byte[_crcMessageSize];
			private ushort _readBufferPosition = 0;
			private bool _readBufferEscaped = false;


			private void AddReadByte(byte value)
			{
				_readBuffer[_readBufferPosition] = value;
				_readBufferPosition = (ushort)((_readBufferPosition + 1) % _readBuffer.Length);
			}

			private bool ProcessMessage(out Message message)
			{
				message = default;
				if (_readBufferPosition <= _crcMessageHeaderSize + _messageHeaderSize)
				{
					return false;
				}

				uint messageCrc = BitConverter.ToUInt32(_readBuffer);
				ReadOnlySpan<byte> packet = _readBuffer.AsSpan(_crcMessageHeaderSize, _readBufferPosition - _crcMessageHeaderSize);
				uint crc = CRC.CRC32(packet);

				if (crc != messageCrc)
				{
					return false;
				}

				ushort dataSize = BitConverter.ToUInt16(packet[2..]);

				if (dataSize != packet.Length - _messageHeaderSize)
				{
					return false;
				}

				message = new(
					(MessageDestination)packet[0],
					packet[1],
					packet[_messageHeaderSize..].ToArray()
				);

				return true;
			}

			public Message[]? Deserialize(int length)
			{
				List<Message> messages = [];

				for (int i = 0; i < length; i++)
				{
					byte value = SerialBuffer[i];

					if (_readBufferEscaped)
					{
						_readBufferEscaped = false;
						AddReadByte(value);
						continue;
					}

					switch (value)
					{
						case _terminationCharacter:
							if(ProcessMessage(out Message message))
							{
								messages.Add(message);
							}

							_readBufferPosition = 0;
							break;
						case _escapeCharacter:
							_readBufferEscaped = true;
							break;
						default:
							AddReadByte(value);
							break;
					}

				}

				if(messages.Count == 0)
				{
					return null;
				}

				return [.. messages];
			}
		}

		public static byte[] Serialize(Message message)
		{
			if (message.Data.Length > _maxMessageDataSize)
			{
				throw new ArgumentException("Data is too large");
			}

			byte[] crcPacketData = new byte[_crcMessageHeaderSize + _messageHeaderSize + message.Data.Length];

			crcPacketData[_crcMessageHeaderSize + 0] = (byte)message.Destination;
			crcPacketData[_crcMessageHeaderSize + 1] = message.DataType;
			BitConverter.TryWriteBytes(crcPacketData.AsSpan(_crcMessageHeaderSize + 2), (ushort)message.Data.Length);
			message.Data.CopyTo(crcPacketData.AsSpan(_crcMessageHeaderSize + _messageHeaderSize));

			uint crc = CRC.CRC32(crcPacketData.AsSpan(sizeof(uint)));
			BitConverter.TryWriteBytes(crcPacketData.AsSpan(), crc);

			int toEscapeCount = crcPacketData.Count(x => x == _terminationCharacter || x == _escapeCharacter);
			byte[] result = new byte[crcPacketData.Length + toEscapeCount + 1];
			for (int i = 0, j = 0; i < crcPacketData.Length; i++, j++)
			{
				byte value = crcPacketData[i];
				if (value == _terminationCharacter || value == _escapeCharacter)
				{
					result[j] = _escapeCharacter;
					j++;
				}

				result[j] = value;
			}

			return result;
		}

	}
}
