using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	[Flags]
	public enum InputButtons : uint
	{
		B = 0x00000001,
		A = 0x00000002,
		Y = 0x00000004,
		X = 0x00000008,
		R = 0x00000010,
		ZR = 0x00000020,
		Plus = 0x00000040,
		StickRight = 0x00000080,
		DPadDown = 0x00000100,
		DPadRight = 0x00000200,
		DPadLeft = 0x00000400,
		DPadUp = 0x00000800,
		L = 0x00001000,
		ZL = 0x00002000,
		Minus = 0x00004000,
		StickLeft = 0x00008000,
		Home = 0x00010000,
		Capture = 0x00020000,
		GR = 0x00040000,
		GL = 0x00080000,
		Chat = 0x00100000,
		Unknown_21 = 0x00200000,
		Unknown_22 = 0x00400000,
		Unknown_23 = 0x00800000,
	}
}
