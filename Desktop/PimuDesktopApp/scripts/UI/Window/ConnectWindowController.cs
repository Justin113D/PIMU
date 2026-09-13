using Godot;
using J113D.Pimu.Desktop.Connector;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace J113D.Pimu.Desktop.App.UI.Window
{
	public partial class ConnectWindowController : Godot.Window
	{
		private readonly struct Option
		{
			public readonly Control container;
			public readonly Label labelPort;
			public readonly Label labelDescription;
			public readonly Button button;

			public Option(Control container, Label labelPort, Label labelDescription, Button button)
			{
				this.container = container;
				this.labelPort = labelPort;
				this.labelDescription = labelDescription;
				this.button = button;
			}
		}

		private readonly List<Option> _options = [];
		private int _selectedOption;

		private Task? _connectingTask;

		private bool IsConnecting => _connectingTask?.Status == TaskStatus.Running;

		#region Exports

		[Export]
		private AppController? AppController { get; set; }

		[Export]
		private PackedScene? SceneConnectionOption { get; set; }

		[ExportCategory("Controls")]
		[Export]
		private AcceptDialog? DialogConnectError { get; set; }

		[Export]
		private Control? ContainerPortList { get; set; }

		[Export]
		private Button? ButtonConnect { get; set; }

		[Export]
		private BusyIndicatorController? BusyIndicator { get; set; }

		#endregion


		public override void _Ready()
		{
			base._Ready();
			CloseRequested += OnCloseRequested;
		}

		private void OnCloseRequested()
		{
			if(!IsConnecting)
			{
				Visible = false;
			}
		}


		public void RefreshList()
		{
			_selectedOption = -1;
			ButtonConnect!.Disabled = true;
			BusyIndicator!.Visible = false;

			(string port, string description)[] available = PimuConnector.ListAvilablePorts();
			foreach ((int index, (string port, string description)) in available.Index())
			{
				Option option;

				if(_options.Count > index)
				{
					option = _options[index];
				}
				else
				{
					Control container = SceneConnectionOption!.Instantiate<Control>();
					ContainerPortList!.AddChild(container);
					
					Button button = container.GetNode<Button>("%Button");
					int buttonIndex = index;
					button.Toggled += t => ButtonPressed(buttonIndex);

					option = new(
						container,
						container.GetNode<Label>("%Label_Port"),
						container.GetNode<Label>("%Label_Description"),
						button
					);

					_options.Add(option);
				}

				option.container.Visible = true;
				option.button.ButtonPressed = false;
				option.labelPort.Text = port;
				option.labelDescription.Text = description;
			}

			for(int i = available.Length; i < ContainerPortList!.GetChildCount(); i++)
			{
				_options[i].container.Visible = false;
			}
		}

		private void ButtonPressed(int index)
		{
			if(_selectedOption >= 0 && _selectedOption != index)
			{
				_options[_selectedOption].button.SetPressedNoSignal(false);
			}

			_selectedOption = index;
			ButtonConnect!.Disabled = !_options[_selectedOption].button.ButtonPressed;
		}
	

		public void Connect()
		{
			if(IsConnecting)
			{
				return;
			}

			BusyIndicator!.Visible = true;

			_connectingTask = Task.Run(ConnectAsync);
		}
		
		private async void ConnectAsync()
		{
			(PimuConnector.ConnectionResult result, string? error)= await AppController!.TryConnect(_options[_selectedOption].labelPort.Text);
			CallDeferred(MethodName.FinishConnect, (int)result, error ?? new Variant());
		}

		private void FinishConnect(PimuConnector.ConnectionResult result, string? message)
		{
			_connectingTask = null;
			BusyIndicator!.Visible = false;

			if (result == PimuConnector.ConnectionResult.Success)
			{
				Visible = false;
				return;
			}

			DialogConnectError!.DialogText = result switch
			{
				PimuConnector.ConnectionResult.NotFound => "Port not found, please refresh the list",
				PimuConnector.ConnectionResult.OpenFailed => "Failed to open the port",
				PimuConnector.ConnectionResult.VersionMismatch => "There is a PIMU device at this port, but is has a different version than the app",
				PimuConnector.ConnectionResult.HandshakeFailed => "Device behind port is not a PIMU device",
				_ => message ?? "An unknown error occured",
			};

			DialogConnectError!.Show();
		}


	}
}
