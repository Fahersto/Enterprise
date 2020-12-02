#pragma once
#include "EP_PCH.h"
#include "Core.h"

#include "Enterprise/Events/Events.h"
#include "ControlIDs.h"

#define EP_PLAYERID_NULL Enterprise::Input::PlayerID(-2)
#define EP_PLAYERID_ALL Enterprise::Input::PlayerID(-1)
#define EP_CONTROLLERID_NULL Enterprise::Input::ControllerID(-1)
#define EP_CONTROLLERID_KBMOUSE Enterprise::Input::ControllerID(0)

namespace Enterprise
{

namespace Constants
{
// The maximum number of Axes a single callback can map.
constexpr size_t MaxAxesPerBinding = 10;
}

/// The Enterprise input system.
class Input
{
public:
	/// Identifies a player in a multiplayer game.
	typedef size_t PlayerID;
	/// Identifies a form of input (nullable, includes keyboard/mouse).
	typedef size_t ControllerID;
	/// Ideentifies a specific gamepad (non-nullable).
	//typedef size_t GamepadID;

	/// Get the next available Player ID.
	/// @remarks Specifically, this function will return the smallest number that currently
	/// identifies no player.  If a controller gets disconnected, the PlayerID it was assigned
	/// to may be returned by this function.
	static PlayerID GetNextPlayerID();

	/// Assign a controller to a specific player.
	/// @param playerID The player to associate the controller with.
	/// @param controllerID The ID for the controller to assign to the player.
	static void AssignControllerToPlayer(PlayerID player, ControllerID controller);

	/// Load all input contexts in an INI file into the Input system.
	/// @param filename Name of the INI file containing the contexts.
	/// @note In order for a context to be loaded, it must be part of the section group
	/// "Input/Contexts/".
	static void LoadContextsFromFile(std::string filename);

	/// Bind a callback function to an input Action.
	/// @param callbackPtr callbackPtr Pointer to the callback function.
	/// @param player The ID of the player whose Action will trigger this binding.
	/// @note @c player can be set to @c EP_PLAYERID_ALL to bind all PlayerIDs at once.
	/// @param isBlocking Passing "true" will block this binding's ControlIDs from being used in lower bindings.
	/// @param contextName The hashed name of the context this Action is a part of.
	/// @param actionName actionName The hashed name of the Action to bind.
	static void BindAction(void(*callbackPtr)(PlayerID player),
									PlayerID player,
									bool isBlocking,
									HashName contextName,
									HashName actionName);

	/// Bind a callback function to one or more input Axes.
	/// @tparam ...FloatPack A variable number of floats.
	/// @tparam ...HashPack A variable number of HashNames.
	/// @param callbackPtr callbackPtr Pointer to the callback function.
	/// @param player The ID of the player whose Axes will be reported.
	/// @note @c player can be set to @c EP_PLAYERID_ALL to bind all PlayerIDs at once.
	/// @param isBlocking Passing "true" will block this binding's ControlIDs from being used in lower bindings.
	/// @param contextName The hashed name of the context these Axes are a part of.
	/// @param ...axisName The hashed name of each Axis to bind.
	template <typename ... FloatPack, typename ... HashPack>
	static std::enable_if_t<is_all_same<float, FloatPack...>::value&&
		is_all_same<HashName, HashPack...>::value &&
		sizeof...(FloatPack) == sizeof...(HashPack),
		void>
		BindAxes(void(*callbackPtr)(PlayerID, FloatPack...),
				 PlayerID player,
				 bool isBlocking,
				 HashName contextName,
				 HashPack...axisName)
	{
		EP_ASSERTF(player != EP_PLAYERID_NULL, "Input System: Attempted to bind Axis to EP_PLAYERID_NULL.");

		BindingStack.emplace_back(
			Binding{ (void*)callbackPtr, contextName,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			player, sizeof...(HashPack), isBlocking });

		std::array<HashName, sizeof...(HashPack)> axesNames{ axisName... };
		for (int i = 0; i < sizeof...(HashPack); i++)
		{
			if (AxisMap[contextName][axesNames[i]].size() == 0)
			{
				EP_ERROR("Input System: Bound Axis \"{}\" has no loaded AxisMappings "
						 "and will not reflect player input."
						 "  Context Name: {}", HN_ToStr(axesNames[i]), HN_ToStr(contextName));
			}

			BindingStack.back().ActionOrAxesHN[i] = axesNames[i];
		}
	}

