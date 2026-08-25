using System;

namespace J113D.Pimu.Desktop.Connector.Structs
{
	[Flags]
	public enum GamepadButtons : uint
	{
		Y = 0x00000001,
		X = 0x00000002,
		B = 0x00000004,
		A = 0x00000008,
		SRRight = 0x00000010,
		SLRight = 0x00000020,
		R = 0x00000040,
		ZR = 0x00000080,
		Minus = 0x00000100,
		Plus = 0x00000200,
		StickRight = 0x00000400,
		StickLeft = 0x00000800,
		Home = 0x00001000,
		Capture = 0x00002000,
		Chat = 0x00004000,
		Unknown15 = 0x00008000,
		DPadDown = 0x00010000,
		DPadUp = 0x00020000,
		DPadRight = 0x00040000,
		DPadLeft = 0x00080000,
		SRLeft = 0x00100000,
		SLLeft = 0x00200000,
		L = 0x00400000,
		ZL = 0x00800000,
		GR = 0x01000000,
		GL = 0x02000000,
		Unknown26 = 0x04000000,
		Unknown27 = 0x08000000,
		Headset = 0x10000000,
		Unknown29 = 0x20000000,
		Unknown30 = 0x40000000,
		Unknown31 = 0x80000000,
	}
}
