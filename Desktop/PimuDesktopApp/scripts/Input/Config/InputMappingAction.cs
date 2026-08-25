using Godot;

namespace J113D.Pimu.Desktop.App.Input.Mapping
{
	[GlobalClass]
	public partial class InputMappingAction : Resource
	{
		[Export]
		public string Label { get; set; } = string.Empty;

		[Export(PropertyHint.InputName)]
		public StringName? Action { get; set; }
	}
}
