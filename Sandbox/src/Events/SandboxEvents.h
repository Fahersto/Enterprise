#pragma once
#include "Enterprise/Event/Event.h"

enum class EventType
{
	// Used by Dispatcher to determine when dealing with client event types
	none = (int)Enterprise::EventType::NumOfTypes,
	
	// Game management
	Score,
	PauseGame,
	GameModeChange,

	// Health
	AOE,
	Damage,

	// AI
	AI_Distraction,
	Inquiring,
	Pursuit,
	Bunkered,

	// Player info
	PlayerPosition,
	PlayerOOA,
	PlayerInjured,

	// Used by Dispatcher to allocate resources
	NumOfTypes
};

enum class EventCategory
{
	// Used by Dispatcher to determine when dealing with client event types
	None = (int)Enterprise::EventCategory::NumOfTypes,

	// Game management
	Score,
	PauseGame,
	GameModeChange,

	// Health
	AOE,
	Damage,

	// AI
	AI_Distraction,
	Inquiring,
	Pursuit,
	Bunkered,

	// Player info
	PlayerPosition,
	PlayerOOA,
	PlayerInjured,

	// Used by Dispatcher to allocate resources
	NumOfTypes
};

std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>> eventCallbackList[(int)EventType::NumOfTypes];

std::vector<std::function<bool(std::shared_ptr<Enterprise::Event>)>>* getClientCallbackLists()
{

	return nullptr;
}