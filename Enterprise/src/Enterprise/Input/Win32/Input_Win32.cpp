#include "EP_PCH.h"
#ifdef _WIN32

#include "Core.h"
#include "Enterprise/Input/Input.h"
#include "Enterprise/Input/InputEvents.h"

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

static bool ProcessRawInputEvent(Events::Event& e)
{
    RAWINPUT* data = Events::Unpack<RAWINPUT*>(e);

    //if (data->header.dwType == RIM_TYPEKEYBOARD)
    //{
    //    EP_TRACE("Kbd: make={} Flags:{} Reserved:{} ExtraInformation:{}, msg={} VK={}",
    //             data->data.keyboard.MakeCode,
    //             data->data.keyboard.Flags,
    //             data->data.keyboard.Reserved,
    //             data->data.keyboard.ExtraInformation,
    //             data->data.keyboard.Message,
    //             data->data.keyboard.VKey);
    //}
    //else if (data->header.dwType == RIM_TYPEMOUSE)
    //{
    //    EP_TRACE("Mouse: usFlags={} ulButtons={} usButtonFlags={} usButtonData={} ulRawButtons={} lLastX={} lLastY={} ulExtraInformation={}",
    //             data->data.mouse.usFlags,
    //             data->data.mouse.ulButtons,
    //             data->data.mouse.usButtonFlags,
    //             data->data.mouse.usButtonData,
    //             data->data.mouse.ulRawButtons,
    //             data->data.mouse.lLastX,
    //             data->data.mouse.lLastY,
    //             data->data.mouse.ulExtraInformation);
    //}

    return true;
}

void Enterprise::Input::PlatformInit()
{
    // Raw Input registration (keyboard and mouse)
    RAWINPUTDEVICE device[2];

    // Mouse
    device[0].usUsagePage = 0x01;
    device[0].usUsage = 0x02;
    device[0].dwFlags = 0;
    device[0].hwndTarget = NULL;

    // Keyboard
    device[1].usUsagePage = 0x01;
    device[1].usUsage = 0x06;
    device[1].dwFlags = RIDEV_NOLEGACY;
    device[1].hwndTarget = NULL;

    EP_VERIFY_NEQ(RegisterRawInputDevices(device, 2, sizeof(device[0])), FALSE);

    Events::SubscribeToType(EventTypes::Win32_RawInput, &ProcessRawInputEvent);
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
                Events::Dispatch(EventTypes::ControllerWake, ControllerID(xinputToGamepadID[i] + 1));
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
                    Events::Dispatch(EventTypes::ControllerDisconnect, disconnectedPlayerID);
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
