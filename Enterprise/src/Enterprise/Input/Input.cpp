#include "EP_PCH.h"
#include "Input.h"
#include "Enterprise/File/File.h"
#include "InputEvents.h"

#define EP_INPUT_BLOCKER uint_fast8_t(-1)


using Enterprise::Input;
using Enterprise::Events;


Input::KBMouseBuffer Input::kbmBuffer;
std::vector<Input::GamePadBuffer> Input::gpBuffer; // Accessed by ControllerID - 1.
std::vector<bool> Input::isGamepadConnected; // Accessed by ControllerID - 1.
bool Input::currentBuffer = 0;

/// Tracks the ControllerID used by each player.  Indexed by PlayerID.
static std::vector<Input::ControllerID> ControllerForPlayer;
/// Tracks whether a PlayerID's input was blocked earlier in the frame.  Indexed by PlayerID.
static std::vector<bool> isPlayerInputBlocked;


std::unordered_map<HashName, std::unordered_map<HashName, std::vector<Input::ActionMapping>>> Input::ActionMap;
std::unordered_map<HashName, std::unordered_map<HashName, std::vector<Input::AxisMapping>>> Input::AxisMap;
std::vector<Input::Binding> Input::BindingStack;


Input::PlayerID Input::GetNextPlayerID()
{
	auto ID_it = std::find(ControllerForPlayer.begin(), ControllerForPlayer.end(), EP_CONTROLLERID_NULL);
	if (ID_it != ControllerForPlayer.end())
	{
		return PlayerID(ID_it - ControllerForPlayer.begin());
	}
	else
	{
		return ControllerForPlayer.size();
	}
}


void Input::AssignControllerToPlayer(PlayerID player, ControllerID controller)
{
	EP_ASSERTF(player <= ControllerForPlayer.size(), "Input System: "
			   "Cannot assign ControllerID \'{}\' to ineligible PlayerID \'{}\'.",
			   controller, player);
	EP_ASSERTF(controller <= gpBuffer.size(), "Input System: "
			   "Attempted to assign ControllerID \'{}\' to PlayerID \'{}\' before its "
			   "buffer was allocated.", controller, player);
	EP_ASSERTF(player == 0 || controller != EP_CONTROLLERID_KBMOUSE, "Input System: "
			   "Attempted to assign non-zero PlayerID \'{}\' the keyboard and mouse.");

	if (player == ControllerForPlayer.size())
	{
		// 'player' has not been used before, so we don't have to break its associations.
		ControllerForPlayer.push_back(controller);
		isPlayerInputBlocked.push_back(false);
		if (controller == EP_CONTROLLERID_KBMOUSE)
		{
			// Keyboard and mouse can only ever be assigned to PlayerID(0), so this is tracked by a bool.
			kbmBuffer.isAssigned = true;
		}
		else
		{
			gpBuffer[controller - 1].assignedPlayer = player;
		}
	}
	else if (player < ControllerForPlayer.size())
	{
		// Break 'player''s previous association
		if (ControllerForPlayer[player] == EP_CONTROLLERID_KBMOUSE)
		{
			kbmBuffer.isAssigned = false;
		}
		else if (ControllerForPlayer[player] != EP_CONTROLLERID_NULL)
		{
			EP_ASSERTF(ControllerForPlayer[player] <= gpBuffer.size(), "Input System: "
					   "Attempted to break an association to an invalid ControllerID.");
			gpBuffer[ControllerForPlayer[player] - 1].assignedPlayer = EP_PLAYERID_NULL;
		}

		// We're reassigning controllerForPlayer[player] anyways, so don't bother to break that link here.

		// Assign controllerforplayer and the buffer.
		if (controller == EP_CONTROLLERID_KBMOUSE)
		{
			kbmBuffer.isAssigned = true;
		}
		else
		{
			gpBuffer[controller - 1].assignedPlayer = player;
		}
		ControllerForPlayer[player] = controller;
	}
}


Input::PlayerID Input::UnassignController(ControllerID controller)
{
	if (controller == EP_CONTROLLERID_KBMOUSE)
	{
		kbmBuffer.isAssigned = false;
	}
	else
	{
		EP_ASSERTF(controller <= gpBuffer.size(), "Input System: Attempted to unassign "
												  "unalloacted gamepad.");
		gpBuffer[controller - 1].assignedPlayer = EP_PLAYERID_NULL;
	}

	auto it = std::find(ControllerForPlayer.begin(), ControllerForPlayer.end(), controller);
	if (it != ControllerForPlayer.end())
	{
		(*it) = EP_CONTROLLERID_NULL;
	}
	return PlayerID(it - ControllerForPlayer.begin());
}


