/* SandboxEvents_CategoryList.h
	A list of all client Event types and their member variables.  Add to this list to make new Event types
	available in the client.

	Comments directly above each macro will be visible in the class's IntelliSense tooltip.  You can use
	this to write a description which will be available to anyone trying to use the Event type.

	WARNINGS:
	- Despite your compiler's protests, do NOT define the macros in this file.  They are defined in
	  multiple other contexts.
	- Do not add a header guard.  This file needs to be included in multiple locations.
	- Preprocessor "\" line break rules apply.

	Macros:
	- EVENTTYPE_X(name, [opt]Member1Type, [opt]Member1Name, ... , [opt]Member3Type, [opt]Member3Name):
	Defines an Event type with the given name and members.  Every pair of arguments after the first
	argument is a type and name for a member variable.  Up to three members can be defined, but you
	must call the macro with the correct number (EVENTTYPE_1 ... EVENTTYPE_3).
*/

// Turn on to disable IntelliSense errors while working in this file.
#if 0
#ifdef __INTELLISENSE__
#define EVENTTYPE(eventtype) 
#define EVENTTYPE_1(eventtype, var1type, var1name)
#define EVENTTYPE_2(eventtype, var1type, var1name, var2type, var2name)
#define EVENTTYPE_3(eventtype, var1type, var1name, var2type, var2name, var3type, var3name)
#endif
#endif

// Note: during engine development, I'm filling this file with hypotheticals for testing.

//PLAYER INFORMATION-----------------------------------------------------------------------------

// The 3D coordinates of the player's location.  This is broadcasted every update.
EVENTTYPE_3(PlayerPosition, float, X, float, Y, float, Z)
// An alert that the player is out of ammo.
EVENTTYPE(PlayerOOA)
// An alert that the player took damage last Update().
EVENTTYPE(PlayerHurt)

//GAMEPLAY ----------------------------------------------------------------------------------------

// Broadcasted when the score changes.
EVENTTYPE_1(Score, int, NewScore)
// Broadcasted when the player pauses the game.
EVENTTYPE(PauseGame)
// Broadcasted when changing game modes.
EVENTTYPE_1(NewGameMode, int, ModeId)

//HEALTH ------------------------------------------------------------------------------------------

EVENTTYPE_2(HealthAOE, float, Position, int, HealthDelta)
EVENTTYPE_1(Damage, int, HealthLost)

//AI ----------------------------------------------------------------------------------------------

EVENTTYPE(AI_Distracted)
EVENTTYPE(AI_Curious)
EVENTTYPE(AI_Pursuit)
EVENTTYPE(AI_Bunkering)