#pragma once
#include "EP_PCH.h"
#include "Core.h"
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
	static PlayerID NextAvailablePlayerID();

	/// Assign a controller to a specific player.
	/// @param playerID The player to associate the controller with.
	/// @param controllerID The ID for the controller to assign to the player.
	static void AssignControllerToPlayer(PlayerID player, ControllerID controller);

	/// Load an input context from an INI file.
	/// @param filename Name of the INI file containing the context.
	/// @param contextname Name of the context.
	static void LoadContextsFromFile(std::string filename);

	/// Bind a callback function to an Action for all players.
	/// @param contextName The hashed name of the input context to which this Action belongs.
	/// @param actionName actionName The hashed name of the Action to bind.
	/// @param callbackPtr callbackPtr Pointer to the callback function.
	/// @param isBlocking Whether this control should "pass through" to lower bindings.
	static void BindAction(void(*callbackPtr)(PlayerID player),
						   bool isBlocking,
						   HashName contextName,
						   HashName actionName);
	/// Bind a callback function to an Action for a specific player.
	/// @param contextName The hashed name of the input context to which this Action belongs.
	/// @param actionName actionName The hashed name of the Action to bind.
	/// @param player The player whose action can trigger this binding.
	/// @param callbackPtr callbackPtr Pointer to the callback function.
	/// @param isBlocking Whether this control should "pass through" to lower bindings.
	static void BindActionForPlayerID(void(*callbackPtr)(PlayerID player),
									PlayerID player,
									bool isBlocking,
									HashName contextName,
									HashName actionName);

	/// Bind a callback function to one or more Axes for all players.
	/// @tparam ...FloatPack A variable number of floats.
	/// @tparam ...HashPack A variable number of HashNames.
	/// @param callbackPtr Pointer to the callback function.
	/// @param isBlocking Whether these controls should "pass through" to lower bindings.
	/// @param contextName The hashed name of the input context to which these Axes belong.
	/// @param ...axisName The hashed name of each Axis to bind.
	template <typename ... FloatPack, typename ... HashPack>
	static std::enable_if_t<is_all_same<float, FloatPack...>::value&&
		is_all_same<HashName, HashPack...>::value &&
		sizeof...(FloatPack) == sizeof...(HashPack),
		void>
		BindAxes(void(*callbackPtr)(PlayerID, FloatPack...),
				 bool isBlocking,
				 HashName contextName,
				 HashPack...axisName)
	{
		BindingStack.emplace_back(
			Binding{ (void*)callbackPtr, contextName,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			EP_PLAYERID_ALL, sizeof...(HashPack), isBlocking });

		std::array<HashName, sizeof...(HashPack)> axesNames{ axisName... };
		for (int i = 0; i < sizeof...(HashPack); i++)
		{
			BindingStack.back().ActionOrAxes[i] = axesNames[i];
			if (!AxisMap.count(std::pair(contextName, axesNames[i])))
			{
				EP_ERROR("Input System: Bound Axis \"{}\" has no loaded AxisMappings."
						 "  Context Name: {}", SN(axesNames[i]), SN(contextName));
			}
		}
	}
	/// Bind a callback function to one or more Axes for all players.
	/// @tparam ...FloatPack A variable number of floats.
	/// @tparam ...HashPack A variable number of HashNames.
	/// @param callbackPtr Pointer to the callback function
	/// @param player The player for whom this binding will report.
	/// @param isBlocking Whether these controls should "pass through" to lower bindings
	/// @param contextName The hashed name of the input context to which these Axes belong.
	/// @param ...axisName The hashed name of each Axis to bind.
	template <typename ... FloatPack, typename ... HashPack>
	static std::enable_if_t<is_all_same<float, FloatPack...>::value&&
		is_all_same<HashName, HashPack...>::value &&
		sizeof...(FloatPack) == sizeof...(HashPack),
		void>
		BindAxesForPlayerID(void(*callbackPtr)(PlayerID, FloatPack...),
						  PlayerID player,
						  bool isBlocking,
						  HashName contextName,
						  HashPack...axisName)
	{
		BindingStack.emplace_back(
			Binding{ (void*)callbackPtr, contextName,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			player, sizeof...(HashPack), isBlocking });

		std::array<HashName, sizeof...(HashPack)> axesNames{ axisName... };
		for (int i = 0; i < sizeof...(HashPack); i++)
		{
			BindingStack.back().ActionOrAxes[i] = axesNames[i];
			if (!AxisMap.count(std::pair(contextName, axesNames[i])))
			{
				EP_ERROR("Input System: Bound Axis \"{}\" has no loaded AxisMappings."
						 "  Context Name: {}", SN(axesNames[i]), SN(contextName));
			}
		}
	}

	/// Add a blocker to the input binding stack.  Bindings lower in the stack will not receive input.
	static void BlockLowerBindings();
	/// Add a blocker to the input binding stack for a single player.  Bindings lower in the stack will not receive input.
	static void BlockLowerBindingsForPlayer(PlayerID player);
	
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
	static std::vector<bool> isGamepadConnected; // TODO: Get rid of this.

	static bool currentBuffer;

	struct Binding
	{
		void* callback;
		HashName ContextName;
		HashName ActionOrAxes[Constants::MaxAxesPerBinding];
		PlayerID playerID;
		uint_fast8_t NumOfAxes;
		bool bBlocking;
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
	static std::map <std::pair<HashName, HashName>, std::vector<ActionMapping>> ActionMap;
	static std::map <std::pair<HashName, HashName>, std::vector<AxisMapping>> AxisMap;
	static std::vector<Binding> BindingStack;

	static void GetRawInput();
	static void CheckForControllerWake();
	static PlayerID UnassignController(ControllerID controller);

	static void ProcessKeyboardBinding(const std::reverse_iterator<std::vector<Enterprise::Input::Binding>::iterator>& bindingIt);
	static void ProcessGamepadBinding(const std::reverse_iterator<std::vector<Enterprise::Input::Binding>::iterator>& bindingIt,
									  PlayerID player);
	static void ProcessBinding(const std::vector<Binding>::reverse_iterator& it);

	static void Init();
	static void Update();
};

}
