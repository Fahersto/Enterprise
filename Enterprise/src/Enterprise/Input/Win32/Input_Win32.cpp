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
static std::vector<bool> isGamepadIDTaken;

void Enterprise::Input::GetRawInput()
{
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
                if (xinputToGamepadID[i] < isGamepadConnected.size())
                {
                    // Copy the previous buffer to the current buffer if no packet change is detected
                    gpBuffer[xinputToGamepadID[i]].buttons[currentBuffer] =
                        gpBuffer[xinputToGamepadID[i]].buttons[!currentBuffer];

                    for (size_t j = 0;
                         j < (size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1); 
                         j++)
                    {
                        gpBuffer[xinputToGamepadID[i]].axes[currentBuffer][j] =
                            gpBuffer[xinputToGamepadID[i]].axes[!currentBuffer][j];
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
                auto it = std::find(isGamepadIDTaken.begin(), 
                                    isGamepadIDTaken.end(), 
                                    false);
                if (it != isGamepadIDTaken.end())
                {
                    xinputToGamepadID[i] = it - isGamepadIDTaken.begin();
                    *it = true;
                }
                else
                {
                    xinputToGamepadID[i] = isGamepadIDTaken.size();
                    isGamepadIDTaken.push_back(true);
                }

                // Make the engine savvy to the new controller
                if (xinputToGamepadID[i] < isGamepadConnected.size())
                {
                    isGamepadConnected[xinputToGamepadID[i]] = true;
                }
                else
                {
                    isGamepadConnected.push_back(true);
                    gpBuffer.emplace_back();
                }

                EP_TRACE("Controller connected!");
                Events::Dispatch(EventTypes::ControllerWake, ControllerID(xinputToGamepadID[i] + 1));
            }

            // Buttons
            {
                uint16_t convertedbuttons = 0;

                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Dpad_Up));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Dpad_Down));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Dpad_Left));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Dpad_Right));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Menu));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_Options));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_LStick_Click));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_RStick_Click));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_LShoulder));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_RShoulder));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_FaceButton_Down));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_FaceButton_Right));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_FaceButton_Left));
                if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
                    convertedbuttons |= BIT(uint16_t(ControlID::GP_FaceButton_Up));

                gpBuffer[xinputToGamepadID[i]].buttons[currentBuffer] = convertedbuttons;
            }

            // TODO: Add data-driven range normalization and dead zones for triggers and sticks.

            // Triggers
            {
                float trigger = float(max(state.Gamepad.bLeftTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 0));
                trigger /= float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
                gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                    [size_t(ControlID::GP_LTrigger) - size_t(ControlID::_EndOfGPButtons) - 1] = trigger;

                trigger = float(max(state.Gamepad.bRightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 0));
                trigger /= float(255 - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
                gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
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
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;

                    if (magnitude > 32767) magnitude = 32767;
                    magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
                    magnitude /= (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
                }
                else
                {
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;

                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_LStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
                }

                thumbX = state.Gamepad.sThumbRX;
                thumbY = state.Gamepad.sThumbRY;
                magnitude = sqrt(thumbX * thumbX + thumbY * thumbY);

                thumbX /= magnitude;
                thumbY /= magnitude;

                if (magnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                {
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY;

                    if (magnitude > 32767) magnitude = 32767;
                    magnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
                    magnitude /= (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbX * magnitude;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_Y) - size_t(ControlID::_EndOfGPButtons) - 1] = thumbY * magnitude;
                }
                else
                {
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_NormalX) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_NormalY) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;

                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
                        [size_t(ControlID::GP_RStick_X) - size_t(ControlID::_EndOfGPButtons) - 1] = 0.0f;
                    gpBuffer[xinputToGamepadID[i]].axes[currentBuffer]
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
                isGamepadIDTaken[xinputToGamepadID[i]] = false;
                isGamepadConnected[xinputToGamepadID[i]] = false;

                PlayerID player = UnassignController(ControllerID(xinputToGamepadID[i] + 1));
                gpBuffer[xinputToGamepadID[i]] = GamePadBuffer();

                Events::Dispatch(EventTypes::ControllerDisconnect, player);
                EP_TRACE("Controller disconnected!");
				
                xinputToGamepadID[i] = EP_CONTROLLERID_NULL;
            }

            // TODO: Increase performance by implementing a timer to delay checks on disconnected controllers.
        }
    }
}

#endif // Win32
