using Godot;
using J113D.Pimu.Desktop.App.Input.Mapping;
using J113D.Pimu.Desktop.App.scripts.Input.Config;
using J113D.Pimu.Desktop.Connector.Structs;
using System;

namespace J113D.Pimu.Desktop.App.Input
{
	public partial class GamepadInputHandler : Node
	{
		readonly struct JoystickMapping
		{
			public StringName Up { get; init; }
			public StringName Right { get; init; }
			public StringName Down { get; init; }
			public StringName Left { get; init; }
			public StringName Reduce { get; init; }
		}
		
		private static readonly StringName HotkeyToggleRightStickUp = "Hotkey_ToggleRightStickUp";

		private static readonly (StringName action, InputButtons button)[] _actionButtonMapping = [
			(GamepadActions.A, InputButtons.A),
			(GamepadActions.B, InputButtons.B),
			(GamepadActions.X, InputButtons.X),
			(GamepadActions.Y, InputButtons.Y),
			(GamepadActions.DPadUp, InputButtons.DPadUp),
			(GamepadActions.DPadRight, InputButtons.DPadRight),
			(GamepadActions.DPadDown, InputButtons.DPadDown),
			(GamepadActions.DPadLeft, InputButtons.DPadLeft),
			(GamepadActions.L, InputButtons.L),
			(GamepadActions.ZL, InputButtons.ZL),
			(GamepadActions.GL, InputButtons.GL),
			(GamepadActions.R, InputButtons.R),
			(GamepadActions.ZR, InputButtons.ZR),
			(GamepadActions.GR, InputButtons.GR),
			(GamepadActions.Plus, InputButtons.Plus),
			(GamepadActions.Minus, InputButtons.Minus),
			(GamepadActions.Home, InputButtons.Home),
			(GamepadActions.Capture, InputButtons.Capture),
			(GamepadActions.Chat, InputButtons.Chat),
			(GamepadActions.LeftStickPress, InputButtons.StickLeft),
			(GamepadActions.RightStickPress, InputButtons.StickRight)
		];

		private readonly static JoystickMapping[] _actionJoystickMapping = [
			new()
			{
				Up = GamepadActions.LeftStickUp,
				Right = GamepadActions.LeftStickRight,
				Down = GamepadActions.LeftStickDown,
				Left = GamepadActions.LeftStickLeft,
				Reduce = GamepadActions.LeftStickReduce
			},
			new()
			{
				Up = GamepadActions.RightStickUp,
				Right = GamepadActions.RightStickRight,
				Down = GamepadActions.RightStickDown,
				Left = GamepadActions.RightStickLeft,
				Reduce = GamepadActions.RightStickReduce
			}
		];


		public GamepadInputState State { get; private set; }

		private bool _captureInput;

		[Export]
		private CheckButton? CheckButtonRightStickUp { get; set; }

		[Signal]
		public delegate void GamepadStateChangedEventHandler(GamepadInputStateChangedEvent @event);

		public override void _Ready()
		{
			base._Ready();
			CheckButtonRightStickUp!.Toggled += OnRightStickUpToggled;
		}

		private static bool ConstructJoystick(InputEvent inputEvent, JoystickMapping mapping, Vector2 previous, out Vector2 output, ref bool handled)
		{
			output = default;

			if (!inputEvent.IsAction(mapping.Up)
				&& !inputEvent.IsAction(mapping.Right)
				&& !inputEvent.IsAction(mapping.Down)
				&& !inputEvent.IsAction(mapping.Left)
				&& !inputEvent.IsAction(mapping.Reduce))
			{
				return false;
			}

			handled = true;

			output = Godot.Input.GetVector(
				mapping.Left,
				mapping.Right,
				mapping.Down,
				mapping.Up
			);

			if (Godot.Input.IsActionPressed(mapping.Reduce))
			{
				output *= 0.5f;
			}

			return output.DistanceTo(previous) >= 0.01f;
		}

		private bool UpdateGamepadState(InputEvent inputEvent, out Inputs.InputFlags changed)
		{
			bool result = false;
			changed = default;
			GamepadInputState state = State;

			if (inputEvent is InputEventMouseMotion mouseMotion)
			{
				result = true;

				if (mouseMotion.Relative != Vector2.Zero || mouseMotion.ScreenRelative != state.PointerDelta)
				{
					changed |= Inputs.InputFlags.Gyro;
				}

				state.PointerDelta = mouseMotion.ScreenRelative;
				state.Pitch -= mouseMotion.ScreenRelative.Y * 0.01f * InputMappingConfig.MouseSensitivity.X;
				state.Yaw -= mouseMotion.ScreenRelative.X * 0.01f * InputMappingConfig.MouseSensitivity.Y;
			}
			else
			{
				foreach ((StringName action, InputButtons button) in _actionButtonMapping)
				{
					if (!inputEvent.IsAction(action))
					{
						continue;
					}

					result = true;
					bool pressed = inputEvent.IsPressed();

					if(pressed != state.Buttons.HasFlag(button))
					{
						changed |= Inputs.InputFlags.Buttons;

						if(pressed)
						{
							state.Buttons |= button;
						}
						else
						{
							state.Buttons &= ~button;
						}
					}
				}
			
				if(ConstructJoystick(inputEvent, _actionJoystickMapping[0], state.StickLeft, out Vector2 newStickLeft, ref result))
				{
					state.StickLeft = newStickLeft;
					changed |= Inputs.InputFlags.StickLeft;
				}

				if (ConstructJoystick(inputEvent, _actionJoystickMapping[1], state.StickRight, out Vector2 newStickRight, ref result))
				{
					state.StickRight = newStickRight;
					changed |= Inputs.InputFlags.StickRight;
				}


				if(inputEvent.IsAction(HotkeyToggleRightStickUp))
				{
					result = true;

					if(inputEvent.IsPressed())
					{
						CheckButtonRightStickUp!.SetPressedNoSignal(!CheckButtonRightStickUp.ButtonPressed);
						state.ForceRightStickUp = CheckButtonRightStickUp.ButtonPressed;
						changed |= Inputs.InputFlags.StickRight;
					}
				}
			}

			State = state;
			return result;
		}

		public override void _Input(InputEvent @event)
		{
			base._Input(@event);
			
			if(!_captureInput)
			{
				return;
			}

			if(!UpdateGamepadState(@event, out Inputs.InputFlags changedFlags))
			{
				return;
			}

			GetViewport().SetInputAsHandled();

			if (changedFlags != default)
			{
				EmitSignalGamepadStateChanged(new GamepadInputStateChangedEvent(State, changedFlags));
			}
		}
	
		public void SetCaptureInput(bool captureInput)
		{
			_captureInput = captureInput;

			if(_captureInput)
			{
				Godot.Input.MouseMode = Godot.Input.MouseModeEnum.Captured;
			}
			else
			{
				Godot.Input.MouseMode = Godot.Input.MouseModeEnum.Visible;
			}
		}

		private void OnRightStickUpToggled(bool pressed)
		{
			GamepadInputState state = State;
			state.ForceRightStickUp = pressed;
			State = state;
			EmitSignalGamepadStateChanged(new GamepadInputStateChangedEvent(State, Inputs.InputFlags.StickRight));
		}

	}
}
