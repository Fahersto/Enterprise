#include "EP_PCH.h"
#include "Input.h"
#include "InputEvents.h"

#define EP_INPUT_BLOCKER uint_fast8_t(-1)

using Enterprise::Input;
using Enterprise::Events;


Input::KBMouseBuffer Input::kbmBuffer;
std::vector<Input::GamePadBuffer> Input::gpBuffer; // Accessed by ControllerID - 1.
std::vector<bool> Input::isGamepadConnected; // Accessed by ControllerID - 1.
bool Input::currentBuffer = 0;


static std::vector<Input::ControllerID> ControllerForPlayer;
static std::vector<bool> isPlayerInputBlocked;


struct ActionMapping
{
	Enterprise::ControlID controlID;
	float threshold;
	bool isDownAction;
};
struct AxisMapping
{
	Enterprise::ControlID controlID;
	float scale;
};
static std::map <std::pair<HashName, HashName>, std::vector<ActionMapping>> ActionMap;
static std::map <std::pair<HashName, HashName>, std::vector<AxisMapping>> AxisMap;
std::vector<Input::Binding> Input::BindingStack;


Input::PlayerID Input::NextAvailablePlayerID()
{
	auto ID_it = std::find(ControllerForPlayer.begin(), ControllerForPlayer.end(), EP_CONTROLLERID_NULL);
	if (ID_it != ControllerForPlayer.end())
	{
		return PlayerID(ID_it - ControllerForPlayer.begin());
	}
	else
	{
		PlayerID returnVal = ControllerForPlayer.size();
		ControllerForPlayer.push_back(EP_CONTROLLERID_NULL);
		isPlayerInputBlocked.push_back(false);
		return returnVal;
	}
}


void Input::AssignControllerToPlayer(PlayerID player, ControllerID controller)
{
	EP_ASSERTF(controller <= gpBuffer.size(),
			   "Attempted to assign a ControllerID before it was allocated a buffer.");
	EP_ASSERTF(player == 0 || controller != EP_CONTROLLERID_KBMOUSE, 
			   "Only PlayerID 0 can be assigned keyboard and mouse input.");

	if (player >= ControllerForPlayer.size())
	{
		// 'player' has not been used yet, so we don't have to break its associations.
		ControllerForPlayer.push_back(controller);
		isPlayerInputBlocked.push_back(false);
		if (controller == EP_CONTROLLERID_KBMOUSE)
		{
			// 'player' is known to be 0 in the case of the keyboard and mouse.
			kbmBuffer.isAssigned = true;
		}
		else
		{
			gpBuffer[controller - 1].assignedPlayer = player;
		}
	}
	else
	{
		// Break 'player''s previous association
		if (ControllerForPlayer[player] == EP_CONTROLLERID_KBMOUSE)
		{
			kbmBuffer.isAssigned = false;
		}
		else if (ControllerForPlayer[player] != EP_CONTROLLERID_NULL)
		{
			EP_ASSERTF(ControllerForPlayer[player] <= gpBuffer.size(),
					   "Attempting to break an association to an invalid ControllerID.");
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
		EP_ASSERTF(controller <= gpBuffer.size(), "Attempted to unassign unalloacted gamepad.");
		gpBuffer[controller - 1].assignedPlayer = EP_PLAYERID_NULL;
	}

	auto it = std::find(ControllerForPlayer.begin(), ControllerForPlayer.end(), controller);
	if (it != ControllerForPlayer.end())
	{
		(*it) = EP_CONTROLLERID_NULL;
	}
	return PlayerID(it - ControllerForPlayer.begin());
}


void Input::LoadContextFromFile(std::string filename, std::string contextname)
{
	// TODO: Actually load bindings from file here.
	// TODO: Assert that no action threshold is equivalent to 0.0 or 1.0.
	// Why?  All analog controls that can be treated as buttons are normalized 0 to 1, and the threshold can't
	// be crossed at extremes.

	// TODO: Reevaluate ActionMapping and AxisMapping structs.

	ActionMap[std::pair(HN(contextname), HN("Options"))].push_back(ActionMapping{ ControlID::GP_Options, 0.5f, false });
	ActionMap[std::pair(HN(contextname), HN("B"))].push_back(ActionMapping{ ControlID::GP_FaceButton_Right, 0.5f, true});
	AxisMap[std::pair(HN(contextname), HN("A"))].push_back(AxisMapping{ ControlID::GP_FaceButton_Down, 1.0f});
	AxisMap[std::pair(HN(contextname), HN("LT"))].push_back(AxisMapping{ ControlID::GP_LTrigger, 1.0f});
}

// TODO: Should the bind functions Assert that the binding is actually loaded?

void Input::BindAction(void(*callbackPtr)(PlayerID player),
					   bool isBlocking,
					   HashName contextName,
					   HashName actionName)
{
	BindingStack.emplace_back(
		Binding{ callbackPtr, contextName,
		actionName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		EP_PLAYERID_ALL, 0, isBlocking });
}

void Input::BindActionForPlayerID(void(*callbackPtr)(PlayerID player),
								PlayerID player,
								bool isBlocking,
								HashName contextName,
								HashName actionName)
{
	BindingStack.emplace_back(
		Binding{ callbackPtr, contextName,
		actionName, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		player, 0, isBlocking });
}

void Input::BlockLowerBindings()
{
	BindingStack.emplace_back(
		Binding{ nullptr, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		EP_PLAYERID_ALL, EP_INPUT_BLOCKER, false});
}

void Input::BlockLowerBindingsForPlayer(PlayerID player)
{
	BindingStack.emplace_back(
		Binding{ nullptr, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		player, EP_INPUT_BLOCKER, false });
}

void Input::PopBindings(size_t count)
{
	EP_ASSERT(count > 0);
	EP_ASSERT(count <= BindingStack.size());
	BindingStack.erase(BindingStack.end() - count, BindingStack.end());
}


void Input::CheckForControllerWake()
{
	// TODO: Iterate through all unassigned ControllerIDs and generate a wake event if a button has changed.

	// To determine if the controller is assigned, we only need a bool.

	if (!kbmBuffer.isAssigned)
	{
		// Check for key/button presses (not releases).
		if (kbmBuffer.keys[currentBuffer][0] > kbmBuffer.keys[!currentBuffer][0] ||
			kbmBuffer.keys[currentBuffer][1] > kbmBuffer.keys[!currentBuffer][1])
		{
			// Key pressed.  Released keys/mouse buttons do not trigger a wake condition.
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

			for (auto mappingIt = ActionMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[0])].begin();
				 mappingIt != ActionMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[0])].end();
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
				for (auto mappingIt = AxisMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[axisID])].begin();
					 mappingIt != AxisMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[axisID])].end();
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

			for (auto mappingIt = ActionMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[0])].begin();
				 mappingIt != ActionMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[0])].end();
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
				for (auto mappingIt = AxisMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[axisID])].begin();
					 mappingIt != AxisMap[std::pair(bindingIt->ContextName, bindingIt->ActionOrAxes[axisID])].end();
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


void Input::Init()
{
	// By default, Player 0 is assigned keyboard and mouse.
	AssignControllerToPlayer(PlayerID(0), EP_CONTROLLERID_KBMOUSE);
}


void Input::Update()
{
	// Clear player blockers
	for (auto& bBlocked : isPlayerInputBlocked)
	{
		bBlocked = false;
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

	// Swapping buffers is done last so OS events can populate the new buffer.
	currentBuffer = !currentBuffer;
}
