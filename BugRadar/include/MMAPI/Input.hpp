#pragma once

#include "Core.hpp"

#include "YYToolkit/YYTK_Shared.hpp"

namespace MMAPI::Input
{
	/// Returns true if the keyboard key was pressed this frame.
	/// @param key The GameMaker virtual key code to check.
	inline bool KeyboardCheckPressed(int key)
	{
		YYTK::RValue pressed = MMAPI::Internal::module_interface->CallBuiltin("keyboard_check_pressed", { key });
		return pressed.ToBoolean();
	}

	/// Returns true if a gamepad is connected in the given slot.
	/// @param gamepad_slot The gamepad slot to check.
	inline bool GamepadIsConnected(int gamepad_slot)
	{
		YYTK::RValue connected = MMAPI::Internal::module_interface->CallBuiltin("gamepad_is_connected", { gamepad_slot });
		return connected.ToBoolean();
	}

	/// Returns the first connected gamepad slot, or -1 if no gamepad is connected.
	inline int GetFirstConnectedGamepadSlot()
	{
		for (int slot = 0; slot < 12; slot++)
		{
			if (GamepadIsConnected(slot))
				return slot;
		}

		return -1;
	}

	/// Returns true if the gamepad button was pressed this frame.
	/// @param gamepad_slot The gamepad slot to check.
	/// @param button The GameMaker gamepad button constant to check.
	inline bool GamepadButtonCheckPressed(int gamepad_slot, int button)
	{
		YYTK::RValue pressed = MMAPI::Internal::module_interface->CallBuiltin("gamepad_button_check_pressed", { gamepad_slot, button });
		return pressed.ToBoolean();
	}
}
