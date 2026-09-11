using Godot;
using J113D.Pimu.Desktop.App.Input;
using J113D.Pimu.Desktop.App.scripts.Input.Config;
using J113D.Pimu.Desktop.App.UI;
using J113D.Pimu.Desktop.App.UI.Window;
using J113D.Pimu.Desktop.Connector;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.App
{
	public partial class AppController : PanelContainer
	{
		private GamepadConnector? _connector;


		[Export]
		private GamepadDisplayController? DisplayController { get; set; }

		[Export]
		private GamepadInputHandler? InputHandler { get; set; }

		[Export]
		private PopupMenu? MenuPimuDevice { get; set; }

		[Export]
		private ConnectWindowController? ConnectWindow { get; set; }

		[Export]
		private FirmwareConfigWindowController? FirmwareConfigWindow { get; set; }

		[Export]
		private GamepadColorWindowController? GamepadColorWindow { get; set; }

		[Export]
		private InputMappingWindowController? InputMappingWindow { get; set; }

		[Export]
		private Label? LabelConnectStatus { get; set; }

		[Export]
		private AcceptDialog? DialogDisconnected { get; set; }




		public override void _EnterTree()
		{
			base._EnterTree();
			InputMappingConfig.StoreDefault();
			InputMappingConfig.LoadFromFile();
		}


		public void OnPimuMenuIDPressed(int id)
		{
			switch (id)
			{
				case 0:
					ConnectWindow!.RefreshList();
					ConnectWindow!.PopupCentered();
					break;
				case 2:
					FirmwareConfigWindow!.Open(_connector!);
					break;
				case 3:
					GamepadColorWindow!.Open(_connector!);
					break;
				case 4:
					_connector?.Disconnect();
					break;
			}
		}

		public void OnSettingsMenuIDPressed(int id)
		{
			switch (id)
			{
				case 0:
					InputMappingWindow!.PopupCentered();
					break;
			}
		}


		public async Task<PimuConnector.ConnectionResult> TryConnect(string port)
		{
			(PimuConnector.ConnectionResult result, PimuConnector? connector) result = await PimuConnector.TryEstablishConnection(port);

			if(result.result == PimuConnector.ConnectionResult.Success)
			{
				_connector = new(result.connector!);
				_connector.Connect(Node.SignalName.TreeExiting, new(this, MethodName.OnConnectorExiting));

				_connector.Connect(GamepadConnector.SignalName.LEDsChanged, new(DisplayController, GamepadDisplayController.MethodName.OnLEDsChanged));
				InputHandler!.Connect(GamepadInputHandler.SignalName.GamepadStateChanged, new(_connector, GamepadConnector.MethodName.OnGamepadStateChanged));

				await _connector.RequestDeviceInfo();

				CallDeferred(MethodName.OnConnectorSetup);
			}

			return result.result;
		}

		private void OnConnectorSetup()
		{
			if(_connector == null)
			{
				return;
			}

			AddChild(_connector);

			LabelConnectStatus!.Text = "Connected to " + _connector.Port;

			MenuPimuDevice!.SetItemDisabled(0, true);
			MenuPimuDevice!.SetItemDisabled(2, false);
			MenuPimuDevice!.SetItemDisabled(3, false);
			MenuPimuDevice!.SetItemDisabled(4, false);
		}

		private void OnConnectorExiting()
		{
			_connector = null;

			LabelConnectStatus!.Text = "Not Connected";

			MenuPimuDevice!.SetItemDisabled(0, false);
			MenuPimuDevice!.SetItemDisabled(2, true);
			MenuPimuDevice!.SetItemDisabled(3, true);
			MenuPimuDevice!.SetItemDisabled(4, true);

			DialogDisconnected!.Show();
		}
	}
}
