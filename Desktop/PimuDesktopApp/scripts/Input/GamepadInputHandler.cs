using Godot;
using J113D.Pimu.Desktop.Connector.Structs;

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
		
		enum HandledState
		{
			None,
			Handled,
			HandledChanged
		}


		private static (StringName action, GamepadButtons button)[] _actionButtonMapping { get; } = [
			(GamepadActions.A, GamepadButtons.A),
			(GamepadActions.B, GamepadButtons.B),
			(GamepadActions.X, GamepadButtons.X),
			(GamepadActions.Y, GamepadButtons.Y),
			(GamepadActions.DPadUp, GamepadButtons.DPadUp),
			(GamepadActions.DPadRight, GamepadButtons.DPadRight),
			(GamepadActions.DPadDown, GamepadButtons.DPadDown),
			(GamepadActions.DPadLeft, GamepadButtons.DPadLeft),
			(GamepadActions.L, GamepadButtons.L),
			(GamepadActions.ZL, GamepadButtons.ZL),
			(GamepadActions.GL, GamepadButtons.GL),
			(GamepadActions.R, GamepadButtons.R),
			(GamepadActions.ZR, GamepadButtons.ZR),
			(GamepadActions.GR, GamepadButtons.GR),
			(GamepadActions.Plus, GamepadButtons.Plus),
			(GamepadActions.Minus, GamepadButtons.Minus),
			(GamepadActions.Home, GamepadButtons.Home),
			(GamepadActions.Capture, GamepadButtons.Capture),
			(GamepadActions.Chat, GamepadButtons.Chat),
			(GamepadActions.LeftStickPress, GamepadButtons.StickLeft),
			(GamepadActions.RightStickPress, GamepadButtons.StickRight)
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


		[Signal]
		public delegate void GamepadStateChangedEventHandler(GamepadInputStateChangedEvent @event);

		private static Vector2 ConstructJoystick(InputEvent inputEvent, JoystickMapping mapping, Vector2 previous, ref HandledState state)
		{
			if (!inputEvent.IsAction(mapping.Up)
				&& !inputEvent.IsAction(mapping.Right)
				&& !inputEvent.IsAction(mapping.Down)
				&& !inputEvent.IsAction(mapping.Left)
				&& !inputEvent.IsAction(mapping.Reduce))
			{
				return previous;
			}

			Vector2 vector = Godot.Input.GetVector(
				mapping.Left,
				mapping.Right,
				mapping.Down,
				mapping.Up
			);

			if (Godot.Input.IsActionPressed(mapping.Reduce))
			{
				vector *= 0.5f;
			}

			if (vector.DistanceTo(previous) > 0.01f)
			{
				state = HandledState.HandledChanged;
				return vector;
			}

			if(state == HandledState.None)
			{
				state = HandledState.Handled;
			}

			return previous;
		}

		private HandledState UpdateGamepadState(InputEvent inputEvent)
		{
			HandledState result = HandledState.None;
			GamepadInputState state = State;

			if(inputEvent is InputEventMouseMotion mouseMotion)
			{
				if(mouseMotion.Relative != Vector2.Zero || mouseMotion.Relative != state.PointerDelta)
				{
					result = HandledState.HandledChanged;
				}
				else
				{
					result = HandledState.Handled;
				}

				state.PointerDelta = mouseMotion.Relative;
				state.YawDelta = state.PointerDelta.Y * 100;

				if (mouseMotion.Relative != Vector2.Zero)
				{
					state.Pitch += mouseMotion.Relative.Y * 0.002f;
				}
			}
			else
			{
				foreach ((StringName action, GamepadButtons button) in _actionButtonMapping)
				{
					if (!inputEvent.IsAction(action))
					{
						continue;
					}

					bool pressed = inputEvent.IsPressed();
					if(pressed != state.Buttons.HasFlag(button))
					{
						result = HandledState.HandledChanged;
						if(pressed)
						{
							state.Buttons |= button;
						}
						else
						{
							state.Buttons &= ~button;
						}
					}
					else if(result == HandledState.None)
					{
						result = HandledState.Handled;
					}
				}
			
				state.StickLeft = ConstructJoystick(inputEvent, _actionJoystickMapping[0], state.StickLeft, ref result);
				state.StickRight = ConstructJoystick(inputEvent, _actionJoystickMapping[1], state.StickRight, ref result);
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

			HandledState handledState = UpdateGamepadState(@event);

			if (handledState == HandledState.None)
			{
				return;
			}

			GetViewport().SetInputAsHandled();

			if (handledState == HandledState.HandledChanged)
			{
				EmitSignalGamepadStateChanged(new GamepadInputStateChangedEvent(State));
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
	}
}
