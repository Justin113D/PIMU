using Godot;
using J113D.Pimu.Desktop.App.Input;
using System;
using System.Collections.Generic;
using static System.Collections.Specialized.BitVector32;

namespace J113D.Pimu.Desktop.App.scripts.Input.Config
{
	public static class InputMappingHelper
	{
		private const string _configPath = "user://InputMapping.cfg";
		private const string _configSection = "Input Mapping";
		private const string _configTypeKey = "Key";
		private const string _configTypeMouseButton = "MouseButton";

		private static readonly Dictionary<StringName, InputEvent?> _defaultEvents = [];

		public static void StoreDefault()
		{
			_defaultEvents.Clear();

			foreach (StringName action in GamepadActions.All)
			{
				Godot.Collections.Array<InputEvent> events = InputMap.ActionGetEvents(action);
				if (events.Count == 0 || (events[0] is not InputEventKey and not InputEventMouseButton))
				{
					_defaultEvents[action] = null;
				}
				else
				{
					InputEvent defaultInputEvent = (InputEvent)Activator.CreateInstance(events[0].GetType())!;
					defaultInputEvent.CopyFromResource(events[0]);
					_defaultEvents[action] = defaultInputEvent;
				}
			}
		}

		public static ConfigFile ToConfigFile()
		{
			ConfigFile result = new();

			foreach (StringName action in GamepadActions.All)
			{
				Godot.Collections.Array<InputEvent> inputEvents = InputMap.ActionGetEvents(action);

				if (inputEvents.Count == 0)
				{
					continue;
				}

				InputEvent inputEvent = inputEvents[0];
				string value;

				if (inputEvent is InputEventKey keyEvent)
				{
					value = $"{_configTypeKey}:{(int)keyEvent.PhysicalKeycode}";

					if (keyEvent.Location != KeyLocation.Unspecified)
					{
						value += ":" + keyEvent.Location.ToString();
					}
				}
				else if (inputEvent is InputEventMouseButton mouseButtonEvent)
				{
					value = $"{_configTypeMouseButton}:{(int)mouseButtonEvent.ButtonIndex}";
				}
				else
				{
					continue;
				}

				result.SetValue(_configSection, action, value);
			}

			return result;
		}

		public static void SaveToFile()
		{
			ToConfigFile().Save(_configPath);
		}

		public static void LoadFromFile()
		{
			ConfigFile config = new();
			if(config.Load(_configPath) == Error.Ok)
			{
				Load(config);
			}
		}

		public static void Load(ConfigFile config)
		{
			if (!config.HasSection(_configSection))
			{
				return;
			}

			HashSet<StringName> loaded = [];

			foreach (string key in config.GetSectionKeys(_configSection))
			{
				StringName action = key;
				if (!_defaultEvents.ContainsKey(action))
				{
					continue;
				}

				string inputValue = (string)config.GetValue(_configSection, key);
				string[] inputValues = inputValue.Split(':');

				if (inputValues.Length == 1)
				{
					continue;
				}

				if (!int.TryParse(inputValues[1], out int inputCode))
				{
					continue;
				}

				InputEvent newInputEvent;

				switch (inputValues[0])
				{
					case _configTypeKey:
						newInputEvent = new InputEventKey()
						{
							PhysicalKeycode = (Key)inputCode
						};

						if (inputValues.Length > 2 && Enum.TryParse(inputValues[2], out KeyLocation location))
						{
							((InputEventKey)newInputEvent).Location = location;
						}

						break;
					case _configTypeMouseButton:
						newInputEvent = new InputEventMouseButton()
						{
							ButtonIndex = (MouseButton)inputCode
						};
						break;
					default:
						continue;
				}

				loaded.Add(action);
				InputMap.ActionEraseEvents(action);
				InputMap.ActionAddEvent(action, newInputEvent);
			}

			foreach (StringName action in GamepadActions.All)
			{
				if(!loaded.Contains(action))
				{
					InputMap.ActionEraseEvents(action);
				}
			}
		}


		public static void SetAction(StringName action, InputEvent @event)
		{
			if (!_defaultEvents.ContainsKey(action) || (@event is not InputEventKey and not InputEventMouseButton))
			{
				return;
			}

			InputMap.ActionEraseEvents(action);
			InputMap.ActionAddEvent(action, @event);
		}

		public static void ResetAll()
		{
			foreach (KeyValuePair<StringName, InputEvent?> defaultEvent in _defaultEvents)
			{
				InputMap.ActionEraseEvents(defaultEvent.Key);
				if(defaultEvent.Value != null)
				{
					InputMap.ActionAddEvent(defaultEvent.Key, defaultEvent.Value);
				}
			}
		}

		public static void ResetAction(StringName name)
		{
			if (!_defaultEvents.TryGetValue(name, out InputEvent? defaultInputEvent))
			{
				return;
			}

			InputMap.ActionEraseEvents(name);
			if (defaultInputEvent != null)
			{
				InputMap.ActionAddEvent(name, defaultInputEvent);
			}
		}

		public static void ClearAction(StringName name)
		{
			if (!_defaultEvents.ContainsKey(name))
			{
				return;
			}

			InputMap.ActionEraseEvents(name);
		}

	}
}
