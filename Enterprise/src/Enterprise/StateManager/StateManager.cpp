#include "EP_PCH.h"
#include "StateManager.h"
#include "../Application/Application.h"

using Enterprise::StateManager;

static std::deque<StateManager::State*> stateStack;
StateManager::State* StateManager::activeState = nullptr;

static std::vector<bool> stateBlocksFixed;
static std::vector<bool> stateBlocksUpdates;
static std::vector<bool> stateBlocksDraws;
static size_t highestFixedBlockingState = 0;
static size_t highestUpdateBlockingState = 0;
static size_t highestDrawBlockingState = 0;

void StateManager::PushState(State* newState, bool blockLowerFixed, bool blockLowerUpdates, bool blockLowerDraws)
{
	stateStack.push_back(newState);
	stateBlocksFixed.push_back(blockLowerFixed);
	stateBlocksUpdates.push_back(blockLowerUpdates);
	stateBlocksDraws.push_back(blockLowerDraws);

	if (blockLowerFixed) highestFixedBlockingState = stateStack.size() - 1;
	if (blockLowerUpdates) highestUpdateBlockingState = stateStack.size() - 1;
	if (blockLowerDraws) highestDrawBlockingState = stateStack.size() - 1;

	activeState = newState;
	newState->Init();
	activeState = nullptr;
}
void StateManager::PopState()
{
	EP_ASSERT(stateStack.size() > 0);

	activeState = stateStack.back();
	activeState->Cleanup();
	activeState = nullptr;

	stateStack.pop_back();
	stateBlocksFixed.pop_back();
	stateBlocksUpdates.pop_back();
	stateBlocksDraws.pop_back();

	if (stateStack.size() == 0)
		Enterprise::Application::Quit();

	int i;
	for (i = (int)stateBlocksFixed.size() - 1; i >= 0; i--)
	{
		if (stateBlocksFixed.at(i))
			break;
	}
	highestFixedBlockingState = i;
	for (i = (int)stateBlocksUpdates.size() - 1; i >= 0; i--)
	{
		if (stateBlocksUpdates.at(i))
			break;
	}
	highestUpdateBlockingState = i;
	for (i = (int)stateBlocksDraws.size() - 1; i >= 0; i--)
	{
		if (stateBlocksDraws.at(i))
			break;
	}
	highestDrawBlockingState = i;
}

void StateManager::SwapState(State* newState)
{
	EP_ASSERT(newState);

	for (State*& s : stateStack)
	{
		if (s == activeState)
		{
			activeState->Cleanup();

			s = newState;
			activeState = newState;
			newState->Init();
			activeState = nullptr;
			return;
		}
	}

	EP_ASSERT_NOENTRY();
}

bool StateManager::IsStateOnTop(State* s)
{
	State* s_actual = s ? s : activeState;
	EP_ASSERT(s_actual != nullptr);
	return stateStack.back() == s_actual;
}


void StateManager::FixedUpdate()
{
	int i = 0;
	for (State* s : stateStack)
	{
		activeState = s;
		if (i >= highestFixedBlockingState)
			s->FixedUpdate();
		i++;
	}
	activeState = nullptr;
}
void StateManager::Update()
{
	int i = 0;
	for (State* s : stateStack)
	{
		activeState = s;
		if (i >= highestUpdateBlockingState)
			s->Update();
		i++;
	}
	activeState = nullptr;
}
void StateManager::Draw()
{
	int i = 0;
	for (State* s : stateStack)
	{
		activeState = s;
		if (i >= highestDrawBlockingState)
			s->Draw();
		i++;
	}
	activeState = nullptr;
}


void StateManager::Cleanup()
{
	for (State* s : stateStack)
	{
		s->Cleanup();
	}
}