/// Converts a string into a ControlID.
/// @param str String to convert.
/// @return First: Whether the conversion was successful.  Second: The converted ControlID.
static std::pair<bool, Enterprise::ControlID> StringToControlID(const std::string& str)
{
	#define STRTOCONTROLIDIMPL(control) \
	if (str == #control) return std::pair(true, Enterprise::ControlID:: control)

	STRTOCONTROLIDIMPL(GP_Dpad_Up);
	STRTOCONTROLIDIMPL(GP_Dpad_Down);
	STRTOCONTROLIDIMPL(GP_Dpad_Left);
	STRTOCONTROLIDIMPL(GP_Dpad_Right);
	STRTOCONTROLIDIMPL(GP_Menu);
	STRTOCONTROLIDIMPL(GP_Options);
	STRTOCONTROLIDIMPL(GP_LStick_Click);
	STRTOCONTROLIDIMPL(GP_RStick_Click);
	STRTOCONTROLIDIMPL(GP_LShoulder);
	STRTOCONTROLIDIMPL(GP_RShoulder);
	STRTOCONTROLIDIMPL(GP_FaceButton_Down);
	STRTOCONTROLIDIMPL(GP_FaceButton_Right);
	STRTOCONTROLIDIMPL(GP_FaceButton_Left);
	STRTOCONTROLIDIMPL(GP_FaceButton_Up);
	STRTOCONTROLIDIMPL(GP_LTrigger);
	STRTOCONTROLIDIMPL(GP_RTrigger);
	STRTOCONTROLIDIMPL(GP_LStick_X);
	STRTOCONTROLIDIMPL(GP_LStick_Y);
	STRTOCONTROLIDIMPL(GP_LStick_NormalX);
	STRTOCONTROLIDIMPL(GP_LStick_NormalY);
	STRTOCONTROLIDIMPL(GP_RStick_X);
	STRTOCONTROLIDIMPL(GP_RStick_Y);
	STRTOCONTROLIDIMPL(GP_RStick_NormalX);
	STRTOCONTROLIDIMPL(GP_RStick_NormalY);
	STRTOCONTROLIDIMPL(KB_Backspace);
	STRTOCONTROLIDIMPL(KB_Tab);
	STRTOCONTROLIDIMPL(KB_Enter);
	STRTOCONTROLIDIMPL(KB_PauseBreak);
	STRTOCONTROLIDIMPL(KB_CapsLock);
	STRTOCONTROLIDIMPL(KB_Esc);
	STRTOCONTROLIDIMPL(KB_Space);
	STRTOCONTROLIDIMPL(KB_PageUp);
	STRTOCONTROLIDIMPL(KB_PageDown);
	STRTOCONTROLIDIMPL(KB_End);
	STRTOCONTROLIDIMPL(KB_Home);
	STRTOCONTROLIDIMPL(KB_Left);
	STRTOCONTROLIDIMPL(KB_Up);
	STRTOCONTROLIDIMPL(KB_Right);
	STRTOCONTROLIDIMPL(KB_Down);
	STRTOCONTROLIDIMPL(KB_PrintScreen);
	STRTOCONTROLIDIMPL(KB_Insert);
	STRTOCONTROLIDIMPL(KB_Delete);
	STRTOCONTROLIDIMPL(KB_0);
	STRTOCONTROLIDIMPL(KB_1);
	STRTOCONTROLIDIMPL(KB_2);
	STRTOCONTROLIDIMPL(KB_3);
	STRTOCONTROLIDIMPL(KB_4);
	STRTOCONTROLIDIMPL(KB_5);
	STRTOCONTROLIDIMPL(KB_6);
	STRTOCONTROLIDIMPL(KB_7);
	STRTOCONTROLIDIMPL(KB_8);
	STRTOCONTROLIDIMPL(KB_9);
	STRTOCONTROLIDIMPL(KB_A);
	STRTOCONTROLIDIMPL(KB_B);
	STRTOCONTROLIDIMPL(KB_C);
	STRTOCONTROLIDIMPL(KB_D);
	STRTOCONTROLIDIMPL(KB_E);
	STRTOCONTROLIDIMPL(KB_F);
	STRTOCONTROLIDIMPL(KB_G);
	STRTOCONTROLIDIMPL(KB_H);
	STRTOCONTROLIDIMPL(KB_I);
	STRTOCONTROLIDIMPL(KB_J);
	STRTOCONTROLIDIMPL(KB_K);
	STRTOCONTROLIDIMPL(KB_L);
	STRTOCONTROLIDIMPL(KB_M);
	STRTOCONTROLIDIMPL(KB_N);
	STRTOCONTROLIDIMPL(KB_O);
	STRTOCONTROLIDIMPL(KB_P);
	STRTOCONTROLIDIMPL(KB_Q);
	STRTOCONTROLIDIMPL(KB_R);
	STRTOCONTROLIDIMPL(KB_S);
	STRTOCONTROLIDIMPL(KB_T);
	STRTOCONTROLIDIMPL(KB_U);
	STRTOCONTROLIDIMPL(KB_V);
	STRTOCONTROLIDIMPL(KB_W);
	STRTOCONTROLIDIMPL(KB_X);
	STRTOCONTROLIDIMPL(KB_Y);
	STRTOCONTROLIDIMPL(KB_Z);
	STRTOCONTROLIDIMPL(KB_Super_Left);
	STRTOCONTROLIDIMPL(KB_Super_Right);
	STRTOCONTROLIDIMPL(KB_Numpad_0);
	STRTOCONTROLIDIMPL(KB_Numpad_1);
	STRTOCONTROLIDIMPL(KB_Numpad_2);
	STRTOCONTROLIDIMPL(KB_Numpad_3);
	STRTOCONTROLIDIMPL(KB_Numpad_4);
	STRTOCONTROLIDIMPL(KB_Numpad_5);
	STRTOCONTROLIDIMPL(KB_Numpad_6);
	STRTOCONTROLIDIMPL(KB_Numpad_7);
	STRTOCONTROLIDIMPL(KB_Numpad_8);
	STRTOCONTROLIDIMPL(KB_Numpad_9);
	STRTOCONTROLIDIMPL(KB_Numpad_Muliply);
	STRTOCONTROLIDIMPL(KB_Numpad_Add);
	STRTOCONTROLIDIMPL(KB_Numpad_Subtract);
	STRTOCONTROLIDIMPL(KB_Numpad_Decimal);
	STRTOCONTROLIDIMPL(KB_Numpad_Divide);
	STRTOCONTROLIDIMPL(KB_F1);
	STRTOCONTROLIDIMPL(KB_F2);
	STRTOCONTROLIDIMPL(KB_F3);
	STRTOCONTROLIDIMPL(KB_F4);
	STRTOCONTROLIDIMPL(KB_F5);
	STRTOCONTROLIDIMPL(KB_F6);
	STRTOCONTROLIDIMPL(KB_F7);
	STRTOCONTROLIDIMPL(KB_F8);
	STRTOCONTROLIDIMPL(KB_F9);
	STRTOCONTROLIDIMPL(KB_F10);
	STRTOCONTROLIDIMPL(KB_F11);
	STRTOCONTROLIDIMPL(KB_F12);
	STRTOCONTROLIDIMPL(KB_NumLock);
	STRTOCONTROLIDIMPL(KB_ScrollLock);
	STRTOCONTROLIDIMPL(KB_Shift_Left);
	STRTOCONTROLIDIMPL(KB_Shift_Right);
	STRTOCONTROLIDIMPL(KB_Ctrl_Left);
	STRTOCONTROLIDIMPL(KB_Ctrl_Right);
	STRTOCONTROLIDIMPL(KB_Menu_Left);
	STRTOCONTROLIDIMPL(KB_Menu_Right);
	STRTOCONTROLIDIMPL(Mouse_Button_1);
	STRTOCONTROLIDIMPL(Mouse_Button_2);
	STRTOCONTROLIDIMPL(Mouse_Button_3);
	STRTOCONTROLIDIMPL(Mouse_Button_4);
	STRTOCONTROLIDIMPL(Mouse_Button_5);
	STRTOCONTROLIDIMPL(Mouse_Wheel_Y);
	STRTOCONTROLIDIMPL(Mouse_Wheel_X);
	STRTOCONTROLIDIMPL(Mouse_Pointer_X);
	STRTOCONTROLIDIMPL(Mouse_Pointer_Y);
	STRTOCONTROLIDIMPL(Mouse_Delta_X);
	STRTOCONTROLIDIMPL(Mouse_Delta_Y);

	#undef STRTOCONTROLIDIMPL

	return std::pair(false, Enterprise::ControlID::_EndOfIDs);
}


/// Detects whether a string represents "Up" or "Down".
/// @param str String to convert.
/// @return First: Whether the conversion was successful.  Second: isDownAction bool value.
static std::pair<bool, bool> StringToDirBool(const std::string& str)
{
	if (str == "up" ||
		str == "Up" || 
		str == "UP")
	{
		return std::pair(true, false);
	}
	else if (str == "down" ||
			 str == "Down" ||
			 str == "DOWN")
	{
		return std::pair(true, true);
	}
	else
	{
		return std::pair(false, false);
	}
}


void Input::LoadContextsFromFile(std::string filename)
{
	File::INIReader ini(filename, true, "Input/Contexts/");

	for (HashName contextName : ini.Sections())
	{
		std::vector<std::unordered_map<HashName, std::string>> mappings =
			ini.GetMultiDictionary(contextName, HN("ActionMapping"));

		bool hasActionMappings = false;
		bool hasAxisMappings = false;

		if (!mappings.empty())
		{
			for (std::unordered_map<HashName, std::string>& map : mappings)
			{
				if (map.count(HN("name")) &&
					map.count(HN("control")) &&
					map.count(HN("dir")))
				{
					std::pair<bool, ControlID> control_converted = StringToControlID(map[HN("control")]);
					std::pair<bool, bool> dir_converted = StringToDirBool(map[HN("dir")]);
					std::pair<bool, float> threshold_converted;
					if (map.count(HN("threshold"))) // threshold is optional
					{
						threshold_converted = File::INIStringToFloat(map[HN("threshold")]);
					}
					else
					{
						threshold_converted = std::pair(true, 0.5f);
					}

					if (control_converted.first &&
						dir_converted.first &&
						threshold_converted.first)
					{
						ActionMap[contextName][HN(map[HN("name")])].emplace_back
						(
							ActionMapping
							{
								.controlID = control_converted.second,
								.threshold = threshold_converted.second,
								.isDownAction = dir_converted.second,
							}
						);

						hasActionMappings = true;
					}
					else
					{
						EP_WARN("Input System: An ActionMapping failed to load due to a subvalue conversion error.  "
								"The mapping was not loaded.\nFile: {}\nContext: {}", filename, SN(contextName));
						continue;
					}
				}
				else
				{
					EP_WARN("Input System: An ActionMapping failed to load due to a missing subkey.  "
							"The mapping was not loaded.\nFile: {}\nContext: {}", filename, SN(contextName));
					continue;
				}
			}
		}

		mappings = ini.GetMultiDictionary(contextName, HN("AxisMapping"));
		if (!mappings.empty())
		{
			for (std::unordered_map<HashName, std::string>& map : mappings)
			{
				if (map.count(HN("name")) &&
					map.count(HN("control")) &&
					map.count(HN("scale")))
				{
					std::pair<bool, ControlID> control_converted = StringToControlID(map[HN("control")]);
					std::pair<bool, float> scale_converted = File::INIStringToFloat(map[HN("scale")]);

					if (control_converted.first &&
						scale_converted.first)
					{
						AxisMap[contextName][HN(map[HN("name")])].emplace_back
						(
							AxisMapping
							{
								.controlID = control_converted.second,
								.scale = scale_converted.second
							}
						);

						hasAxisMappings = true;
					}
					else
					{
						EP_WARN("Input System: An AxisMapping failed to load due to a subvalue conversion error.  "
								"The mapping was not loaded.\nFile: {}\nContext: {}", filename, SN(contextName));
						continue;
					}
				}
				else
				{
					EP_WARN("Input System: An AxisMapping failed to load due to a missing subkey.  "
							"The mapping was not loaded.\nFile: {}\nContext: {}", filename, SN(contextName));
					continue;
				}
			}
		}

		if (!hasActionMappings && !hasAxisMappings)
		{
			EP_WARN("Input System: Input context \"{}\" contains no valid Action or Axis mappings."
					"  File: {}", SN(contextName), filename);
		}
	}
}


void Input::BindAction(void(*callbackPtr)(PlayerID player),
								PlayerID player,
								bool isBlocking,
								HashName contextName,
								HashName actionName)
{
	EP_ASSERTF(player != EP_PLAYERID_NULL, "Input System: Attempted to bind Action to EP_PLAYERID_NULL.");

	if (ActionMap[contextName][actionName].size() == 0)
	{
		EP_ERROR("Input System: Bound Action \"{}\" has no loaded ActionMappings "
				 "and will never trigger.  Context Name: {}", SN(actionName), SN(contextName));
	}

	BindingStack.emplace_back(
		Binding{ (void*)callbackPtr, contextName,
		actionName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		player, 0, isBlocking });
}


void Input::BlockLowerBindings(PlayerID player)
{
	EP_ASSERTF(player != EP_PLAYERID_NULL, "Input System: Attempted to apply input blocker for "
			   "EP_PLAYERID_NULL.");

	BindingStack.emplace_back(
		Binding{ nullptr, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		player, EP_INPUT_BLOCKER, false });
}


void Input::PopBindings(size_t count)
{
	EP_ASSERTF(count <= BindingStack.size(), "Input System: Attempted to pop more bindings "
			   "than were stored in BindingStack.");

	if (count == 0)
	{
		EP_WARN("Input System: Attempted to pop zero bindings from BindingStack.");
	}

	BindingStack.erase(BindingStack.end() - count, BindingStack.end());
}


void Input::CheckForControllerWake()
{
	if (!kbmBuffer.isAssigned)
	{
		// Check for key/button presses (not releases).
		if (kbmBuffer.keys[currentBuffer][0] > kbmBuffer.keys[!currentBuffer][0] ||
			kbmBuffer.keys[currentBuffer][1] > kbmBuffer.keys[!currentBuffer][1])
		{
			Events::Dispatch(EventTypes::ControllerWake, EP_CONTROLLERID_KBMOUSE);
		}
		// Check for axes changes.
		else
		{
			// TODO: Implement axes wake after determining how mouse input is normalized.
		}
	}

	for (auto gpIt = gpBuffer.begin();
		 gpIt != gpBuffer.end();
		 ++gpIt)
	{
		if (gpIt->assignedPlayer == EP_PLAYERID_NULL)
		{
			// Check for button presses
			if (gpIt->buttons[currentBuffer] > gpIt->buttons[!currentBuffer])
			{
				Events::Dispatch(EventTypes::ControllerWake, ControllerID(gpIt - gpBuffer.begin() + 1));
			}
			// Check for axes changes
			else
			{
				for (int i = 0; 
					 i < (size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1); 
					 i++)
				{
					if (gpIt->axes[currentBuffer][i] != gpIt->axes[!currentBuffer][i])
					{
						Events::Dispatch(EventTypes::ControllerWake, ControllerID(gpIt - gpBuffer.begin() + 1));
						break;
					}
				}
			}
		}
	}
}


/// Helper function that binds to and invokes an Axis callback.
/// @param callback A pointer to the callback function.
/// @param player The PlayerID for the call.
/// @param numOfAxes The number of float parameters in this callback.
/// @param Axes An array containing the axes values for this binding.
static void InvokeAxisBindingCallback(void* callback,
									  Input::PlayerID player, 
									  int numOfAxes, 
									  float Axes[Enterprise::Constants::MaxAxesPerBinding])
{
	switch (numOfAxes)
	{
	case 1:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0]);
	}
	break;
	case 2:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1]);
	}
	break;
	case 3:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2]);
	}
	break;
	case 4:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3]);
	}
	break;
	case 5:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4]);
	}
	break;
	case 6:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4], Axes[5]);
	}
	break;
	case 7:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4], Axes[5], Axes[6]);
	}
	break;
	case 8:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4], Axes[5], Axes[6], Axes[7]);
	}
	break;
	case 9:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4], Axes[5], Axes[6], Axes[7], Axes[8]);
	}
	break;
	case 10:
	{
		typedef void(*axisCallbackPtr)(Input::PlayerID player, float, float, float, float, float, float, float, float, float, float);
		axisCallbackPtr boundFnPtr = axisCallbackPtr(callback);
		(*boundFnPtr)(player, Axes[0], Axes[1], Axes[2], Axes[3], Axes[4], Axes[5], Axes[6], Axes[7], Axes[8], Axes[9]);
	}
	break;
	default:
		EP_ASSERT_NOENTRY();
		break;
	}
}


