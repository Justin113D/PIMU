using Godot;

namespace J113D.Pimu.Desktop.App.Input.Mapping
{
	[GlobalClass]
	public partial class InputMapping : Resource
	{
		[Export]
		public Godot.Collections.Array<InputMappingCategory>? ActionCategories { get; set; }
	}
}
