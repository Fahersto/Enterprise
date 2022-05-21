#include "Enterprise/StateManager.h"
#include "Enterprise/Runtime.h"

using Enterprise::StateManager;

std::list<std::shared_ptr<StateManager::State>> StateManager::stateList;
std::weak_ptr<StateManager::State> StateManager::activeState;

void StateManager::PushExistingState(std::shared_ptr<StateManager::State> state)
{
	for (const std::shared_ptr<State>& s : stateList)
	{
		if(s != state)
		{
			EP_ERROR("StateManager::PushExistingState(): State is already in active state list!");
			return;
		}
	}

	stateList.push_back(state);

	activeState = state;
	state->Init();
	activeState.reset();
}

void StateManager::InsertExistingState(std::shared_ptr<StateManager::State> state, std::weak_ptr<State> insertionPoint)
{
	for (const std::shared_ptr<State>& s : stateList)
	{
		if(s != state)
		{
			EP_ERROR("StateManager::InsertExistingState(): State is already in active state list!");
			return;
		}
	}

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
				stateList.push_back(state);

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

	activeState = state;
	state->Init();
	activeState = prevActiveState;
}

void StateManager::InsertExistingStateAfterCurrent(std::shared_ptr<StateManager::State> state)
{
	InsertExistingState(state, std::weak_ptr<State>());
}

void StateManager::EndState(std::weak_ptr<State> state)
{
	std::shared_ptr<State> state_strong;
	if (state.use_count() == 0)
	{
		if (!activeState.expired())
		{
			state_strong = activeState.lock();
		}
		else
		{
			EP_ERROR("StateManager::EndState(): Target state is not specified, and there is no active state!");
			return;
		}
	}
	else
	{
		state_strong = state.lock();
	}

	for (auto it = stateList.begin(); it != stateList.end(); ++it)
	{
		if (*it == state_strong)
		{
			std::weak_ptr<State> prevActiveState = activeState;
			activeState = state;
			state_strong->Cleanup();
			activeState = prevActiveState;

			it = stateList.erase(it);
			if (stateList.size() == 0)
				Enterprise::Runtime::Quit();

			return;
		}
	}

	EP_WARN("StateManager::EndState(): State was not found!");
}


void StateManager::FixedUpdate()
{
	for (std::shared_ptr<State> s : stateList)
	{
		activeState = s;
		s->FixedUpdate();
	}
	activeState.reset();
}

void StateManager::Update()
{
	for (std::shared_ptr<State> s : stateList)
	{
		activeState = s;
		s->Update();
	}
	activeState.reset();
}

void StateManager::Draw()
{
	for (std::shared_ptr<State> s : stateList)
	{
		activeState = s;
		s->Draw();
	}
	activeState.reset();
}

void StateManager::Cleanup()
{
	for (std::shared_ptr<State> s : stateList)
	{
		s->Cleanup();
	}
	stateList.clear();
}
