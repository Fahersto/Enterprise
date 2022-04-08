#include "Enterprise/Input.h"
#include "Enterprise/File.h"
#include "Enterprise/Time.h"
#include "Enterprise/StateManager.h"

using Enterprise::Input;
using Enterprise::Events;
using Enterprise::Time;
using Enterprise::StateManager;

Input::KBMouseBuffer Input::kbmBuffer;
glm::vec2 Input::cursorPos;
std::vector<Input::GamePadBuffer> Input::gpBuffer; // Accessed by ControllerID - 2.
bool Input::currentBuffer = 0;

std::map<HashName, Input::Context> Input::contextRegistry;
std::forward_list<Input::Context> Input::contextStack[MaxInputContextLayers];
std::map<Input::ContextHandle, int> Input::layerOfContext;
static std::map<void*, std::map<HashName, std::vector<std::pair<Input::ActionCallbackPtr, StateManager::State*>>>> actionCallbacks;
std::map<void*, std::map<HashName, float>> Input::axisValues[2]; // Array index is Time::inFixedTimestep().
static float mouseAxisAccumulator[4] = { 0 };
static float mouseDeltaAccumulator = 0.0f;

static std::vector<Input::ControllerID> StreamBindings; // Indexed by StreamID.
static std::vector<bool> isStreamBlocked; // Indexed by StreamID.

void Input::BindController(ControllerID controller, StreamID stream)
{
	EP_ASSERTF(controller != 0, "Input::BindController(): 'controller' cannot be NULL.");
	EP_ASSERTF(stream != 0, "Input::BindController(): 'stream' cannot be NULL.");

	EP_ASSERTF(controller < gpBuffer.size() + 2, "Input System: Attempted to assign unallocated ControllerID.");
	EP_ASSERTF(stream == 1 || controller != ControllerID(1), "Input System: Attempted to assign keyboard and mouse "
			   "to invalid StreamID.  Keyboard and mouse can only be bound to StreamID 1.");

	if (stream >= StreamBindings.size())
	{
		// this stream has not been bound before

		while (stream != StreamBindings.size())
		{
			StreamBindings.push_back(ControllerID(0));
			isStreamBlocked.push_back(false);
		}

		StreamBindings.push_back(controller);
		isStreamBlocked.push_back(false);
		if (controller == ControllerID(1))
		{
			// Keyboard and mouse can only ever be assigned to StreamID 1, so this is tracked by a bool.
			kbmBuffer.isAssigned = true;
		}
		else
		{
			gpBuffer[controller - 2].assignedStream = stream;
		}
	}
	else
	{
		// Break the stream's previous binding
		if (StreamBindings[stream] == ControllerID(1))
		{
			kbmBuffer.isAssigned = false;
		}
		else if (StreamBindings[stream] != 0)
		{
			EP_ASSERTF(StreamBindings[stream] < gpBuffer.size() + 2, "Input System: "
					   "Attempted to break an association to an invalid ControllerID.");
			gpBuffer[StreamBindings[stream] - 2].assignedStream = StreamID(0);
		}

		// We're reassigning StreamBindings[stream] anyways, so we don't bother to break that link here.

		// Assign controllerforplayer and the buffer.
		if (controller == ControllerID(1))
		{
			kbmBuffer.isAssigned = true;
		}
		else
		{
			gpBuffer[controller - 2].assignedStream = stream;
		}
		StreamBindings[stream] = controller;
	}
}


Input::StreamID Input::UnbindController(ControllerID controller)
{
	EP_ASSERTF(controller != 0, "Input::UnbindController(): 'controller' cannot be NULL.");

	if (controller == ControllerID(1))
	{
		kbmBuffer.isAssigned = false;
	}
	else
	{
		EP_ASSERTF(controller < gpBuffer.size() + 2, "Input System: Attempted to unassign "
												  "unalloacted gamepad.");
		gpBuffer[controller - 2].assignedStream = StreamID(0);
	}

	auto it = std::find(StreamBindings.begin(), StreamBindings.end(), controller);
	if (it != StreamBindings.end())
	{
		(*it) = ControllerID(0);
		return StreamID(it - StreamBindings.begin());
	}
	else
	{
		return StreamID(0);
	}

	// TODO: Notify macOS platform code that the controller is unassigned, so GCController can unmark the player assignment.
}


