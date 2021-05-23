#include "EP_PCH.h"
#if defined(__APPLE__) && defined(__MACH__)

#include "Enterprise/Input/Input.h"

using Enterprise::Input;
using Enterprise::Events;

static NSMutableArray *gamepads; // The list of connected GCControllers.  Ordered oldest to newest.
static std::vector<size_t> activeGamepadIDs; // The list of active GamepadIDs.  Ordered oldest to newest.
static std::vector<bool> isGamepadIDActive; // Stores GamepadID active status by index.

void Input::PlatformInit()
{
	// Init gamepads array
	gamepads = [[NSMutableArray alloc] init];

	// New gamepad connections
	[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidConnectNotification
													  object:nil
													   queue:[NSOperationQueue currentQueue]
												  usingBlock:^(NSNotification* Notification)
	 {
		// We only support the extended gamepad profile.  TODO: Should we support the others?
		if ([Notification.object extendedGamepad] != nil)
		{
			// Identify the next available GamePadID.
			auto GPIDIt = std::find(isGamepadIDActive.begin(), isGamepadIDActive.end(), false);
			size_t newGPID = GPIDIt - isGamepadIDActive.begin();

			// Store the GCController reference and its GamepadID.
			[gamepads addObject:Notification.object];
			activeGamepadIDs.push_back(newGPID);

			// Mark the GamepadID as taken, allocating engine resources if needed.
			if (GPIDIt == isGamepadIDActive.end())
			{
				isGamepadIDActive.push_back(true);
				Input::gpBuffer.emplace_back();
			}
			else
			{
				isGamepadIDActive[newGPID] = true;
				(*GPIDIt) = true;
			}

			// Notify the game code that the controller was connected.
			Events::Dispatch(HN("ControllerWake"), newGPID + 1);

			EP_INFO("Gamepad connected.  ControllerID: {}", newGPID + 1);
		}
	 }];

	// Gamepad disconnections
	[[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidDisconnectNotification
													  object:nil
													   queue:[NSOperationQueue currentQueue]
												  usingBlock:^(NSNotification* Notification)
	{
		NSUInteger disconnectedReferenceIndex = [gamepads indexOfObject:Notification.object];
		if (disconnectedReferenceIndex != NSNotFound)
		{
			// Get the GamepadID of the disconnected controller.
			size_t disconnectedGPID = activeGamepadIDs[disconnectedReferenceIndex];

			// Release the GamepadID in the engine.
			Input::gpBuffer[disconnectedGPID] = GamePadBuffer(); // Clear buffer for future connections
			PlayerID disconnectedPlayerID = Input::UnassignController(disconnectedGPID + 1);

			// Notify the game code of the disconnection.
			if (disconnectedPlayerID != EP_PLAYERID_NULL)
			{
				Events::Dispatch(HN("ControllerDisconnect"), disconnectedPlayerID);
				EP_INFO("Gamepad disconnected.  PlayerID: {}", disconnectedPlayerID);
			}
			else
			{
				// ControllerDisconnect events only fire for gamepads assigned to a player.
				EP_INFO("Gamepad disconnected.  PlayerID: unassigned");
			}

			// Delete the gamepad reference and clear its ID.
			[gamepads removeObjectAtIndex:disconnectedReferenceIndex];
			activeGamepadIDs.erase(activeGamepadIDs.begin() + disconnectedReferenceIndex);
			isGamepadIDActive[disconnectedGPID] = false;
		}
		else //if notification.object is an extended gamepad
		{
			EP_ERROR("A gamepad was disconnected that Enterprise was unaware of.");
			EP_ASSERT_NOENTRY();
		}
	}];

	// Initial gamepad registration
	for (GCController* connectedCont in [GCController controllers])
	{
		// We only support the extended gamepad profile. TODO: Should we support the others?
		if (connectedCont.extendedGamepad != nil)
		{
			activeGamepadIDs.push_back(gamepads.count);
			[gamepads addObject:connectedCont];

			isGamepadIDActive.push_back(true);
			Input::gpBuffer.emplace_back();
		}
	}

	// Subscribe to keyboard events
	Events::Subscribe(HN("macOS_keyEvent"), &HandlePlatformEvents);
	Events::Subscribe(HN("macOS_flagsChanged"), &HandlePlatformEvents);
}

bool Input::HandlePlatformEvents(Events::Event& e)
{
	switch (e.Type())
	{
	case "macOS_keyEvent"_HN:
		{
			auto [keycode, isDownAction] = Events::Unpack<std::pair<unsigned short, bool>>(e);
			ControlID control = ControlID::_EndOfIDs;

			// Keycodes are practically random compared to ControlIDs, so switch statement is just the simplest way to convert
			switch (keycode)
			{
				case kVK_Delete: 		control = ControlID::KB_Backspace; 		break;
				case kVK_Tab: 			control = ControlID::KB_Tab; 			break;
				case kVK_Return: 		control = ControlID::KB_Enter; 			break;
				case kVK_F15: 			control = ControlID::KB_PauseBreak; 	break;
				case kVK_Escape: 		control = ControlID::KB_Esc; 			break;
				case kVK_Space: 		control = ControlID::KB_Space; 			break;
				case kVK_PageUp: 		control = ControlID::KB_PageUp; 		break;
				case kVK_PageDown: 		control = ControlID::KB_PageDown; 		break;
				case kVK_End: 			control = ControlID::KB_End; 			break;
				case kVK_Home: 			control = ControlID::KB_Home; 			break;
				case kVK_LeftArrow: 	control = ControlID::KB_Left; 			break;
				case kVK_UpArrow: 		control = ControlID::KB_Up; 			break;
				case kVK_RightArrow: 	control = ControlID::KB_Right; 			break;
				case kVK_DownArrow: 	control = ControlID::KB_Down; 			break;
				case kVK_F13: 			control = ControlID::KB_PrintScreen;	break;
				case kVK_ForwardDelete:	control = ControlID::KB_Delete; 		break;

				case kVK_ANSI_0: 		control = ControlID::KB_0; 		break;
				case kVK_ANSI_1: 		control = ControlID::KB_1; 		break;
				case kVK_ANSI_2: 		control = ControlID::KB_2; 		break;
				case kVK_ANSI_3: 		control = ControlID::KB_3; 		break;
				case kVK_ANSI_4: 		control = ControlID::KB_4; 		break;
				case kVK_ANSI_5: 		control = ControlID::KB_5; 		break;
				case kVK_ANSI_6: 		control = ControlID::KB_6; 		break;
				case kVK_ANSI_7: 		control = ControlID::KB_7; 		break;
				case kVK_ANSI_8: 		control = ControlID::KB_8; 		break;
				case kVK_ANSI_9: 		control = ControlID::KB_9; 		break;

				case kVK_ANSI_A: 		control = ControlID::KB_A; 		break;
				case kVK_ANSI_B: 		control = ControlID::KB_B; 		break;
				case kVK_ANSI_C: 		control = ControlID::KB_C; 		break;
				case kVK_ANSI_D: 		control = ControlID::KB_D; 		break;
				case kVK_ANSI_E: 		control = ControlID::KB_E; 		break;
				case kVK_ANSI_F: 		control = ControlID::KB_F; 		break;
				case kVK_ANSI_G: 		control = ControlID::KB_G; 		break;
				case kVK_ANSI_H: 		control = ControlID::KB_H; 		break;
				case kVK_ANSI_I: 		control = ControlID::KB_I; 		break;
				case kVK_ANSI_J: 		control = ControlID::KB_J; 		break;
				case kVK_ANSI_K: 		control = ControlID::KB_K; 		break;
				case kVK_ANSI_L: 		control = ControlID::KB_L; 		break;
				case kVK_ANSI_M: 		control = ControlID::KB_M; 		break;
				case kVK_ANSI_N: 		control = ControlID::KB_N; 		break;
				case kVK_ANSI_O: 		control = ControlID::KB_O; 		break;
				case kVK_ANSI_P: 		control = ControlID::KB_P; 		break;
				case kVK_ANSI_Q: 		control = ControlID::KB_Q; 		break;
				case kVK_ANSI_R: 		control = ControlID::KB_R; 		break;
				case kVK_ANSI_S: 		control = ControlID::KB_S; 		break;
				case kVK_ANSI_T: 		control = ControlID::KB_T; 		break;
				case kVK_ANSI_U: 		control = ControlID::KB_U; 		break;
				case kVK_ANSI_V: 		control = ControlID::KB_V; 		break;
				case kVK_ANSI_W: 		control = ControlID::KB_W; 		break;
				case kVK_ANSI_X: 		control = ControlID::KB_X; 		break;
				case kVK_ANSI_Y: 		control = ControlID::KB_Y; 		break;
				case kVK_ANSI_Z: 		control = ControlID::KB_Z; 		break;

				case kVK_ANSI_KeypadEquals: control = ControlID::KB_Numpad_Equals; 	break; // macOS exclusive
				case kVK_ANSI_Keypad0: 		control = ControlID::KB_Numpad_0; 		break;
				case kVK_ANSI_Keypad1: 		control = ControlID::KB_Numpad_1; 		break;
				case kVK_ANSI_Keypad2: 		control = ControlID::KB_Numpad_2; 		break;
				case kVK_ANSI_Keypad3: 		control = ControlID::KB_Numpad_3; 		break;
				case kVK_ANSI_Keypad4: 		control = ControlID::KB_Numpad_4; 		break;
				case kVK_ANSI_Keypad5: 		control = ControlID::KB_Numpad_5; 		break;
				case kVK_ANSI_Keypad6: 		control = ControlID::KB_Numpad_6; 		break;
				case kVK_ANSI_Keypad7: 		control = ControlID::KB_Numpad_7; 		break;
				case kVK_ANSI_Keypad8: 		control = ControlID::KB_Numpad_8; 		break;
				case kVK_ANSI_Keypad9: 		control = ControlID::KB_Numpad_9; 		break;

				case kVK_ANSI_KeypadMultiply: 	control = ControlID::KB_Numpad_Muliply; 	break;
				case kVK_ANSI_KeypadPlus: 		control = ControlID::KB_Numpad_Add; 		break;
				case kVK_ANSI_KeypadEnter: 		control = ControlID::KB_Numpad_Enter; 		break;
				case kVK_ANSI_KeypadMinus: 		control = ControlID::KB_Numpad_Subtract; 	break;
				case kVK_ANSI_KeypadDecimal: 	control = ControlID::KB_Numpad_Decimal; 	break;
				case kVK_ANSI_KeypadDivide: 	control = ControlID::KB_Numpad_Divide; 		break;

				case kVK_F1: 	control = ControlID::KB_F1; 	break;
				case kVK_F2: 	control = ControlID::KB_F2; 	break;
				case kVK_F3: 	control = ControlID::KB_F3; 	break;
				case kVK_F4: 	control = ControlID::KB_F4; 	break;
				case kVK_F5: 	control = ControlID::KB_F5; 	break;
				case kVK_F6: 	control = ControlID::KB_F6; 	break;
				case kVK_F7: 	control = ControlID::KB_F7; 	break;
				case kVK_F8: 	control = ControlID::KB_F8; 	break;
				case kVK_F9: 	control = ControlID::KB_F9; 	break;
				case kVK_F10: 	control = ControlID::KB_F10; 	break;
				case kVK_F11: 	control = ControlID::KB_F11; 	break;
				case kVK_F12: 	control = ControlID::KB_F12; 	break;
				// F13-15 are used for PrintScr, ScrollLock, and PauseBreak
				case kVK_F16: 	control = ControlID::KB_F16; 	break; // macOS exclusive
				case kVK_F17: 	control = ControlID::KB_F17; 	break; // macOS exclusive
				case kVK_F18: 	control = ControlID::KB_F18; 	break; // macOS exclusive
				case kVK_F19: 	control = ControlID::KB_F19; 	break; // macOS exclusive

				case kVK_ANSI_KeypadClear: 	control = ControlID::KB_NumLock; 	break;
				case kVK_F14: 				control = ControlID::KB_ScrollLock; break;

				case kVK_ANSI_Semicolon: 	control = ControlID::KB_Semicolon; 	break;
				case kVK_ANSI_Equal: 		control = ControlID::KB_Plus; 		break;
				case kVK_ANSI_Comma: 		control = ControlID::KB_Comma; 		break;
				case kVK_ANSI_Minus: 		control = ControlID::KB_Minus; 		break;
				case kVK_ANSI_Period: 		control = ControlID::KB_Period; 	break;
				case kVK_ANSI_Slash: 		control = ControlID::KB_FSlash; 	break;
				case kVK_ANSI_Grave: 		control = ControlID::KB_Tilde; 		break;
				case kVK_ANSI_LeftBracket: 	control = ControlID::KB_LBracket; 	break;
				case kVK_ANSI_Backslash: 	control = ControlID::KB_BSlash; 	break;
				case kVK_ANSI_RightBracket:	control = ControlID::KB_RBracket; 	break;
				case kVK_ANSI_Quote: 		control = ControlID::KB_Quote; 		break;
			}

			if (control != ControlID::_EndOfIDs)
			{
				uint16_t bufferIndex = uint16_t(control) - uint16_t(ControlID::_EndOfGPAxes) - 1;
				if (isDownAction)
					kbmBuffer.keys[currentBuffer][bufferIndex > 63] |= bufferIndex > 63 ? BIT(bufferIndex % 64) : BIT(bufferIndex);
				else
					kbmBuffer.keys[currentBuffer][bufferIndex > 63] &= bufferIndex > 63 ? ~(BIT(bufferIndex % 64)) : ~(BIT(bufferIndex));
			}
		}
		break;
	case "macOS_flagsChanged"_HN:
		{
			uint64_t flags = Events::Unpack<uint64_t>(e);

			// Cut out previous status bits for modifier keys
			constexpr uint64_t invertedModKeyMask_low =
				~(BIT((uint16_t)ControlID::KB_LSuper - (uint16_t)ControlID::_EndOfGPAxes - 1) |
				  BIT((uint16_t)ControlID::KB_RSuper - (uint16_t)ControlID::_EndOfGPAxes - 1));
			constexpr uint64_t invertedModKeyMask_high =
				~(BIT(((uint16_t)ControlID::KB_LCtrl - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) |
				  BIT(((uint16_t)ControlID::KB_RCtrl - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) |
				  BIT(((uint16_t)ControlID::KB_LAlt - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) |
				  BIT(((uint16_t)ControlID::KB_RAlt - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) |
				  BIT(((uint16_t)ControlID::KB_LShift - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) |
				  BIT(((uint16_t)ControlID::KB_RShift - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64));
			kbmBuffer.keys[currentBuffer][0] &= invertedModKeyMask_low;
			kbmBuffer.keys[currentBuffer][1] &= invertedModKeyMask_high;

			// Ensure that the flag masks have the expected relationship to the ControlID enum
			static_assert(BIT((uint16_t)ControlID::KB_LSuper - (uint16_t)ControlID::_EndOfGPAxes - 1) >> 51 == NX_DEVICELCMDKEYMASK);
			static_assert(BIT((uint16_t)ControlID::KB_RSuper - (uint16_t)ControlID::_EndOfGPAxes - 1) >> 51 == NX_DEVICERCMDKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_LCtrl - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 29 == NX_DEVICELCTLKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_RCtrl - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 17 == NX_DEVICERCTLKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_LAlt - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 26 == NX_DEVICELALTKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_RAlt - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 26 == NX_DEVICERALTKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_LShift - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 26 == NX_DEVICELSHIFTKEYMASK);
			static_assert(BIT(((uint16_t)ControlID::KB_RShift - (uint16_t)ControlID::_EndOfGPAxes - 1) % 64) >> 26 == NX_DEVICERSHIFTKEYMASK);

			// Slot in updated status bits for modifier keys
			kbmBuffer.keys[currentBuffer][0] |= (flags & (NX_DEVICELCMDKEYMASK | NX_DEVICERCMDKEYMASK)) << 51;
			kbmBuffer.keys[currentBuffer][1] |= (flags & NX_DEVICELCTLKEYMASK) << 29;
			kbmBuffer.keys[currentBuffer][1] |= (flags & NX_DEVICERCTLKEYMASK) << 17;
			kbmBuffer.keys[currentBuffer][1] |= (flags & (NX_DEVICELALTKEYMASK | NX_DEVICERALTKEYMASK | NX_DEVICELSHIFTKEYMASK | NX_DEVICERSHIFTKEYMASK)) << 26;
		}
		break;
	default:
		break;
	}

	return true;
}

// Somehow, when a PlayerID is assigned a Controller ID, that has to cause the associated controller object's
// PlayerIndex property to be set.  Likewise, we need to unset it when the controller is unassigned.
// Perhaps an event would do?

void Input::GetRawInput()
{
	std::vector<size_t>::iterator GPIDit = activeGamepadIDs.begin();
	for (GCController *gamepad in gamepads)
	{
		// Buttons

		Input::gpBuffer[*GPIDit].buttons[currentBuffer] =
			gamepad.extendedGamepad.dpad.up.pressed * BIT(uint16_t(ControlID::GP_Dpad_Up))
			| gamepad.extendedGamepad.dpad.down.pressed * BIT(uint16_t(ControlID::GP_Dpad_Down))
			| gamepad.extendedGamepad.dpad.left.pressed * BIT(uint16_t(ControlID::GP_Dpad_Left))
			| gamepad.extendedGamepad.dpad.right.pressed * BIT(uint16_t(ControlID::GP_Dpad_Right))
			| gamepad.extendedGamepad.buttonMenu.pressed * BIT(uint16_t(ControlID::GP_Menu))
			// TODO: Handle gamepads without the options button.
			| gamepad.extendedGamepad.buttonOptions.pressed * BIT(uint16_t(ControlID::GP_Options))
			// TODO: Handle gamepads without clickable thumbsticks.
			| gamepad.extendedGamepad.leftThumbstickButton.pressed * BIT(uint16_t(ControlID::GP_LStick_Click))
			| gamepad.extendedGamepad.rightThumbstickButton.pressed * BIT(uint16_t(ControlID::GP_RStick_Click))
			| gamepad.extendedGamepad.leftShoulder.pressed * BIT(uint16_t(ControlID::GP_LShoulder))
			| gamepad.extendedGamepad.rightShoulder.pressed * BIT(uint16_t(ControlID::GP_RShoulder))
			| gamepad.extendedGamepad.buttonA.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Down))
			| gamepad.extendedGamepad.buttonB.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Right))
			| gamepad.extendedGamepad.buttonX.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Left))
			| gamepad.extendedGamepad.buttonY.pressed * BIT(uint16_t(ControlID::GP_FaceButton_Up));

		// Triggers

		// The triggers are already normalized by the Game Controller framework.
		Input::gpBuffer[*GPIDit].axes[currentBuffer]
			[size_t(ControlID::GP_LTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = gamepad.extendedGamepad.leftTrigger.value;
		Input::gpBuffer[*GPIDit].axes[currentBuffer]
			[size_t(ControlID::GP_RTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = gamepad.extendedGamepad.rightTrigger.value;

		// Sticks
		{
			// While the Game Controller framework does normalize the thumbsticks, they are normalized per axis.
			// We have to renormalize them in 2D to ensure that the overall magnitude is never greater than 1.
			// Dead zones are already handled in 2D by the framework.

			// Left Stick

			float thumbX = gamepad.extendedGamepad.leftThumbstick.xAxis.value;
			float thumbY = gamepad.extendedGamepad.leftThumbstick.yAxis.value;
			float magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

			if (magnitude > 0.0)
			{
				// Get the unit vector for the stick
				thumbX /= magnitude;
				thumbY /= magnitude;

				// Clamp the 2D magnitude to normalled range
				if (magnitude > 1.0) magnitude = 1.0f;


				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
			}
			else
			{
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
			}

			// Right Stick

			thumbX = gamepad.extendedGamepad.rightThumbstick.xAxis.value;
			thumbY = gamepad.extendedGamepad.rightThumbstick.yAxis.value;
			magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

			if (magnitude > 0.0)
			{
				// Get the unit vector for the stick
				thumbX /= magnitude;
				thumbY /= magnitude;

				// Clamp the 2D magnitude to normalled range
				if (magnitude > 1.0) magnitude = 1.0f;


				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
			}
			else
			{
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				Input::gpBuffer[*GPIDit].axes[currentBuffer]
					[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
			}
		}

		// Step the GPID iterator for the next connected gamepad
		++GPIDit;
	}
}

#endif // macOS
