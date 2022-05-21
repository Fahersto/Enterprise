#pragma once
#include "Enterprise/Core.h"
#include <list>
#include <memory>
#include <type_traits>

namespace Enterprise
{

/// Enterprise's global state machine.
class StateManager
{
public:

	/// An applet which can slot into Enterprise's global state machine.
	/// @remarks Extend this class to create game rules, UIs, and more.
	class EP_API State
	{
	public:
		/// Invoked when the state is made active.
		virtual void Init() {};
		/// Invoked once every fixed timestep.
		virtual void FixedUpdate() {};
		/// Invoked once every frame.
		virtual void Update() {};
		/// Invoked during each frame's draw stage.
		virtual void Draw() {};
		/// Invoked when the state is made inactive.
		virtual void Cleanup() {};

		virtual ~State() = default;
	};

	/// Create a new state and add it to the end of the active state list.
	/// @tparam S The new state type.  This type must be a class derived from StateManager::State.
	/// @param args Arguments to forward to the state class's constructor.
	/// @return An std::weak_ptr to the new state.
	template <class S, typename... Args>
	static std::weak_ptr<State> PushState(Args&&... args)
	{
		static_assert(std::is_base_of<State, S>::value);
		std::shared_ptr<State> newState = std::make_shared<S>(std::forward<Args>(args)...);

		stateList.push_back(newState);

		activeState = newState;
		newState->Init();
		activeState.reset();

		return newState;
	}

	/// Add a preallocated state to the end of the active state list.
	/// @param state An std::shared_ptr to the preallocated state.
	/// @warning If `state` is already in the active state list, it will not be reinserted.
	static EP_API void PushExistingState(std::shared_ptr<State> state);

	/// Create a new state and insert it into the active state list.
	/// @tparam S The new state type.  This type must be a class derived from StateManager::State.
	/// @param insertionPoint An std::weak_ptr to the state to insert after.
	/// @param args Arguments to forward to the state class's constructor.
	/// @return An std::weak_ptr to the newly created state.
	template <class S, typename... Args>
	static std::weak_ptr<State> InsertState(std::weak_ptr<State> insertionPoint, Args&&... args)
	{
		static_assert(std::is_base_of<State, S>::value);
		std::shared_ptr<State> newState = std::make_shared<S>(std::forward<Args>(args)...);
		std::shared_ptr<State> insertionPoint_strong;

		if (insertionPoint.use_count() == 0)
		{
			if (activeState.use_count() != 0)
			{
				insertionPoint_strong = activeState.lock();
			}
			else
			{
				EP_WARN("StateManager::InsertExistingState(): No active state and 'insertionPoint' not specified!");
			}
		}
		else
		{
			insertionPoint_strong = insertionPoint.lock();
		}

		auto it = stateList.begin();
		for (; it != stateList.end(); ++it)
		{
			if (*it == insertionPoint_strong)
			{
				++it;
				if (it == stateList.end())
				{
					stateList.push_back(newState);

					// Break out of for loop without indicating the state wasn't inserted
					it = stateList.begin();
					break;
				}
				else
				{
					stateList.insert(it, insertionPoint_strong);
				}
				break;
			}
		}
		if (it == stateList.end())
		{
			EP_WARN("StateManager::InsertState(): Insertion point not present in state list!");
			stateList.push_back(insertionPoint_strong);
		}

		std::weak_ptr<State> prevActiveState = activeState;

		activeState = newState;
		newState->Init();
		activeState = prevActiveState;

		return newState;
	}

	/// Create a new state and insert it into the active state list after the calling state.
	/// @tparam S The new state type.  This type must be a class derived from StateManager::State.
	/// @param args Arguments to forward to the state class's constructor.
	/// @return An std::weak_ptr to the newly created state.
	template <class S, typename... Args>
	static std::weak_ptr<State> InsertStateAfterCurrent(Args&&... args)
	{
		static_assert(std::is_base_of<State, S>::value);
		return InsertState<S>(std::weak_ptr<State>(), std::forward<Args>(args)...);
	}

	/// Insert a preallocated state into the active state list.
	/// @param state An std::shared_ptr to the preallocated state.
	/// @param insertionPoint An std::weak_ptr to the state to insert after.
	/// @warning If `state` is already in the active state list, it will not be reinserted.
	static EP_API void InsertExistingState(std::shared_ptr<State> state, std::weak_ptr<State> insertionPoint);

	/// Insert a preallocated state into the active state list after the calling state.
	/// @param state An std::shared_ptr to the preallocated state.
	/// @warning If `state` is already in the active state list, it will not be reinserted.
	static EP_API void InsertExistingStateAfterCurrent(std::shared_ptr<State> state);

	/// Create a new state and replace an active state with it.
	/// @tparam S The new state type.  This type must be a class derived from StateManager::State.
	/// @param targetState An std::weak_ptr to the state to replace.
	/// @param args Arguments to forward to the state class's constructor.
	/// @return An std::weak_ptr to the newly created state.
	/// @remarks This method does not overwrite the state pointed to by `targetState`.  If the state was preallocated, it 
	/// can be inserted back into the active state list by future calls to PushExistingState() or InsertExistingState().
	template <class S, typename... Args>
	static std::weak_ptr<State> SwapState(std::weak_ptr<State> targetState, Args&&... args)
	{
		static_assert(std::is_base_of<State, S>::value);
		std::shared_ptr<State> newState = std::make_shared<S>(std::forward<Args>(args)...);
		std::shared_ptr<State> targetState_strong;

		if (targetState.use_count() == 0)
		{
			if (activeState.use_count() != 0)
			{
				targetState_strong = activeState.lock();
			}
			else
			{
				EP_ERROR("StateManager::SwapState(): No target has been specified for swapping!");
				return std::weak_ptr<State>();
			}
		}
		else
		{
			targetState_strong = targetState.lock();
		}

		auto it = stateList.begin();
		for (; it != stateList.end(); ++it)
		{
			if (*it == targetState_strong)
			{
				std::weak_ptr<State> prevActiveState = activeState;
				
				activeState = targetState_strong;
				targetState_strong->Cleanup();
				activeState = newState;
				newState->Init();

				activeState = prevActiveState;

				*it = newState;
				break;
			}
		}
		if (it == stateList.end())
		{
			EP_ERROR("StateManager::SwapState(): Target state not present in state list!  Swap was skipped.");
			return std::weak_ptr<State>();
		}

		return newState;
	}

	/// Create a new state and replace the calling state with it.
	/// @tparam S The new state type.  This type must be a class derived from StateManager::State.
	/// @param args Arguments to forward to the state class's constructor.
	/// @return An std::weak_ptr to the newly created state.
	/// @remarks This method does not overwrite the state pointed to by `targetState`.  If the state was preallocated, it 
	/// can be inserted back into the active state list by future calls to PushExistingState() or InsertExistingState().
	template <class S, typename... Args>
	static std::weak_ptr<State> SwapCurrentState(Args&&... args)
	{
		static_assert(std::is_base_of<State, S>::value);
		return SwapState<S>(std::weak_ptr<State>(), std::forward<Args>(args)...);
	}

	/// Make a state inactive and remove it from the state list.
	/// @param state An std::weak_ptr to the state to end.  If unspecified, the calling state will be terminated.
	static EP_API void EndState(std::weak_ptr<State> state = std::weak_ptr<State>());

private:
	friend class Runtime;
	friend class Events;
	friend class Input;

	static EP_API std::list<std::shared_ptr<State>> stateList;
	static EP_API std::weak_ptr<State> activeState;

	static void FixedUpdate();
	static void Update();
	static void Draw();
	static void Cleanup();
};

}