void Input::ProcessKeyboardBinding(const std::reverse_iterator<std::vector<Binding>::iterator>& bindingIt)
{
	if (bindingIt->NumOfAxes == 0) // Actions
	{
		if (!isPlayerInputBlocked[0])
		{
			bool isActionTriggered = false;

			for (auto mappingIt = ActionMap[bindingIt->ContextName][bindingIt->ActionOrAxes[0]].begin();
				 mappingIt != ActionMap[bindingIt->ContextName][bindingIt->ActionOrAxes[0]].end();
				 ++mappingIt)
			{
				if (mappingIt->controlID > ControlID::_EndOfGPAxes)
				{
					if (mappingIt->controlID < ControlID::_EndOfKBMouseButtons &&
						size_t(mappingIt->controlID) < 64 &&
						!(kbmBuffer.keys_blockstatus[0] & BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1)))
					{
						// Unblocked button in low-order word.
						uint64_t control_flag = BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1);

						isActionTriggered |=
							((kbmBuffer.keys[currentBuffer][0] & control_flag) &&
							 !(kbmBuffer.keys[!currentBuffer][0] & control_flag) &&
							 mappingIt->isDownAction) ||
							(!(kbmBuffer.keys[currentBuffer][0] & control_flag) &&
							 (kbmBuffer.keys[!currentBuffer][0] & control_flag) &&
							 !mappingIt->isDownAction);

						kbmBuffer.keys_blockstatus[0] |= control_flag * bindingIt->bBlocking;
					}
					else if (mappingIt->controlID < ControlID::_EndOfKBMouseButtons &&
							 size_t(mappingIt->controlID) >= 64 &&
							 !(kbmBuffer.keys_blockstatus[1] & BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1 - 64)))
					{
						// Unblocked button in high-order word.
						uint64_t control_flag = BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1 - 64);

						isActionTriggered |=
							((kbmBuffer.keys[currentBuffer][1] & control_flag) &&
							 !(kbmBuffer.keys[!currentBuffer][1] & control_flag) &&
							 mappingIt->isDownAction) ||
							(!(kbmBuffer.keys[currentBuffer][1] & control_flag) &&
							 (kbmBuffer.keys[!currentBuffer][1] & control_flag) &&
							 !mappingIt->isDownAction);

						kbmBuffer.keys_blockstatus[1] |= control_flag * bindingIt->bBlocking;
					}
					else if (!kbmBuffer.axes_blockstatus[size_t(mappingIt->controlID) - size_t(ControlID::_EndOfKBMouseButtons) - 1])
					{
						// Unblocked axis.
						size_t control = size_t(mappingIt->controlID) - size_t(ControlID::_EndOfKBMouseButtons) - 1;

						isActionTriggered |=
							((kbmBuffer.axes[currentBuffer][control] > mappingIt->threshold) &&
							 (kbmBuffer.axes[!currentBuffer][control] < mappingIt->threshold) &&
							 mappingIt->isDownAction && mappingIt->threshold > 0) ||
							((kbmBuffer.axes[currentBuffer][control] < mappingIt->threshold) &&
							 (kbmBuffer.axes[!currentBuffer][control] > mappingIt->threshold) &&
							 mappingIt->isDownAction && mappingIt->threshold < 0) ||
							((kbmBuffer.axes[currentBuffer][control] < mappingIt->threshold) &&
							 (kbmBuffer.axes[!currentBuffer][control] > mappingIt->threshold) &&
							 !mappingIt->isDownAction && mappingIt->threshold > 0) ||
							((kbmBuffer.axes[currentBuffer][control] > mappingIt->threshold) &&
							 (kbmBuffer.axes[!currentBuffer][control] < mappingIt->threshold) &&
							 !mappingIt->isDownAction && mappingIt->threshold < 0);

						kbmBuffer.axes_blockstatus[control] |= bindingIt->bBlocking;
					}
					else
					{
						// Blocked control.
						continue;
					}
				}
				else
				{
					// Not a gamepad binding.
					continue;
				}
			}

			if (isActionTriggered)
			{
				typedef void(*actionCallbackPtr)(PlayerID player);
				actionCallbackPtr callbackPtr = actionCallbackPtr(bindingIt->callback);
				(*callbackPtr)(0);
			}
		}
	}
	else if (bindingIt->NumOfAxes == EP_INPUT_BLOCKER) // Blockers
	{
		isPlayerInputBlocked[0] = true;
	}
	else // Axes
	{
		EP_ASSERT_SLOW(bindingIt->NumOfAxes <= Constants::MaxAxesPerBinding);
		float outAxes[Constants::MaxAxesPerBinding] = { 0 };

		if (!isPlayerInputBlocked[0])
		{
			for (int axisID = 0; axisID < bindingIt->NumOfAxes; axisID++)
			{
				for (auto mappingIt = AxisMap[bindingIt->ContextName][bindingIt->ActionOrAxes[axisID]].begin();
					 mappingIt != AxisMap[bindingIt->ContextName][bindingIt->ActionOrAxes[axisID]].end();
					 ++mappingIt)
				{
					// At this point, we're iterating through every mappingIt in an axis, then puting it in out axes.

					if (mappingIt->controlID > ControlID::_EndOfGPAxes)
					{
						if (mappingIt->controlID < ControlID::_EndOfKBMouseButtons &&
							size_t(mappingIt->controlID) < 64 &&
							!(kbmBuffer.keys_blockstatus[0] & BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1)))
						{
							// Unblocked key, low-order word.
							uint64_t control_flag = BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1);

							outAxes[axisID] +=
								(kbmBuffer.keys[currentBuffer][0] & control_flag ? 1.0f : 0.0f) * mappingIt->scale;

							kbmBuffer.keys_blockstatus[0] |= control_flag * bindingIt->bBlocking;
						}
						else if (mappingIt->controlID < ControlID::_EndOfKBMouseButtons &&
								 size_t(mappingIt->controlID) >= 64 &&
								 !(kbmBuffer.keys_blockstatus[1] & BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1 - 64)))
						{
							// Unblocked key, high-order word.
							uint64_t control_flag = BIT(uint64_t(mappingIt->controlID) - uint64_t(ControlID::_EndOfGPAxes) - 1 - 64);

							outAxes[axisID] +=
								(kbmBuffer.keys[currentBuffer][1] & control_flag ? 1.0f : 0.0f) * mappingIt->scale;

							kbmBuffer.keys_blockstatus[1] |= control_flag * bindingIt->bBlocking;
						}
						else if (!kbmBuffer.axes_blockstatus[size_t(mappingIt->controlID) - size_t(ControlID::_EndOfKBMouseButtons) - 1])
						{
							// Unblocked axis.
							size_t control = size_t(mappingIt->controlID) - size_t(ControlID::_EndOfKBMouseButtons) - 1;

							outAxes[axisID] += kbmBuffer.axes[currentBuffer][control] * mappingIt->scale;
							kbmBuffer.axes_blockstatus[control] |= bindingIt->bBlocking;
						}
						else
						{
							// Blocked control.
							continue;
						}
					}
					else
					{
						// Not a keyboard/mouse binding.
						continue;
					}
				}
			}
		}

		InvokeAxisBindingCallback(bindingIt->callback, 0, bindingIt->NumOfAxes, outAxes);
	}
}


