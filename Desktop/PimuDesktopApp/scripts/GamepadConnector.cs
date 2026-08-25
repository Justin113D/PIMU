using Godot;
using J113D.Pimu.Desktop.App.Event;
using J113D.Pimu.Desktop.App.Input;
using J113D.Pimu.Desktop.Connector;
using J113D.Pimu.Desktop.Connector.Event;
using J113D.Pimu.Desktop.Connector.Structs;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.App
{
	public partial class GamepadConnector : Node
	{
		private readonly PimuConnector _connector;

		private GamepadInputState? _sendState;

		public string Port => _connector.Port;


		public FirmwareConfig Config { get; private set; }

		public GamepadColors Colors { get; private set; }

		public bool[] PlayerLEDsEnabled { get; private set; } = [];

		public bool PlayerLEDsFlashing { get; private set; }


		[Signal]
		public delegate void LEDsChangedEventHandler(GamepadLEDsChangedEvent @event);


		public GamepadConnector(PimuConnector connector)
		{
			_connector = connector;

			_connector.Disconnected += OnDisconnected;
			_connector.ReceivedFirmwareCurrentConfig += OnReceivedFirmwareCurrentConfig;
			_connector.ReceivedGamepadCurrentColors += OnReceivedGamepadCurrentColors;
			_connector.ReceivedGamepadPlayerLEDs += OnReceivedGamepadPlayerLEDs;
			_connector.ReceivedDebug += OnReceivedDebug;
		}


		public override async void _Process(double delta)
		{
			base._Process(delta);

			if(_sendState == null)
			{
				return;
			}

			try
			{
				await _connector.SendGamepadInputs(_sendState.Value.ToInputs());
			}
			catch
			{
				return;
			}

			_sendState = null;
		}

		public async Task RequestDeviceInfo()
		{
			await _connector.SendFirmwareRequestConfig();
			await _connector.SendGamepadRequestColors();
			await _connector.SendGamepadRequestPlayerLEDs();
		}


		private void OnReceivedDebug(string port, ReceivedDebugEventArgs args)
		{
			GD.Print(args.Debug);
		}

		private void OnReceivedFirmwareCurrentConfig(string port, ReceivedFirmwareCurrentConfigEventArgs args)
		{
			Config = args.Config;
		}

		private void OnReceivedGamepadCurrentColors(string port, ReceivedGamepadCurrentColorsEventArgs args)
		{
			Colors = args.Colors;
		}

		private void OnReceivedGamepadPlayerLEDs(string port, ReceivedGamepadPlayerLEDsEventArgs args)
		{
			PlayerLEDsEnabled = args.LEDsEnabled;
			PlayerLEDsFlashing = args.Flashing;

			CallDeferred(
				GodotObject.MethodName.EmitSignal, 
				SignalName.LEDsChanged, 
				new GamepadLEDsChangedEvent(args.LEDsEnabled, args.Flashing)
			);
		}


		private void OnDisconnected(string port, DisconnectedEventArgs args)
		{
			Disconnect();
		}

		public void Disconnect()
		{
			if(IsQueuedForDeletion())
			{
				// already disconnecting
				return;
			}

			_sendState = null;
			EmitSignalLEDsChanged(new GamepadLEDsChangedEvent(new bool[4], false));

			_connector.Dispose();

			QueueFree();
		}


		public void OnGamepadStateChanged(GamepadInputStateChangedEvent @event)
		{
			if(IsQueuedForDeletion())
			{
				return;
			}

			_sendState = @event.State;
		}
	
		public ValueTask SetConfig(FirmwareConfig config)
		{
			Config = config;
			return _connector.SendFirmwareSetConfig(config);
		}

		public ValueTask SetColors(GamepadColors colors)
		{
			Colors = colors;
			return _connector.SendGamepadSetColors(colors);
		}
	}
}
