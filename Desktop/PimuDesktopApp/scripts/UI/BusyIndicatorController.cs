using Godot;
using System.Linq;

namespace J113D.Pimu.Desktop.App.UI
{
	public partial class BusyIndicatorController : Control
	{
		private static readonly StringName _panelStyleBox = "panel";

		private Panel[] _busyDots = [];
		private StyleBoxFlat? _busyDotStyleboxDefault;
		private StyleBoxFlat? _busyDotStyleboxBig;
		private int _currentBigIndex = -1;


		public override void _Ready()
		{
			base._Ready();

			Container busyDotsContainer = GetNode<Container>("%Container_BusyDots")!;
			_busyDots = [.. busyDotsContainer.GetChildren().Cast<Panel>()];

			_busyDotStyleboxDefault = (StyleBoxFlat)_busyDots[0].GetThemeStylebox(_panelStyleBox);

			_busyDotStyleboxBig = new();
			_busyDotStyleboxBig.CopyFromResource(_busyDotStyleboxDefault);
			_busyDotStyleboxBig.SetExpandMarginAll(8);
			_busyDotStyleboxBig.SetCornerRadiusAll(_busyDotStyleboxBig.CornerRadiusBottomLeft + 8);
		}

		public override void _Process(double delta)
		{
			base._Process(delta);
			if (!Visible)
			{
				return;
			}

			int bigIndex = (int)(Time.GetTicksMsec() % 1500 / 500);
			if (_currentBigIndex == bigIndex)
			{
				return;
			}

			_currentBigIndex = bigIndex;
			for (int i = 0; i < _busyDots.Length; i++)
			{
				_busyDots[i].AddThemeStyleboxOverride(
					_panelStyleBox,
					i == _currentBigIndex
						? _busyDotStyleboxBig
						: _busyDotStyleboxDefault
				);
			}
		}
	}
}
