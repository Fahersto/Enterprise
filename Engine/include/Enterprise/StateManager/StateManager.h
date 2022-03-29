#pragma once
#include "Enterprise/Core.h"

namespace Enterprise
{

/// Enterprise's global state machine.
class StateManager
{
public:

	/// An applet which can slot into Enterprise's global state machine.
	/// @remarks States are the foundation of every Enterprise game.  Extend this generic class to create game rules,
	/// UIs, and more.
	class EP_API State
	{
	public:
		/// Invoked immediately after the state is made active with StateManager::PushState() or StateManager::SwapState().
		/// @note It is a best practice to set up all State initialization code in Init() instead of the class constructor.
		/// @see [Core Calls](@ref Core_Calls)
		virtual void Init() {};
		/// Invoked once every fixed timestep.
		/// @see [Core Calls](@ref Core_Calls)
		/// @see @ref Time
		virtual void FixedUpdate() {};
		/// Invoked once every frame.
		/// @see [Core Calls](@ref Core_Calls)
		/// @see @ref Time
		virtual void Update() {};
		/// Invoked once every frame, after all Update() calls are complete.
		/// @see [Core Calls](@ref Core_Calls)
		/// @see @ref Time
		virtual void Draw() {};
		/// Invoked when a state is removed via StateManager::PopState() or StateManager::SwapState().
		/// @note It is a best practice to set up all State cleanup code in Cleanup() instead of the class destructor.
		/// @see [Core Calls](@ref Core_Calls)
		virtual void Cleanup() {};

		virtual ~State() = default;
	};

	/// Push a new state on top of the state stack.
	/// @param newState Pointer to the new state.
	/// @param blockLowerFixed Whether lower states should receive @c FixedUpdate() calls while this state is active.
	/// @param blockLowerUpdates Whether lower states should receive @c Update() calls while this state is active.
	/// @param blockLowerDraws Whether lower states should receive @c Draw() calls while this state is active.
	EP_API static void PushState(State* newState, bool blockLowerFixed = false, bool blockLowerUpdates = false, bool blockLowerDraws = false);
	/// Pop the topmost state on the state stack.
	EP_API static void PopState();
	/// Replace the current state with a new one.
	/// @param newState Pointer to the new state.
	/// @note This function can only be invoked from a core call of the state to be replaced.
	/// @note Higher states are unaffected by SwapState() being invoked by the states beneath them.
	/// @note The new state will follow the same core call blocking pattern as the state it is replacing.
	EP_API static void SwapState(State* newState);
	/// Check whether a state is currently on top of the state stack.
	/// @param s Pointer to the state to check.  If @c nullptr, the calling state is checked.
	/// @return @c true if @c s is the highest-ranking state.  Otherwise, @c false.
	EP_API static bool IsStateOnTop(State* s = nullptr);

private:
	friend class Application;
	friend class Events;
	friend class Input;

	static State* activeState;

	static void FixedUpdate();
	static void Update();
	static void Draw();
	static void Cleanup();
};

}
