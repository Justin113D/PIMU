using Godot;
using System;

namespace J113D.Pimu.Desktop.App.Event
{
	public partial class GamepadLEDsChangedEvent : Resource
	{
		public bool[] PlayerLEDs { get; }

		public bool Flashing { get; }

		public GamepadLEDsChangedEvent(bool[] playerLEDs, bool flashing)
		{
			if(playerLEDs.Length != 4)
			{
				throw new ArgumentException("Player LEDs is expected to have a length of 4!", nameof(playerLEDs));
			}

			PlayerLEDs = [.. playerLEDs];
			Flashing = flashing;
		}
	}
}
