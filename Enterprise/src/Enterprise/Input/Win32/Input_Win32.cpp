#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Input/Input.h"

using Enterprise::Events;

static bool xinputWasConnected[4] = { 0 };
static DWORD xinputPrevPacketNo[4] = { 0 };
static bool xinputAlreadyCopiedBuffer[4] = { 0 };
static Enterprise::Input::ControllerID xinputToGamepadID[4] =
{
	EP_CONTROLLERID_NULL,
	EP_CONTROLLERID_NULL,
	EP_CONTROLLERID_NULL,
	EP_CONTROLLERID_NULL
};
static std::vector<bool> isGamepadIDActive;


void Enterprise::Input::PlatformInit()
{
	// Raw Input registration (keyboard and mouse)
	RAWINPUTDEVICE device[2];

	// Mouse
	device[0].usUsagePage = 0x01;
	device[0].usUsage = 0x02;
	device[0].dwFlags = 0; // Disabling legacy messages appears to block regular window interaction.
	device[0].hwndTarget = NULL;

	// Keyboard
	device[1].usUsagePage = 0x01;
	device[1].usUsage = 0x06;
	device[1].dwFlags = 0; // Disabling legacy messages means no WM_CHAR.
	device[1].hwndTarget = NULL;

	EP_VERIFY_NEQ(RegisterRawInputDevices(device, 2, sizeof(device[0])), FALSE);

	Events::Subscribe(HN("Win32_RawInput"), &HandlePlatformEvents);
}

