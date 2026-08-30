using Godot;
using J113D.Pimu.Desktop.App.Event;
using J113D.Pimu.Desktop.App.Input;
using J113D.Pimu.Desktop.Connector.Structs;
using System;
using System.Collections.Generic;

namespace J113D.Pimu.Desktop.App.UI
{
	public partial class GamepadDisplayController : PanelContainer
	{
		private static readonly Vector2 _stickOffsetFactor = new(0.25f, -0.25f);

		public enum OutlineMode
		{
			None,
			Highlighted,
			Capturing
		}


		private (GamepadButtons button, Control mask)[] _maskMap = [];
		private Control[] _playerLEDMasks = [];

		private bool _mouseOver;
		private OutlineMode _outlineMode;


		[ExportCategory("Controls")]
		[Export]
		private TextureRect? Outline { get; set; }

		[Export]
		private TextureRect? LeftStick { get; set; }

		[Export]
		private TextureRect? RightStick { get; set; }

		[Export]
		private Node3D? GamepadModel { get; set; }


		[ExportCategory("Display properties")]
		[Export]
		private Texture2D? LeftStickDefault { get; set; }

		[Export]
		private Texture2D? LeftStickActive { get; set; }

		[Export]
		private Texture2D? RightStickDefault { get; set; }

		[Export]
		private Texture2D? RightStickActive { get; set; }

		[Export]
		private Color HighlightingColor { get; set; }

		[Export]
		private Color CapturingColor { get; set; }


		public override void _Ready()
		{
			base._Ready();

			Outline!.Modulate = HighlightingColor;
			Outline!.Visible = false;

			List<(GamepadButtons, Control)> maskMap = [];
			foreach (GamepadButtons button in Enum.GetValues<GamepadButtons>())
			{
				if (GetNodeOrNull<Control>($"%Mask_{button}") is Control mask)
				{
					maskMap.Add((button, mask));
				}
			}
			_maskMap = [.. maskMap];

			_playerLEDMasks = new Control[4];
			for (int i = 0; i < _playerLEDMasks.Length; i++)
			{
				_playerLEDMasks[i] = GetNode<Control>($"%Mask_PlayerLED{i + 1}");
			}
		}

		public override void _Process(double delta)
		{
			base._Process(delta);

			if (_outlineMode == OutlineMode.Capturing)
			{
				Outline!.OffsetTransformPositionRatio = Outline!.OffsetTransformPositionRatio.Lerp(Vector2.Zero, (float)delta);
			}
		}

		private void UpdateDisplay(GamepadInputState state)
		{
			foreach ((GamepadButtons button, Control? mask) in _maskMap)
			{
				mask!.Visible = state.Buttons.HasFlag(button);
			}

			LeftStick!.Texture = state.Buttons.HasFlag(GamepadButtons.StickLeft)
				? LeftStickActive
				: LeftStickDefault;

			LeftStick!.OffsetTransformPositionRatio = state.StickLeft * _stickOffsetFactor;

			RightStick!.Texture = state.Buttons.HasFlag(GamepadButtons.StickRight)
				? RightStickActive
				: RightStickDefault;

			RightStick!.OffsetTransformPositionRatio = state.StickRight * _stickOffsetFactor;


			Vector2 outlineOffset = Vector2.Zero;

			if (state.PointerDelta.X != 0)
			{
				outlineOffset.X = float.Log(float.Abs(state.PointerDelta.X) + 1) * float.Sign(state.PointerDelta.X);
			}

			if (state.PointerDelta.Y != 0)
			{
				outlineOffset.Y = float.Log(float.Abs(state.PointerDelta.Y) + 1) * float.Sign(state.PointerDelta.Y);
			}

			Outline!.OffsetTransformPositionRatio = outlineOffset * 0.002f;

			GamepadModel!.Quaternion = Basis.FromEuler(new(state.Pitch, state.Yaw, 0), EulerOrder.Xyz).GetRotationQuaternion();
		}

		private void SetOutlineMode(OutlineMode outlineMode)
		{
			_outlineMode = outlineMode;
			Outline!.OffsetTransformPositionRatio = Vector2.Zero;

			switch (_outlineMode)
			{
				case OutlineMode.None:
					Outline!.Visible = false;
					break;
				case OutlineMode.Highlighted:
					Outline!.Visible = true;
					Outline.Modulate = HighlightingColor;
					break;
				case OutlineMode.Capturing:
					Outline!.Visible = true;
					Outline.Modulate = CapturingColor;
					break;
			}
		}


		public void OnMouseOverChanged(bool mouseOver)
		{
			_mouseOver = mouseOver;

			if (_outlineMode == OutlineMode.Capturing)
			{
				return;
			}

			SetOutlineMode(
				_mouseOver
				? OutlineMode.Highlighted
				: OutlineMode.None
			);
		}

		public void OnCaptureInputChanged(bool captureInput)
		{
			if (captureInput)
			{
				SetOutlineMode(OutlineMode.Capturing);
			}
			else
			{
				SetOutlineMode(
					_mouseOver
					? OutlineMode.Highlighted
					: OutlineMode.None
				);
			}
		}

		public void OnGamepadStateChanged(GamepadInputStateChangedEvent args)
		{
			UpdateDisplay(args.State);
		}

		public void OnLEDsChanged(GamepadLEDsChangedEvent args)
		{
			// TODO implement flashing

			for (int i = 0; i < args.PlayerLEDs.Length; i++)
			{
				_playerLEDMasks[i].Visible = args.PlayerLEDs[i];
			}
		}
	}
}