	/// Add a blocker to the input binding stack.  Bindings lower in the stack will not receive input.
	/// @param player The PlayerID to block.  This can be EP_PLAYERID_ALL.
	static void BlockLowerBindings(PlayerID player);
	
	/// Pop one or more bindings from the binding stack.
	/// @param count Number of bindings to pop.
	static void PopBindings(size_t count);

private:
	friend class Application;

	struct KBMouseBuffer 
	{
		static_assert((size_t(ControlID::_EndOfKBMouseButtons) - size_t(ControlID::_EndOfGPAxes) - 1) <= 128,
					  "There are more KBMouseButtons than will fit in two 64-bit bit fields.");

		/// Bit field representing the up/down state for every key and mouse button.
		uint64_t keys[2][2] = { 0 };

		/// Bit field representing whether a key has been blocked by a prior blocking context.
		uint64_t keys_blockstatus[2] = { 0 };

		/// Current state of all mouse axes.
		float axes[2][size_t(ControlID::_EndOfIDs) - size_t(ControlID::_EndOfKBMouseButtons) - 1] = { 0 };

		/// Whether a given axis has been blocked by a prior blocking context.
		bool axes_blockstatus[size_t(ControlID::_EndOfIDs) - size_t(ControlID::_EndOfKBMouseButtons) - 1] = { 0 };
		
		/// Whether the keyboard is currently assigned to Player 0 (no other player assignment is valid).
		bool isAssigned = false;
	};
	static KBMouseBuffer kbmBuffer;

	struct GamePadBuffer 
	{
		/// The currently assigned PlayerID.  This can be EP_PLAYERID_NULL.
		PlayerID assignedPlayer = EP_PLAYERID_NULL;

		static_assert(size_t(ControlID::_EndOfGPButtons) <= 16,
					  "There are more GPButtons than will fit in a 16-bit bit field.");

		/// Bit field representing the up/down state for every button.
		uint16_t buttons[2] = { 0 };

		/// Bit field representing whether a button has been blocked by a prior blocking context.
		uint16_t buttons_blockstatus = { 0 };

		/// Current state of all gamepad axes.
		float axes[2][size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1] = { 0 };

		/// Whether a given axis has been blocked by a prior blocking context.
		bool axes_blockstatus[size_t(ControlID::_EndOfGPAxes) - size_t(ControlID::_EndOfGPButtons) - 1] = { 0 };
	};
	static std::vector<GamePadBuffer> gpBuffer;

	static bool currentBuffer;

	struct Binding
	{
		void* callbackPtr;
		HashName ContextHN;
		HashName ActionOrAxesHN[Constants::MaxAxesPerBinding];
		PlayerID playerID;
		uint_fast8_t NumOfAxes;
		bool isBlocking;
	};

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
	static std::unordered_map<HashName, std::unordered_map<HashName, std::vector<ActionMapping>>> ActionMap;
	static std::unordered_map<HashName, std::unordered_map<HashName, std::vector<AxisMapping>>> AxisMap;
	static std::vector<Binding> BindingStack;

	static bool HandlePlatformEvents(Events::Event& e);
	static void GetRawInput();
	static void CheckForControllerWake();
	static PlayerID UnassignController(ControllerID controller);

	static void ProcessKeyboardBinding(const std::reverse_iterator<std::vector<Enterprise::Input::Binding>::iterator>& bindingIt);
	static void ProcessGamepadBinding(const std::reverse_iterator<std::vector<Enterprise::Input::Binding>::iterator>& bindingIt,
									  PlayerID player);
	static void ProcessBinding(const std::vector<Binding>::reverse_iterator& it);

	static void Init();
	static void PlatformInit();
	static void Update();
};

}