bool Enterprise::Input::HandlePlatformEvents(Events::Event& e)
{
	EP_ASSERT_SLOW(e.Type() == HN("Win32_RawInput"));

	RAWINPUT* ridata = Events::Unpack<RAWINPUT*>(e);
	ControlID control = ControlID::_EndOfIDs;
	bool isDownAction;

	if (ridata->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT keycode = ridata->data.keyboard.VKey;
		isDownAction = !(ridata->data.keyboard.Flags & 0x01);

		// Determine handedness of Shift, Control, and Alt keys
		if (keycode == VK_SHIFT) { keycode = MapVirtualKey(ridata->data.keyboard.MakeCode, MAPVK_VSC_TO_VK_EX); }
		if (keycode == VK_CONTROL) { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_RCONTROL : VK_LCONTROL; }
		if (keycode == VK_MENU) { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_RMENU : VK_LMENU; }

		// Distinguish Return vs. number pad Enter
		if (keycode == VK_RETURN) { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_SEPARATOR : VK_RETURN; }

		// Undo vkey transforms caused by (a lack of) number lock
		if (keycode == VK_INSERT) { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_INSERT : VK_NUMPAD0; }
		if (keycode == VK_END)    { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_END    : VK_NUMPAD1; }
		if (keycode == VK_DOWN)   { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_DOWN   : VK_NUMPAD2; }
		if (keycode == VK_NEXT)   { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_NEXT   : VK_NUMPAD3; }
		if (keycode == VK_LEFT)   { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_LEFT   : VK_NUMPAD4; }
		if (keycode == VK_CLEAR)  { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_CLEAR  : VK_NUMPAD5; }
		if (keycode == VK_RIGHT)  { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_RIGHT  : VK_NUMPAD6; }
		if (keycode == VK_HOME)   { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_HOME   : VK_NUMPAD7; }
		if (keycode == VK_UP)     { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_UP     : VK_NUMPAD8; }
		if (keycode == VK_PRIOR)  { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_PRIOR  : VK_NUMPAD9; }
		if (keycode == VK_DELETE) { keycode = ridata->data.keyboard.Flags & RI_KEY_E0 ? VK_DELETE : VK_DECIMAL; }

		// Ensure that virtual-key codes have the expected relationship to the ControlID enum values
		static_assert(VK_SPACE + 0          == uint16_t(ControlID::KB_Space));
		static_assert(VK_PRIOR + 0          == uint16_t(ControlID::KB_PageUp));
		static_assert(VK_NEXT + 0           == uint16_t(ControlID::KB_PageDown));
		static_assert(VK_END + 0            == uint16_t(ControlID::KB_End));
		static_assert(VK_HOME + 0           == uint16_t(ControlID::KB_Home));
		static_assert(VK_LEFT + 0           == uint16_t(ControlID::KB_Left));
		static_assert(VK_UP + 0             == uint16_t(ControlID::KB_Up));
		static_assert(VK_RIGHT + 0          == uint16_t(ControlID::KB_Right));
		static_assert(VK_DOWN + 0           == uint16_t(ControlID::KB_Down));

		static_assert(0x30 /* 0 key */ - 4  == uint16_t(ControlID::KB_0));
		// ... numbers
		static_assert(0x39 /* 9 key */ - 4  == uint16_t(ControlID::KB_9));

		static_assert(0x41 /* A key */ - 11 == uint16_t(ControlID::KB_A));
		// ... letters
		static_assert(0x5A /* Z key */ - 11 == uint16_t(ControlID::KB_Z));
		static_assert(VK_LWIN - 11          == uint16_t(ControlID::KB_LSuper));
		static_assert(VK_RWIN - 11          == uint16_t(ControlID::KB_RSuper));

		static_assert(0x60 /* 0 key */ - 13 == uint16_t(ControlID::KB_Numpad_0));
		// ... numpad numbers
		static_assert(0x69 /* 9 key */ - 13 == uint16_t(ControlID::KB_Numpad_9));
		static_assert(VK_MULTIPLY - 13      == uint16_t(ControlID::KB_Numpad_Muliply));
		static_assert(VK_ADD - 13           == uint16_t(ControlID::KB_Numpad_Add));
		static_assert(VK_SEPARATOR - 13     == uint16_t(ControlID::KB_Numpad_Enter));
		static_assert(VK_SUBTRACT - 13      == uint16_t(ControlID::KB_Numpad_Subtract));
		static_assert(VK_DECIMAL - 13       == uint16_t(ControlID::KB_Numpad_Decimal));
		static_assert(VK_DIVIDE - 13        == uint16_t(ControlID::KB_Numpad_Divide));
		static_assert(VK_F1 - 13            == uint16_t(ControlID::KB_F1));
		// ... function keys
		static_assert(VK_F12 - 13           == uint16_t(ControlID::KB_F12));

		static_assert(VK_LSHIFT - 43        == uint16_t(ControlID::KB_LShift));
		static_assert(VK_RSHIFT - 43        == uint16_t(ControlID::KB_RShift));
		static_assert(VK_LCONTROL - 43      == uint16_t(ControlID::KB_LCtrl));
		static_assert(VK_RCONTROL - 43      == uint16_t(ControlID::KB_RCtrl));
		static_assert(VK_LMENU - 43         == uint16_t(ControlID::KB_LAlt));
		static_assert(VK_RMENU - 43         == uint16_t(ControlID::KB_RAlt));

		static_assert(VK_OEM_1 - 63         == uint16_t(ControlID::KB_Semicolon));
		static_assert(VK_OEM_PLUS - 63      == uint16_t(ControlID::KB_Plus));
		static_assert(VK_OEM_COMMA - 63     == uint16_t(ControlID::KB_Comma));
		static_assert(VK_OEM_MINUS - 63     == uint16_t(ControlID::KB_Minus));
		static_assert(VK_OEM_PERIOD - 63    == uint16_t(ControlID::KB_Period));
		static_assert(VK_OEM_2 - 63         == uint16_t(ControlID::KB_FSlash));
		static_assert(VK_OEM_3 - 63         == uint16_t(ControlID::KB_Tilde));

		static_assert(VK_OEM_4 - 89         == uint16_t(ControlID::KB_LBracket));
		static_assert(VK_OEM_5 - 89         == uint16_t(ControlID::KB_BSlash));
		static_assert(VK_OEM_6 - 89         == uint16_t(ControlID::KB_RBracket));
		static_assert(VK_OEM_7 - 89         == uint16_t(ControlID::KB_Quote));

		// Map the vkey to a ControlID
		switch (keycode)
		{
		case VK_BACK:       control = ControlID::KB_Backspace;      break;
		case VK_TAB:        control = ControlID::KB_Tab;            break;
		case VK_RETURN:     control = ControlID::KB_Enter;          break;
		case VK_PAUSE:      control = ControlID::KB_PauseBreak;     break;
		case VK_CAPITAL:    control = ControlID::KB_CapsLock;       break;
		case VK_ESCAPE:     control = ControlID::KB_Esc;            break;
		case VK_SNAPSHOT:   control = ControlID::KB_PrintScreen;    break;
		case VK_INSERT:     control = ControlID::KB_Insert;         break;
		case VK_DELETE:     control = ControlID::KB_Delete;         break;
		case VK_NUMLOCK:    control = ControlID::KB_NumLock;        break;
		case VK_SCROLL:     control = ControlID::KB_ScrollLock;     break;
		default:
			if      (keycode >=  VK_SPACE && keycode <= VK_DELETE)  { control = ControlID(keycode); }
			else if (keycode >=      0x30 && keycode <= 0x39)       { control = ControlID(keycode - 4); }
			else if (keycode >=		 0x41 && keycode <= 0x5C)       { control = ControlID(keycode - 11); }
			else if (keycode >= VK_LSHIFT && keycode <= VK_RMENU)   { control = ControlID(keycode - 43); }
			else if (keycode >=		 0x60 && keycode <= VK_F12)     { control = ControlID(keycode - 13); }
			else if (keycode >=  VK_OEM_1 && keycode <= VK_OEM_3)   { control = ControlID(keycode - 63); }
			else if (keycode >=  VK_OEM_4 && keycode <= VK_OEM_7)   { control = ControlID(keycode - 89); }
			break;
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
	else if (ridata->header.dwType == RIM_TYPEMOUSE)
	{
		// TODO: Handle mouse events
	}

	return true;
}

void Enterprise::Input::GetRawInput()
{
	// XInput
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		dwResult = XInputGetState(i, &state);

		if (state.dwPacketNumber == xinputPrevPacketNo[i])
		{
			// Duplicate the existing buffer the frame that the packet number stops updating
			if (!xinputAlreadyCopiedBuffer[i])
			{
				if (xinputToGamepadID[i] < isGamepadIDActive.size())
				{
					// Copy the previous buffer to the current buffer if no packet change is detected
					Input::gpBuffer[xinputToGamepadID[i]].buttons[currentBuffer] =
						Input::gpBuffer[xinputToGamepadID[i]].buttons[!currentBuffer];

					for (size_t j = 0;
						 j < (size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1); 
						 j++)
					{
						Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer][j] =
							Input::gpBuffer[xinputToGamepadID[i]].axes[!currentBuffer][j];
					}
				}
				xinputAlreadyCopiedBuffer[i] = true;
			}
			continue;
		}
		else
		{
			xinputAlreadyCopiedBuffer[i] = false;
			xinputPrevPacketNo[i] = state.dwPacketNumber;
		}

		if (dwResult == ERROR_SUCCESS) // Controller is connected
		{
			if (!xinputWasConnected[i])
			{
				xinputWasConnected[i] = true;

				// Assign the controller a ControllerID.
				auto it = std::find(isGamepadIDActive.begin(), 
									isGamepadIDActive.end(), 
									false);
				if (it != isGamepadIDActive.end())
				{
					xinputToGamepadID[i] = it - isGamepadIDActive.begin();
					*it = true;
				}
				else
				{
					xinputToGamepadID[i] = isGamepadIDActive.size();
					isGamepadIDActive.push_back(true);
					Input::gpBuffer.emplace_back();
				}

				EP_INFO("Gamepad connected.  ControllerID: {}", xinputToGamepadID[i] + 1);
				Events::Dispatch(HN("ControllerWake"), ControllerID(xinputToGamepadID[i] + 1));
			}

			// Buttons
			{
				// Ensure that the XInput bit masks have the expected relationship to the ControlID enum values
				static_assert(XINPUT_GAMEPAD_DPAD_UP        == BIT(uint16_t(ControlID::GP_Dpad_Up)));
				static_assert(XINPUT_GAMEPAD_DPAD_DOWN      == BIT(uint16_t(ControlID::GP_Dpad_Down)));
				static_assert(XINPUT_GAMEPAD_DPAD_LEFT      == BIT(uint16_t(ControlID::GP_Dpad_Left)));
				static_assert(XINPUT_GAMEPAD_DPAD_RIGHT     == BIT(uint16_t(ControlID::GP_Dpad_Right)));
				static_assert(XINPUT_GAMEPAD_START          == BIT(uint16_t(ControlID::GP_Menu)));
				static_assert(XINPUT_GAMEPAD_BACK           == BIT(uint16_t(ControlID::GP_Options)));
				static_assert(XINPUT_GAMEPAD_LEFT_THUMB     == BIT(uint16_t(ControlID::GP_LStick_Click)));
				static_assert(XINPUT_GAMEPAD_RIGHT_THUMB    == BIT(uint16_t(ControlID::GP_RStick_Click)));
				static_assert(XINPUT_GAMEPAD_LEFT_SHOULDER  == BIT(uint16_t(ControlID::GP_LShoulder)));
				static_assert(XINPUT_GAMEPAD_RIGHT_SHOULDER == BIT(uint16_t(ControlID::GP_RShoulder)));
				static_assert(XINPUT_GAMEPAD_A              == BIT(uint16_t(ControlID::GP_FaceButton_Down) + 2));
				static_assert(XINPUT_GAMEPAD_B              == BIT(uint16_t(ControlID::GP_FaceButton_Right) + 2));
				static_assert(XINPUT_GAMEPAD_X              == BIT(uint16_t(ControlID::GP_FaceButton_Left) + 2));
				static_assert(XINPUT_GAMEPAD_Y              == BIT(uint16_t(ControlID::GP_FaceButton_Up) + 2));

				uint16_t convertedbuttons = state.Gamepad.wButtons &
					(
						XINPUT_GAMEPAD_DPAD_UP |
						XINPUT_GAMEPAD_DPAD_DOWN |
						XINPUT_GAMEPAD_DPAD_LEFT |
						XINPUT_GAMEPAD_DPAD_RIGHT |
						XINPUT_GAMEPAD_START |
						XINPUT_GAMEPAD_BACK |
						XINPUT_GAMEPAD_LEFT_THUMB |
						XINPUT_GAMEPAD_RIGHT_THUMB |
						XINPUT_GAMEPAD_LEFT_SHOULDER |
						XINPUT_GAMEPAD_RIGHT_SHOULDER
					);

				convertedbuttons |= (state.Gamepad.wButtons &
					(
						XINPUT_GAMEPAD_A |
						XINPUT_GAMEPAD_B |
						XINPUT_GAMEPAD_X |
						XINPUT_GAMEPAD_Y
					)) >> 2;

				Input::gpBuffer[xinputToGamepadID[i]].buttons[currentBuffer] = convertedbuttons;
			}

			// TODO: Add data-driven range normalization and dead zones for triggers and sticks.

			// Triggers
			{
				float trigger = float(max(state.Gamepad.bLeftTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 0));
				trigger /= float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
					[size_t(ControlID::GP_LTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = trigger;

				trigger = float(max(state.Gamepad.bRightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 0));
				trigger /= float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
					[size_t(ControlID::GP_RTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = trigger;
			}

			// Sticks
			{
				float thumbX = state.Gamepad.sThumbLX;
				float thumbY = state.Gamepad.sThumbLY;
				float magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

				thumbX /= magnitude;
				thumbY /= magnitude;

				if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;

					if (magnitude > 32767) magnitude = 32767;
					magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
					magnitude /= (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
				}
				else
				{
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;

					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				}

				thumbX = state.Gamepad.sThumbRX;
				thumbY = state.Gamepad.sThumbRY;
				magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

				thumbX /= magnitude;
				thumbY /= magnitude;

				if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
				{
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;

					if (magnitude > 32767) magnitude = 32767;
					magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
					magnitude /= (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
				}
				else
				{
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;

					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
					Input::gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
						[size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
				}
			}
		}
		else // Controller is disconnected
		{
			if (xinputWasConnected[i])
			{
				xinputWasConnected[i] = false;
				xinputPrevPacketNo[i] = 0;
				xinputAlreadyCopiedBuffer[i] = 0;
				isGamepadIDActive[xinputToGamepadID[i]] = false;

				Input::gpBuffer[xinputToGamepadID[i]] = GamePadBuffer();
				PlayerID disconnectedPlayerID = UnassignController(ControllerID(xinputToGamepadID[i] + 1));

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
				
				xinputToGamepadID[i] = EP_CONTROLLERID_NULL;
			}

			// TODO: Increase performance by implementing a timer to delay checks on disconnected controllers.
		}
	}
}

#endif // Win32