void Input::ProcessGamepadBinding(const std::reverse_iterator<std::vector<Binding>::iterator>& bindingIt, 
											  PlayerID player)
{
	ControllerID gamepad = ControllerForPlayer[player] - 1;

	if (bindingIt->NumOfAxes == 0) // Actions
	{
		if (!isPlayerInputBlocked[player])
		{
			bool isActionTriggered = false;

			for (auto mappingIt = ActionMap[bindingIt->ContextName][bindingIt->ActionOrAxes[0]].begin();
				 mappingIt != ActionMap[bindingIt->ContextName][bindingIt->ActionOrAxes[0]].end();
				 ++mappingIt)
			{
				if (mappingIt->controlID < ControlID::_EndOfGPAxes)
				{
					if (mappingIt->controlID < ControlID::_EndOfGPButtons &&
						!(gpBuffer[gamepad].buttons_blockstatus & BIT(size_t(mappingIt->controlID))))
					{
						// Unblocked button.
						uint16_t control_flag = BIT(uint16_t(mappingIt->controlID));

						isActionTriggered |=
							((gpBuffer[gamepad].buttons[currentBuffer] & control_flag) &&
							!(gpBuffer[gamepad].buttons[!currentBuffer] & control_flag) &&
							mappingIt->isDownAction) ||
							(!(gpBuffer[gamepad].buttons[currentBuffer] & control_flag) &&
							 (gpBuffer[gamepad].buttons[!currentBuffer] & control_flag) &&
							 !mappingIt->isDownAction);

						gpBuffer[gamepad].buttons_blockstatus |= control_flag * bindingIt->bBlocking;
					}
					else if (!gpBuffer[gamepad]
							   .axes_blockstatus[size_t(mappingIt->controlID) - size_t(ControlID::_EndOfGPButtons) - 1])
					{
						// Unblocked axis.
						size_t control = size_t(mappingIt->controlID) - size_t(ControlID::_EndOfGPButtons) - 1;

						isActionTriggered |=
							((gpBuffer[gamepad].axes[currentBuffer][control] > mappingIt->threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] < mappingIt->threshold) &&
							 mappingIt->isDownAction && mappingIt->threshold > 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] < mappingIt->threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] > mappingIt->threshold) &&
							 mappingIt->isDownAction && mappingIt->threshold < 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] < mappingIt->threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] > mappingIt->threshold) &&
							 !mappingIt->isDownAction && mappingIt->threshold > 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] > mappingIt->threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] < mappingIt->threshold) &&
							 !mappingIt->isDownAction && mappingIt->threshold < 0);

						gpBuffer[gamepad].axes_blockstatus[control] |= bindingIt->bBlocking;
					}
					else
					{
						// Blocked control.
						continue;
					}
				}
				else
				{
					// Not a gamepad binding.
					continue;
				}
			}

			if (isActionTriggered)
			{
				typedef void(*actionCallbackPtr)(PlayerID player);
				actionCallbackPtr callbackPtr = actionCallbackPtr(bindingIt->callback);
				(*callbackPtr)(player);
			}
		}
	}
	else if (bindingIt->NumOfAxes == EP_INPUT_BLOCKER) // Blockers
	{
		isPlayerInputBlocked[player] = true;
	}
	else // Axes
	{
		EP_ASSERT_SLOW(bindingIt->NumOfAxes <= Constants::MaxAxesPerBinding);
		float outAxes[Constants::MaxAxesPerBinding] = { 0 };

		if (!isPlayerInputBlocked[player])
		{
			for (int axisID = 0; axisID < bindingIt->NumOfAxes; axisID++)
			{
				for (auto mappingIt = AxisMap[bindingIt->ContextName][bindingIt->ActionOrAxes[axisID]].begin();
					 mappingIt != AxisMap[bindingIt->ContextName][bindingIt->ActionOrAxes[axisID]].end();
					 ++mappingIt)
				{
					if (mappingIt->controlID < ControlID::_EndOfGPAxes)
					{
						if (mappingIt->controlID < ControlID::_EndOfGPButtons &&
							!(gpBuffer[gamepad].buttons_blockstatus & BIT(size_t(mappingIt->controlID))))
						{
							// Unblocked button.
							uint16_t control_flag = BIT(uint16_t(mappingIt->controlID));

							outAxes[axisID] += 
								(gpBuffer[gamepad].buttons[currentBuffer] & control_flag ? 1.0f : 0.0f) * mappingIt->scale;

							gpBuffer[gamepad].buttons_blockstatus |= control_flag * bindingIt->bBlocking;
						}
						else if (!gpBuffer[gamepad]
								 .axes_blockstatus[size_t(mappingIt->controlID) - size_t(ControlID::_EndOfGPButtons) - 1])
						{
							// Unblocked axis.
							size_t control = size_t(mappingIt->controlID) - size_t(ControlID::_EndOfGPButtons) - 1;

							outAxes[axisID] += gpBuffer[gamepad].axes[currentBuffer][control] * mappingIt->scale;
							gpBuffer[gamepad].axes_blockstatus[control] |= bindingIt->bBlocking;
						}
						else
						{
							// Blocked control.
							continue;
						}
					}
					else
					{
						// Not a gamepad binding.
						continue;
					}
				}
			}
		}

		InvokeAxisBindingCallback(bindingIt->callback, player, bindingIt->NumOfAxes, outAxes);
	}
}


