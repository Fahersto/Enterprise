#pragma once
#include "Core.h"

namespace Enterprise
{

/// The entry point for an Enterprise game project.
class Game
{
public:
	/// Initializes the game.
	static void Init();

	/// Cleans up the game at program termination.
	static void Cleanup();
};

}
