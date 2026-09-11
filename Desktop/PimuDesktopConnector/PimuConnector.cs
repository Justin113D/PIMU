using HidSharp;
using J113D.Pimu.Desktop.Connector.Event;
using J113D.Pimu.Desktop.Connector.Internal;
using J113D.Pimu.Desktop.Connector.Structs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.Connector
{
	public sealed class PimuConnector : IDisposable
	{
		public string Port { get; }

		private readonly SerialStream _messageStream;

		private readonly MessageSerializer.DeserializeBuffer _readBuffer;

		public event ReceivedDebugEventHandler? ReceivedDebug;
		public event ReceivedFirmwareCurrentConfigEventHandler? ReceivedFirmwareCurrentConfig;
		public event ReceivedGamepadCurrentColorsEventHandler? ReceivedGamepadCurrentColors;
		public event ReceivedGamepadPlayerLEDsEventHandler? ReceivedGamepadPlayerLEDs;

		public event DisconnectedEventHandler? Disconnected;

		private PimuConnector(string port, SerialStream messageStream)
		{
			Port = port;
			_messageStream = messageStream;
			_messageStream.BaudRate = 115200;
			_readBuffer = new();
		}


		public static (string port, string description)[] ListAvilablePorts()
		{
			List<(string, string)> result = [];

			foreach (SerialDevice device in DeviceList.Local.GetSerialDevices())
			{
				string port = device.GetFileSystemName();

				string description = string.Empty;
				try
				{
					description = device.GetProductName();
				}
				catch { }

				if(string.IsNullOrWhiteSpace(description))
				{
					description = device.GetFriendlyName();
				}

				result.Add((port, description));
			}

			return [.. result.OrderBy(x => x.Item1)];
		}

		public enum ConnectionResult
		{
			Success,
			NotFound,
			OpenFailed,
			VersionMismatch,
			HandshakeFailed,
			UnknownError
		}

		public static async Task<(ConnectionResult, PimuConnector?)> TryEstablishConnection(string port)
		{
			SerialStream? stream = null;
			PimuConnector? connector = null;
			try
			{
				if (!DeviceList.Local.TryGetSerialDevice(out SerialDevice device, port))
				{
					return (ConnectionResult.NotFound, null);
				}

				if (!device.TryOpen(out stream))
				{
					return (ConnectionResult.OpenFailed, null);
				}

				connector = new(port, stream);

				byte[] check = RandomNumberGenerator.GetBytes(16);

				// perform handshake to confirm that this is a pimu device
				Message handshake = new(
					MessageDestination.DeviceHandshake,
					Constants.PimuVersion,
					check
				);

				byte[] handshakeSerialSequence = MessageSerializer.Serialize(handshake);

				const int handshakeAttempts = 5;
				const long handshakeWaitTicks = TimeSpan.TicksPerSecond * 5;
				Message receivedHandshake = default;

				for (int i = 0; i < handshakeAttempts; i++)
				{
					await stream.WriteAsync(handshakeSerialSequence);

					long end = DateTime.UtcNow.Ticks + handshakeWaitTicks;
					while (DateTime.UtcNow.Ticks < end)
					{
						if(!connector._messageStream.CanRead)
						{
							continue;
						}

						Message[]? receivedMessages = await connector.ReadAsync();
						receivedHandshake = receivedMessages?.FirstOrDefault(x => x.Destination == MessageDestination.DesktopHandshake) ?? default;
						if(receivedHandshake.Destination != default)
						{
							break;
						}

					}

					if (receivedHandshake.Destination == default
						|| !receivedHandshake.Data.SequenceEqual(check))
					{
						continue;
					}

					if(receivedHandshake.DataType != handshake.DataType)
					{
						connector.Dispose();
						return (ConnectionResult.VersionMismatch, null);
					}
					else
					{
						// confirming the connection
						_ = Task.Run(connector.ReadAsyncTask);
						await connector.SendMessage(new(MessageDestination.DeviceHandshake, 0, []));
						return (ConnectionResult.Success, connector);
					}

				}

				connector.Dispose();
				return (ConnectionResult.HandshakeFailed, null);
			}
			catch
			{
				connector?.Dispose();
				return (ConnectionResult.UnknownError, null);
			}
		}


		private async Task ReadAsyncTask()
		{
			while (_messageStream.CanRead)
			{
				Message[]? receivedMessages = await ReadAsync();
				if(receivedMessages != null)
				{
					foreach (Message receivedMessage in receivedMessages)
					{
						HandleReceivedMessage(receivedMessage);
					}
				}
			}

			Disconnected?.Invoke(Port, new());
		}

		private async Task<Message[]?> ReadAsync()
		{
			int bytesRead = await _messageStream.ReadAsync(_readBuffer.SerialBuffer);
			return _readBuffer.Deserialize(bytesRead);
		}

		private void HandleReceivedMessage(Message message)
		{
			switch (message.Destination)
			{
				case MessageDestination.DesktopDebug:
					if(ReceivedDebugEventArgs.DeserializeMessage(message) is ReceivedDebugEventArgs debugEventArgs)
					{
						ReceivedDebug?.Invoke(Port, debugEventArgs);
					}
					break;
				case MessageDestination.DesktopFirmware:
					switch ((MessageDesktopFirmwareDataType)message.DataType)
					{
						case MessageDesktopFirmwareDataType.CurrentConfig:
							if (ReceivedFirmwareCurrentConfigEventArgs.DeserializeMessage(message) is ReceivedFirmwareCurrentConfigEventArgs firmwareCurrentConfig)
							{
								ReceivedFirmwareCurrentConfig?.Invoke(Port, firmwareCurrentConfig);
							}
							break;
					}
					break;
				case MessageDestination.DesktopGamepad:
					switch ((MessageDesktopGamepadDataType)message.DataType)
					{
						case MessageDesktopGamepadDataType.CurrentColors:
							if (ReceivedGamepadCurrentColorsEventArgs.DeserializeMessage(message) is ReceivedGamepadCurrentColorsEventArgs gamepadCurrentColors)
							{
								ReceivedGamepadCurrentColors?.Invoke(Port, gamepadCurrentColors);
							}
							break;
						case MessageDesktopGamepadDataType.PlayerLEDs:
							if (ReceivedGamepadPlayerLEDsEventArgs.DeserializeMessage(message) is ReceivedGamepadPlayerLEDsEventArgs GamepadPlayerLEDs)
							{
								ReceivedGamepadPlayerLEDs?.Invoke(Port, GamepadPlayerLEDs);
							}
							break;
					}
					break;
			}
		}


		private ValueTask SendMessage(Message message)
		{
			byte[] confirmationSequence = MessageSerializer.Serialize(message);
			return _messageStream.WriteAsync(confirmationSequence);
		}

		public ValueTask SendGamepadRequestColors()
		{
			return SendMessage(new(
				MessageDestination.DeviceGamepad,
				(byte)MessageDeviceGamepadDataType.RequestColors,
				[]
			));
		}

		public ValueTask SendGamepadSetColors(GamepadColors colors)
		{
			return SendMessage(new(
				MessageDestination.DeviceGamepad,
				(byte)MessageDeviceGamepadDataType.SetColors,
				colors.ToBytes()
			));
		}

		public ValueTask SendGamepadRequestPlayerLEDs()
		{
			return SendMessage(new(
				MessageDestination.DeviceGamepad,
				(byte)MessageDeviceGamepadDataType.RequestPlayerLEDs,
				[]
			));
		}

		public ValueTask SendGamepadInputs(GamepadInputs inputs)
		{
			return SendMessage(new(
				MessageDestination.DeviceGamepad,
				(byte)MessageDeviceGamepadDataType.Input,
				inputs.ToBytes()
			));
		}

		public ValueTask SendFirmwareRequestConfig()
		{
			return SendMessage(new(
				MessageDestination.DeviceFirmware,
				(byte)MessageDeviceFirmwareDataType.RequestConfig,
				[]
			));
		}

		public ValueTask SendFirmwareSetConfig(FirmwareConfig config)
		{
			return SendMessage(new(
				MessageDestination.DeviceFirmware,
				(byte)MessageDeviceFirmwareDataType.SetConfig,
				config.ToBytes()
			));
		}

		#region Boilerplate dispose code

		private bool _disposedValue;

		private void Dispose(bool disposing)
		{
			if (!_disposedValue)
			{
				if (disposing)
				{
					if(_messageStream.CanWrite)
					{
						_messageStream.Write(MessageSerializer.Serialize(new(MessageDestination.DeviceDisconnect, 0, [])));
					}

					_messageStream.Dispose();
				}

				_disposedValue = true;
			}
		}

		public void Dispose()
		{
			// Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
			Dispose(disposing: true);
			GC.SuppressFinalize(this);
		}

		#endregion

	}
}