void Input::ProcessBinding(const std::vector<Binding>::reverse_iterator& bindingIt)
{
	if (bindingIt->playerID == EP_PLAYERID_ALL)
	{
		if (ControllerForPlayer[0] == EP_CONTROLLERID_KBMOUSE)
		{
			ProcessKeyboardBinding(bindingIt);
		}
		else if (ControllerForPlayer[0] != EP_CONTROLLERID_NULL)
		{
			ProcessGamepadBinding(bindingIt, PlayerID(0));
		}

		for (PlayerID playerIt = 1; playerIt < ControllerForPlayer.size(); playerIt++)
		{
			// The other PlayerIDs can only be assigned gamepads, so KB/mouse check is necessary.
			if (ControllerForPlayer[playerIt] != EP_CONTROLLERID_NULL)
			{
				ProcessGamepadBinding(bindingIt, playerIt);
			}
		}
	}
	else
	{
		// Process the binding, if the PlayerID has been assigned a ControllerID
		if (bindingIt->playerID < ControllerForPlayer.size())
		{
			if (ControllerForPlayer[bindingIt->playerID] == EP_CONTROLLERID_KBMOUSE)
			{
				ProcessKeyboardBinding(bindingIt);
			}
			else if (ControllerForPlayer[bindingIt->playerID] != EP_CONTROLLERID_NULL)
			{
				ProcessGamepadBinding(bindingIt, bindingIt->playerID);
			}
		}
	}
}