/// Converts a string into a ControlID.
/// @param str String to convert.
/// @return First: Whether the conversion was successful.  Second: The converted ControlID.
static std::pair<bool, Enterprise::ControlID> StringToControlID(const std::string& str)
{
	/// @cond DOXYGEN_SKIP
	#define STRTOCONTROLIDIMPL(control) \
	if (str == #control) return std::pair(true, Enterprise::ControlID:: control)
	/// @endcond

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
	STRTOCONTROLIDIMPL(KB_LSuper);
	STRTOCONTROLIDIMPL(KB_RSuper);
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
	STRTOCONTROLIDIMPL(KB_Numpad_Multiply);
	STRTOCONTROLIDIMPL(KB_Numpad_Add);
	STRTOCONTROLIDIMPL(KB_Numpad_Enter);
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
	STRTOCONTROLIDIMPL(KB_LShift);
	STRTOCONTROLIDIMPL(KB_RShift);
	STRTOCONTROLIDIMPL(KB_LCtrl);
	STRTOCONTROLIDIMPL(KB_RCtrl);
	STRTOCONTROLIDIMPL(KB_LAlt);
	STRTOCONTROLIDIMPL(KB_RAlt);
	STRTOCONTROLIDIMPL(Mouse_Button_1);
	STRTOCONTROLIDIMPL(Mouse_Button_2);
	STRTOCONTROLIDIMPL(Mouse_Button_3);
	STRTOCONTROLIDIMPL(Mouse_Button_4);
	STRTOCONTROLIDIMPL(Mouse_Button_5);
    STRTOCONTROLIDIMPL(Mouse_Delta_X);
    STRTOCONTROLIDIMPL(Mouse_Delta_Y);
	STRTOCONTROLIDIMPL(Mouse_Wheel_Y);
    STRTOCONTROLIDIMPL(Mouse_Wheel_X);
    STRTOCONTROLIDIMPL(KB_Semicolon);
	STRTOCONTROLIDIMPL(KB_Plus);
	STRTOCONTROLIDIMPL(KB_Comma);
	STRTOCONTROLIDIMPL(KB_Minus);
	STRTOCONTROLIDIMPL(KB_Period);
	STRTOCONTROLIDIMPL(KB_FSlash);
	STRTOCONTROLIDIMPL(KB_Tilde);
	STRTOCONTROLIDIMPL(KB_LBracket);
	STRTOCONTROLIDIMPL(KB_BSlash);
	STRTOCONTROLIDIMPL(KB_RBracket);
	STRTOCONTROLIDIMPL(KB_Quote);

	#undef STRTOCONTROLIDIMPL

	return std::pair(false, Enterprise::ControlID::_EndOfIDs);
}

/// Converts a ControlID into a string.
/// @param id ControlID to convert
/// @return The string name of the ControlID.
static std::string ControlIDToString(Enterprise::ControlID id)
{
	/// @cond DOXYGEN_SKIP
	#define CONTROLIDTOSTRIMPL(control) \
	if (id == Enterprise::ControlID:: control) return #control
	/// @endcond

	CONTROLIDTOSTRIMPL(GP_Dpad_Up);
	CONTROLIDTOSTRIMPL(GP_Dpad_Down);
	CONTROLIDTOSTRIMPL(GP_Dpad_Left);
	CONTROLIDTOSTRIMPL(GP_Dpad_Right);
	CONTROLIDTOSTRIMPL(GP_Menu);
	CONTROLIDTOSTRIMPL(GP_Options);
	CONTROLIDTOSTRIMPL(GP_LStick_Click);
	CONTROLIDTOSTRIMPL(GP_RStick_Click);
	CONTROLIDTOSTRIMPL(GP_LShoulder);
	CONTROLIDTOSTRIMPL(GP_RShoulder);
	CONTROLIDTOSTRIMPL(GP_FaceButton_Down);
	CONTROLIDTOSTRIMPL(GP_FaceButton_Right);
	CONTROLIDTOSTRIMPL(GP_FaceButton_Left);
	CONTROLIDTOSTRIMPL(GP_FaceButton_Up);
	CONTROLIDTOSTRIMPL(GP_LTrigger);
	CONTROLIDTOSTRIMPL(GP_RTrigger);
	CONTROLIDTOSTRIMPL(GP_LStick_X);
	CONTROLIDTOSTRIMPL(GP_LStick_Y);
	CONTROLIDTOSTRIMPL(GP_LStick_NormalX);
	CONTROLIDTOSTRIMPL(GP_LStick_NormalY);
	CONTROLIDTOSTRIMPL(GP_RStick_X);
	CONTROLIDTOSTRIMPL(GP_RStick_Y);
	CONTROLIDTOSTRIMPL(GP_RStick_NormalX);
	CONTROLIDTOSTRIMPL(GP_RStick_NormalY);
	CONTROLIDTOSTRIMPL(KB_Backspace);
	CONTROLIDTOSTRIMPL(KB_Tab);
	CONTROLIDTOSTRIMPL(KB_Enter);
	CONTROLIDTOSTRIMPL(KB_PauseBreak);
	CONTROLIDTOSTRIMPL(KB_CapsLock);
	CONTROLIDTOSTRIMPL(KB_Esc);
	CONTROLIDTOSTRIMPL(KB_Space);
	CONTROLIDTOSTRIMPL(KB_PageUp);
	CONTROLIDTOSTRIMPL(KB_PageDown);
	CONTROLIDTOSTRIMPL(KB_End);
	CONTROLIDTOSTRIMPL(KB_Home);
	CONTROLIDTOSTRIMPL(KB_Left);
	CONTROLIDTOSTRIMPL(KB_Up);
	CONTROLIDTOSTRIMPL(KB_Right);
	CONTROLIDTOSTRIMPL(KB_Down);
	CONTROLIDTOSTRIMPL(KB_PrintScreen);
	CONTROLIDTOSTRIMPL(KB_Insert);
	CONTROLIDTOSTRIMPL(KB_Delete);
	CONTROLIDTOSTRIMPL(KB_0);
	CONTROLIDTOSTRIMPL(KB_1);
	CONTROLIDTOSTRIMPL(KB_2);
	CONTROLIDTOSTRIMPL(KB_3);
	CONTROLIDTOSTRIMPL(KB_4);
	CONTROLIDTOSTRIMPL(KB_5);
	CONTROLIDTOSTRIMPL(KB_6);
	CONTROLIDTOSTRIMPL(KB_7);
	CONTROLIDTOSTRIMPL(KB_8);
	CONTROLIDTOSTRIMPL(KB_9);
	CONTROLIDTOSTRIMPL(KB_A);
	CONTROLIDTOSTRIMPL(KB_B);
	CONTROLIDTOSTRIMPL(KB_C);
	CONTROLIDTOSTRIMPL(KB_D);
	CONTROLIDTOSTRIMPL(KB_E);
	CONTROLIDTOSTRIMPL(KB_F);
	CONTROLIDTOSTRIMPL(KB_G);
	CONTROLIDTOSTRIMPL(KB_H);
	CONTROLIDTOSTRIMPL(KB_I);
	CONTROLIDTOSTRIMPL(KB_J);
	CONTROLIDTOSTRIMPL(KB_K);
	CONTROLIDTOSTRIMPL(KB_L);
	CONTROLIDTOSTRIMPL(KB_M);
	CONTROLIDTOSTRIMPL(KB_N);
	CONTROLIDTOSTRIMPL(KB_O);
	CONTROLIDTOSTRIMPL(KB_P);
	CONTROLIDTOSTRIMPL(KB_Q);
	CONTROLIDTOSTRIMPL(KB_R);
	CONTROLIDTOSTRIMPL(KB_S);
	CONTROLIDTOSTRIMPL(KB_T);
	CONTROLIDTOSTRIMPL(KB_U);
	CONTROLIDTOSTRIMPL(KB_V);
	CONTROLIDTOSTRIMPL(KB_W);
	CONTROLIDTOSTRIMPL(KB_X);
	CONTROLIDTOSTRIMPL(KB_Y);
	CONTROLIDTOSTRIMPL(KB_Z);
	CONTROLIDTOSTRIMPL(KB_LSuper);
	CONTROLIDTOSTRIMPL(KB_RSuper);
	CONTROLIDTOSTRIMPL(KB_Numpad_0);
	CONTROLIDTOSTRIMPL(KB_Numpad_1);
	CONTROLIDTOSTRIMPL(KB_Numpad_2);
	CONTROLIDTOSTRIMPL(KB_Numpad_3);
	CONTROLIDTOSTRIMPL(KB_Numpad_4);
	CONTROLIDTOSTRIMPL(KB_Numpad_5);
	CONTROLIDTOSTRIMPL(KB_Numpad_6);
	CONTROLIDTOSTRIMPL(KB_Numpad_7);
	CONTROLIDTOSTRIMPL(KB_Numpad_8);
	CONTROLIDTOSTRIMPL(KB_Numpad_9);
	CONTROLIDTOSTRIMPL(KB_Numpad_Multiply);
	CONTROLIDTOSTRIMPL(KB_Numpad_Add);
	CONTROLIDTOSTRIMPL(KB_Numpad_Enter);
	CONTROLIDTOSTRIMPL(KB_Numpad_Subtract);
	CONTROLIDTOSTRIMPL(KB_Numpad_Decimal);
	CONTROLIDTOSTRIMPL(KB_Numpad_Divide);
	CONTROLIDTOSTRIMPL(KB_F1);
	CONTROLIDTOSTRIMPL(KB_F2);
	CONTROLIDTOSTRIMPL(KB_F3);
	CONTROLIDTOSTRIMPL(KB_F4);
	CONTROLIDTOSTRIMPL(KB_F5);
	CONTROLIDTOSTRIMPL(KB_F6);
	CONTROLIDTOSTRIMPL(KB_F7);
	CONTROLIDTOSTRIMPL(KB_F8);
	CONTROLIDTOSTRIMPL(KB_F9);
	CONTROLIDTOSTRIMPL(KB_F10);
	CONTROLIDTOSTRIMPL(KB_F11);
	CONTROLIDTOSTRIMPL(KB_F12);
	CONTROLIDTOSTRIMPL(KB_NumLock);
	CONTROLIDTOSTRIMPL(KB_ScrollLock);
	CONTROLIDTOSTRIMPL(KB_LShift);
	CONTROLIDTOSTRIMPL(KB_RShift);
	CONTROLIDTOSTRIMPL(KB_LCtrl);
	CONTROLIDTOSTRIMPL(KB_RCtrl);
	CONTROLIDTOSTRIMPL(KB_LAlt);
	CONTROLIDTOSTRIMPL(KB_RAlt);
	CONTROLIDTOSTRIMPL(Mouse_Button_1);
	CONTROLIDTOSTRIMPL(Mouse_Button_2);
	CONTROLIDTOSTRIMPL(Mouse_Button_3);
	CONTROLIDTOSTRIMPL(Mouse_Button_4);
	CONTROLIDTOSTRIMPL(Mouse_Button_5);
	CONTROLIDTOSTRIMPL(Mouse_Delta_X);
	CONTROLIDTOSTRIMPL(Mouse_Delta_Y);
	CONTROLIDTOSTRIMPL(Mouse_Wheel_Y);
	CONTROLIDTOSTRIMPL(Mouse_Wheel_X);
	CONTROLIDTOSTRIMPL(KB_Semicolon);
	CONTROLIDTOSTRIMPL(KB_Plus);
	CONTROLIDTOSTRIMPL(KB_Comma);
	CONTROLIDTOSTRIMPL(KB_Minus);
	CONTROLIDTOSTRIMPL(KB_Period);
	CONTROLIDTOSTRIMPL(KB_FSlash);
	CONTROLIDTOSTRIMPL(KB_Tilde);
	CONTROLIDTOSTRIMPL(KB_LBracket);
	CONTROLIDTOSTRIMPL(KB_BSlash);
	CONTROLIDTOSTRIMPL(KB_RBracket);
	CONTROLIDTOSTRIMPL(KB_Quote);

	#undef CONTROLIDTOSTRIMPL

	return "";
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


void Input::LoadContextFile(std::string path, std::string fallbackPath)
{
	File::INIReader ini(path, fallbackPath, { "Input." });

	for (HashName contextName : ini.Sections())
	{
		// Flush the previously loaded context, if it was loaded before
		if (contextRegistry.count(contextName))
		{
			contextRegistry[contextName].actions.clear();
			contextRegistry[contextName].axes.clear();
		}

		std::vector<std::map<HashName, inivalue_t>> mappings;
		bool hasActionMappings = false;
		bool hasAxisMappings = false;

		mappings = ini.GetMultiDictionary(contextName, HN("ActionMapping"), 
			{
				{HN("name"), INIDataType::String},
				{HN("control"), INIDataType::String},
				{HN("dir"), INIDataType::String}
			});
		if (!mappings.empty())
		{
			for (std::map<HashName, inivalue_t>& map : mappings)
			{
				if (map.count(HN("name")) &&
					map.count(HN("control")) &&
					map.count(HN("dir")))
				{
					std::pair<bool, ControlID> control_converted = StringToControlID(map[HN("control")]);
					std::pair<bool, bool> dir_converted = StringToDirBool(map[HN("dir")]);
					float threshold_converted;
					if (map.count(HN("threshold"))) // threshold is optional
					{
						threshold_converted = map[HN("threshold")];
					}
					else
					{
						threshold_converted = 0.5f;
					}

					if (control_converted.first &&
						dir_converted.first)
					{
						contextRegistry[contextName].actions.emplace_back
						(
							ActionMapping
							{
								map[HN("name")].Hash(),
								control_converted.second,
								threshold_converted,
								dir_converted.second
							}
						);

						hasActionMappings = true;
					}
					else
					{
						EP_WARN("Input System: An ActionMapping failed to load due to a subvalue conversion error.  "
								"The mapping was not loaded.\nFile: {}\nContext: {}", path, HN_ToStr(contextName));
						continue;
					}
				}
				else
				{
					EP_WARN("Input System: An ActionMapping failed to load due to a missing subkey.  "
							"The mapping was not loaded.\nFile: {}\nContext: {}", path, HN_ToStr(contextName));
					continue;
				}
			}
		}

		mappings = ini.GetMultiDictionary(contextName, HN("AxisMapping"), 
			{
				{ HN("name"), INIDataType::String },
				{ HN("control"), INIDataType::String },
				{ HN("scale"), INIDataType::Float}
			});
		if (!mappings.empty())
		{
			for (std::map<HashName, inivalue_t>& map : mappings)
			{
				if (map.count(HN("name")) &&
					map.count(HN("control")) &&
					map.count(HN("scale")))
				{
					std::pair<bool, ControlID> control_converted = StringToControlID(map[HN("control")]);
					float scale_converted = map[HN("scale")];

					if (control_converted.first)
					{
						contextRegistry[contextName].axes.emplace_back
						(
							AxisMapping
							{
								map[HN("name")].Hash(),
								control_converted.second,
								scale_converted
							}
						);

						hasAxisMappings = true;
					}
					else
					{
						EP_WARN("Input System: An AxisMapping failed to load due to a subvalue conversion error.  "
								"The mapping was not loaded.\nFile: {}\nContext: {}", path, HN_ToStr(contextName));
						continue;
					}
				}
				else
				{
					EP_WARN("Input System: An AxisMapping failed to load due to a missing subkey.  "
							"The mapping was not loaded.\nFile: {}\nContext: {}", path, HN_ToStr(contextName));
					continue;
				}
			}
		}

		if (!hasActionMappings && !hasAxisMappings)
		{
			EP_WARN("Input System: Input context \"{}\" contains no valid Action or Axis mappings."
					"  File: {}", HN_ToStr(contextName), path);
		}
	}
}


Input::ContextHandle Input::BindContext(HashName contextName,
										StreamID stream,
										int blockingLevel,
										unsigned int layer)
{
	if (contextRegistry.count(contextName) == 0)
		EP_ERROR("Input::BindContext(): \"{}\" has not been loaded with Input::LoadContextFile()!", 
			HN_ToStr(contextName));

	contextStack[layer].push_front(contextRegistry[contextName]);
	contextStack[layer].front().blockingLevel = blockingLevel;
	contextStack[layer].front().stream = stream;
	layerOfContext[&contextStack[layer].front()] = layer;

	for (const auto& axis : contextRegistry[contextName].axes)
	{
		axisValues[0][&contextStack[layer].front()][axis.name];
		axisValues[1][&contextStack[layer].front()][axis.name];
	}
	for (const auto& action : contextRegistry[contextName].actions)
	{
		actionCallbacks[&contextStack[layer].front()][action.name];
	}

	return ContextHandle(&contextStack[layer].front());
}

void Input::PopContext(ContextHandle context)
{
	EP_ASSERTF(layerOfContext.count(context) > 0,
			   "Input::PopContext(): 'context' is not currently bound.");
	int layer = layerOfContext[context];
	layerOfContext.erase(context);

	auto prev = contextStack[layer].before_begin();
	for (auto it = contextStack[layer].begin();
		 it != contextStack[layer].end();
		 ++it)
	{
		if (&(*it) == context)
		{
			axisValues[0].erase(&(*it));
			axisValues[1].erase(&(*it));
			actionCallbacks.erase(&(*it));
			contextStack[layer].erase_after(prev);
			break;
		}
		prev++;
		it = prev;
	}
}

void Input::BindAction(ContextHandle context, HashName actionName, ActionCallbackPtr callback)
{
	if (actionCallbacks.count(context) == 0)
		EP_WARN("Input::BindAction(): Context contains no input actions!");
	if (actionCallbacks[context].count(actionName) == 0)
		EP_WARN("Input::BindAction(): Context does not contain action \"{}\"!", HN_ToStr(actionName));

	actionCallbacks[context][actionName].emplace_back(std::pair(callback, StateManager::activeState));
}

float Input::GetAxis(ContextHandle context, HashName axisName)
{
	EP_ASSERTF_SLOW(axisValues[0].count(context) > 0,
		"Input::GetAxis(): context contains no input axes");
	EP_ASSERTF_SLOW(axisValues[0][context].count(axisName) > 0,
		"Input::GetAxis(): context does not contain this axis");

	return axisValues[Time::inFixedTimestep()][context][axisName];
}


void Input::CheckForControllerWake()
{
	if (!kbmBuffer.isAssigned)
	{
		// Check for key/button presses (not releases).
		if (kbmBuffer.keys[currentBuffer][0] > kbmBuffer.keys[!currentBuffer][0] ||
			kbmBuffer.keys[currentBuffer][1] > kbmBuffer.keys[!currentBuffer][1])
		{
			Events::Dispatch(HN("ControllerWake"), ControllerID(1));
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
		if (gpIt->assignedStream == 0)
		{
			// Check for button presses
			if (gpIt->buttons[currentBuffer] > gpIt->buttons[!currentBuffer])
			{
				Events::Dispatch(HN("ControllerWake"), ControllerID(gpIt - gpBuffer.begin() + 2));
			}
			// Check for axes changes
			else
			{
				for (int i = 0; 
					 i < (int(ControlID::_EndOfGPAxes) - int(ControlID::_EndOfGPButtons) - 1); 
					 i++)
				{
					if (gpIt->axes[currentBuffer][i] != gpIt->axes[!currentBuffer][i])
					{
						Events::Dispatch(HN("ControllerWake"), ControllerID(gpIt - gpBuffer.begin() + 2));
						break;
					}
				}
			}
		}
	}
}


void Input::ProcessContext(Input::Context& context)
{
	EP_ASSERT_SLOW(context.stream != 0);
	EP_ASSERT_SLOW(context.stream < StreamBindings.size());

	if (!isStreamBlocked[context.stream])
	{
		isStreamBlocked[context.stream] = (context.blockingLevel == 2);

		if (StreamBindings[context.stream] == 0)
		{
			// No ControllerID is bound to the context's stream.
			return;
		}
		else if (StreamBindings[context.stream] == ControllerID(1)) // Keyboard & mouse
		{
			for (const auto& axis : context.axes)
			{
				if (axis.control > ControlID::_EndOfGPAxes)
				{
					if (axis.control < ControlID::_EndOfKBMouseButtons)
					{
						// This is a kb/mouse binding

						int keyIndex = int(axis.control) - int(ControlID::_EndOfGPAxes) - 1;

						// Is key bit in high-order word?
						if (keyIndex < 64)
						{
							// Check if key is blocked
							if (!(kbmBuffer.keys_blockstatus[0] & BIT(keyIndex)))
							{
								// Map key to axis
								if (kbmBuffer.keys[currentBuffer][0] & BIT(keyIndex))
								{
									axisValues[0][&context][axis.name] += axis.scale;
									axisValues[1][&context][axis.name] += axis.scale;
								}

								// Set key block bit if appropriate
								if (context.blockingLevel > 0)
									kbmBuffer.keys_blockstatus[0] |= BIT(keyIndex);
							}
						}
						else
						{
							// Check if key is blocked
							if (!(kbmBuffer.keys_blockstatus[1] & BIT(keyIndex % 64)))
							{
								// Map key to axis
								if (kbmBuffer.keys[currentBuffer][1] & BIT(keyIndex % 64))
								{
									axisValues[0][&context][axis.name] += axis.scale;
									axisValues[1][&context][axis.name] += axis.scale;
								}

								// Set key block bit if appropriate
								if (context.blockingLevel > 0)
									kbmBuffer.keys_blockstatus[1] |= BIT(keyIndex % 64);
							}
						}
					}
					else
					{
						// This is a mouse axis

						int axisIndex = int(axis.control) - int(ControlID::_EndOfKBMouseButtons) - 1;
						if (!kbmBuffer.axes_blockstatus[axisIndex])
						{
							// Not blocked.
							axisValues[0][&context][axis.name] += kbmBuffer.axes[currentBuffer][axisIndex] / Time::ActualRealDelta() * axis.scale;
							axisValues[1][&context][axis.name] += mouseAxisAccumulator[axisIndex] / mouseDeltaAccumulator * axis.scale;

							kbmBuffer.axes_blockstatus[axisIndex] |= (context.blockingLevel > 0);
						}
					}
				}
			}

			std::map<HashName, bool> isActionTriggered;

			for (const auto& action : context.actions)
			{
				if (action.control > ControlID::_EndOfGPAxes)
				{
					int kbm_control_index = (int)action.control - (int)ControlID::_EndOfGPAxes - 1;

					if ((kbm_control_index) < 64 &&
						!(kbmBuffer.keys_blockstatus[0] & BIT(kbm_control_index)))
					{
						// Unblocked button/key in low-order word.
						uint64_t control_flag = BIT(kbm_control_index);

						isActionTriggered[action.name] |=
							((kbmBuffer.keys[currentBuffer][0] & control_flag) &&
								!(kbmBuffer.keys[!currentBuffer][0] & control_flag) &&
								action.isDownAction) ||
							(!(kbmBuffer.keys[currentBuffer][0] & control_flag) &&
								(kbmBuffer.keys[!currentBuffer][0] & control_flag) &&
								!action.isDownAction);

						if (context.blockingLevel > 0)
							kbmBuffer.keys_blockstatus[0] |= control_flag;
					}
					else if (action.control < ControlID::_EndOfKBMouseButtons &&
							 !(kbmBuffer.keys_blockstatus[1] & BIT(kbm_control_index - 64)))
					{
						// Unblocked button in high-order word.
						uint64_t control_flag = BIT(kbm_control_index - 64);

						isActionTriggered[action.name] |=
							((kbmBuffer.keys[currentBuffer][1] & control_flag) &&
								!(kbmBuffer.keys[!currentBuffer][1] & control_flag) &&
								action.isDownAction) ||
							(!(kbmBuffer.keys[currentBuffer][1] & control_flag) &&
								(kbmBuffer.keys[!currentBuffer][1] & control_flag) &&
								!action.isDownAction);

						if (context.blockingLevel > 0)
							kbmBuffer.keys_blockstatus[1] |= control_flag;
					}
					else if (!kbmBuffer.axes_blockstatus[int(action.control) - int(ControlID::_EndOfKBMouseButtons) - 1])
					{
						// Unblocked axis.
						int control = int(action.control) - int(ControlID::_EndOfKBMouseButtons) - 1;

						isActionTriggered[action.name] |=
							((kbmBuffer.axes[currentBuffer][control] > action.threshold) &&
								(kbmBuffer.axes[!currentBuffer][control] < action.threshold) &&
								action.isDownAction && action.threshold > 0) ||
							((kbmBuffer.axes[currentBuffer][control] < action.threshold) &&
								(kbmBuffer.axes[!currentBuffer][control] > action.threshold) &&
								action.isDownAction && action.threshold < 0) ||
							((kbmBuffer.axes[currentBuffer][control] < action.threshold) &&
								(kbmBuffer.axes[!currentBuffer][control] > action.threshold) &&
								!action.isDownAction && action.threshold > 0) ||
							((kbmBuffer.axes[currentBuffer][control] > action.threshold) &&
								(kbmBuffer.axes[!currentBuffer][control] < action.threshold) &&
								!action.isDownAction && action.threshold < 0);

						kbmBuffer.axes_blockstatus[control] |= (context.blockingLevel > 0);
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

			for (const auto& [actionName, triggered] : isActionTriggered)
			{
				if (triggered)
				{
					for (const auto& callback : actionCallbacks[&context][actionName])
					{
						StateManager::activeState = callback.second;
						callback.first();
						StateManager::activeState = nullptr;
					}
				}
			}
		}
		else // Gamepad mappings
		{
			ControllerID gamepad = StreamBindings[context.stream] - 2;

			for (const auto& axis : context.axes)
			{
				if (axis.control < ControlID::_EndOfGPAxes)
				{
					if (axis.control < ControlID::_EndOfGPButtons &&
						!(gpBuffer[gamepad].buttons_blockstatus & BIT(int(axis.control))))
					{
						// Unblocked button.
						uint16_t control_flag = BIT(uint16_t(axis.control));

						axisValues[0][&context][axis.name] +=
							(gpBuffer[gamepad].buttons[currentBuffer] & control_flag ? 1.0f : 0.0f) * axis.scale;
						axisValues[1][&context][axis.name] +=
							(gpBuffer[gamepad].buttons[currentBuffer] & control_flag ? 1.0f : 0.0f) * axis.scale;

						if (context.blockingLevel > 0)
							gpBuffer[gamepad].buttons_blockstatus |= control_flag;
					}
					else if (!gpBuffer[gamepad]
							 .axes_blockstatus[int(axis.control) - int(ControlID::_EndOfGPButtons) - 1])
					{
						// Unblocked axis.
						int control = int(axis.control) - int(ControlID::_EndOfGPButtons) - 1;

						axisValues[0][&context][axis.name] += gpBuffer[gamepad].axes[currentBuffer][control] * axis.scale;
						axisValues[1][&context][axis.name] += gpBuffer[gamepad].axes[currentBuffer][control] * axis.scale;
						gpBuffer[gamepad].axes_blockstatus[control] |= (context.blockingLevel > 0);
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

			std::map<HashName, bool> isActionTriggered;

			for (const auto& action : context.actions)
			{
				if (action.control < ControlID::_EndOfGPAxes)
				{
					if (action.control < ControlID::_EndOfGPButtons &&
						!(gpBuffer[gamepad].buttons_blockstatus & BIT(int(action.control))))
					{
						// Unblocked button.
						uint16_t control_flag = BIT(uint16_t(action.control));

						isActionTriggered[action.name] |=
							((gpBuffer[gamepad].buttons[currentBuffer] & control_flag) &&
							 !(gpBuffer[gamepad].buttons[!currentBuffer] & control_flag) &&
							 action.isDownAction) ||
							(!(gpBuffer[gamepad].buttons[currentBuffer] & control_flag) &&
							 (gpBuffer[gamepad].buttons[!currentBuffer] & control_flag) &&
							 !action.isDownAction);

						if (context.blockingLevel > 0)
							gpBuffer[gamepad].buttons_blockstatus |= control_flag;
					}
					else if (!gpBuffer[gamepad]
							 .axes_blockstatus[int(action.control) - int(ControlID::_EndOfGPButtons) - 1])
					{
						// Unblocked axis.
						int control = int(action.control) - int(ControlID::_EndOfGPButtons) - 1;

						isActionTriggered[action.name] |=
							((gpBuffer[gamepad].axes[currentBuffer][control] > action.threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] < action.threshold) &&
							 action.isDownAction && action.threshold > 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] < action.threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] > action.threshold) &&
							 action.isDownAction && action.threshold < 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] < action.threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] > action.threshold) &&
							 !action.isDownAction && action.threshold > 0) ||
							((gpBuffer[gamepad].axes[currentBuffer][control] > action.threshold) &&
							 (gpBuffer[gamepad].axes[!currentBuffer][control] < action.threshold) &&
							 !action.isDownAction && action.threshold < 0);

						gpBuffer[gamepad].axes_blockstatus[control] |= (context.blockingLevel > 0);
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

			for (const auto& [actionName, triggered] : isActionTriggered)
			{
				if (triggered)
				{
					for (const auto& callback : actionCallbacks[&context][actionName])
					{
						StateManager::activeState = callback.second;
						callback.first();
						StateManager::activeState = nullptr;
					}
				}
			}
		}
	}
}


void Input::Init()
{
	PlatformInit();

	// Clear event-based input buffers if the window loses focus.
	Events::Subscribe(
		HN("WindowLostFocus"),
		[](Events::Event& e)
		{
			memset(kbmBuffer.keys[currentBuffer], 0, sizeof(kbmBuffer.keys[currentBuffer]));
			memset(kbmBuffer.axes[currentBuffer], 0, sizeof(kbmBuffer.axes[currentBuffer]));
			return false;
		});

	// Catch mouse position events for polling
	Events::Subscribe(
		HN("MousePosition"),
		[](Events::Event& e)
		{
			auto position = Events::Unpack<glm::vec2>(e);
			cursorPos = position;
			return true;
		});

	// Bind the keyboard and mouse to StreamID 1 by default
	BindController(ControllerID(1), StreamID(1));
}


void Input::Update()
{
	// Clear raw input block flags
	std::fill(isStreamBlocked.begin(), isStreamBlocked.end(), false);
	kbmBuffer.keys_blockstatus[0] = 0;
	kbmBuffer.keys_blockstatus[1] = 0;
	memset(kbmBuffer.axes_blockstatus, 0, sizeof(kbmBuffer.axes_blockstatus));
	for (auto& gamepadBufferIt : gpBuffer)
	{
		gamepadBufferIt.buttons_blockstatus = 0;
		memset(gamepadBufferIt.axes_blockstatus, false, sizeof(gamepadBufferIt.axes_blockstatus));
	}

	// Accumulate raw mouse deltas for use in FixedUpdate() axes
	mouseAxisAccumulator[0] += kbmBuffer.axes[currentBuffer][0];
	mouseAxisAccumulator[1] += kbmBuffer.axes[currentBuffer][1];
	mouseAxisAccumulator[2] += kbmBuffer.axes[currentBuffer][2];
	mouseAxisAccumulator[3] += kbmBuffer.axes[currentBuffer][3];
	mouseDeltaAccumulator += Time::ActualRealDelta();

	// Reset active context axes
	for (auto& [context, axes] : axisValues[0])
	{
		for (auto& [axis, value] : axes)
		{
			value = 0.0f;
		}
	}
	for (auto& [context, axes] : axisValues[1])
	{
		for (auto& [axis, value] : axes)
		{
			value = 0.0f;
		}
	}

	// Process input
	GetRawInput();
	CheckForControllerWake();
	for (int i = 0; i < MaxInputContextLayers; i++)
	{
		auto prev = contextStack[i].before_begin();
		for (auto it = contextStack[i].begin();
			 it != contextStack[i].end();
			 ++it)
		{
			ProcessContext(*it);
			prev++;
			it = prev;
			if (it == contextStack[i].end()) break;
		}
	}

	if (Time::isFixedUpdatePending())
	{
		// Clear raw mouse delta accumulators, as the results will be used this FixedUpdate()
		memset(mouseAxisAccumulator, 0, sizeof(mouseAxisAccumulator));
		mouseDeltaAccumulator = 0.0f;
	}

	// Flip buffers for the next frame
	currentBuffer = !currentBuffer;
	memcpy(kbmBuffer.keys[currentBuffer], kbmBuffer.keys[!currentBuffer], sizeof(kbmBuffer.keys[currentBuffer]));
	memset(kbmBuffer.axes[currentBuffer], 0, sizeof(kbmBuffer.axes[currentBuffer]));
}
