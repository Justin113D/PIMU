using Godot;

namespace J113D.Pimu.Desktop.App.Input.Mapping
{
	[GlobalClass]
	public partial class InputMappingCategory : Resource
	{
		[Export]
		public string Label { get; set; } = string.Empty;

		[Export]
		public Godot.Collections.Array<InputMappingAction>? Actions { get; set; }
	}
}