static bool handlePlayer0Disconnect(Events::Event& e)
{
	// By default, Player 0 gets reassigned the keyboard and mouse automatically upon controller disconnect.
	// Devs can override this behavior in their games by handling the ControllerDisconnect event.
	Input::PlayerID player = Events::Unpack<Input::PlayerID>(e);
	if (player == Input::PlayerID(0))
	{
		Input::AssignControllerToPlayer(Input::PlayerID(0), EP_CONTROLLERID_KBMOUSE);
	}
	return true;
}


void Input::Init()
{
	// By default, Player 0 is assigned keyboard and mouse.
	AssignControllerToPlayer(PlayerID(0), EP_CONTROLLERID_KBMOUSE);
	// Automatically reassign the keyboard and mouse to PlayerID 0 upon controller disconnect.
	Events::SubscribeToType(EventTypes::ControllerDisconnect, &handlePlayer0Disconnect);
}


void Input::Update()
{
	// Clear player blockers
	for (auto bBlocked = isPlayerInputBlocked.begin();
		 bBlocked != isPlayerInputBlocked.end();
		 ++bBlocked)
	{
		*bBlocked = false;
	}

	// Clear keyboard blockers
	kbmBuffer.keys_blockstatus[0] = 0;
	kbmBuffer.keys_blockstatus[1] = 0;
	for (size_t i = 0; i < (size_t(ControlID::_EndOfIDs) - size_t(ControlID::_EndOfKBMouseButtons) - 1); i++)
	{
		kbmBuffer.axes_blockstatus[i] = false;
	}

	// Clear gamepad blockers
	for (auto& gamepadBufferIt : gpBuffer)
	{
		gamepadBufferIt.buttons_blockstatus = 0;

		for (size_t i = 0; i < (size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1); i++)
		{
			gamepadBufferIt.axes_blockstatus[i] = false;
		}
	}

	GetRawInput();
	CheckForControllerWake();

	// Dispatch mapped input
	for (auto it = BindingStack.rbegin(); it != BindingStack.rend(); ++it)
	{
		ProcessBinding(it);
	}

	// This is done last so OS events can populate the new buffer between frames.
	currentBuffer = !currentBuffer;
}
