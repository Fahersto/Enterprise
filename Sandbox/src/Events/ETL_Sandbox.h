/* ETL_Sandbox.h (Event Type List)
	A list of all Event types in the client, including names and types of their members.

	EDIT THIS ----- The preprocessor uses this list to generate the EventType enum in BaseEvent.h, and the classes themselves
	in Events.h.  To add a new Event type to the core engine, add it to this list.

	WARNINGS:
	- Despite your compiler's warnings, do NOT add a definition for the macros in this file.  This would
	  supplant the definitions in ------ EDIT THIS ------ Events.h and BaseEvent.h.
	- Do NOT add "#pragma once", as this file needs to be included in multiple files.
	- Do not add anything to this file except for C comments and the EVENTTYPE macros.

	Macros:
	  EVENTTYPE(name): Creates an Event named "Event_[name]" with no members.
	  EVENTTYPE_1(name, VarType, VarName):  Creates an Event with one member variable.
	  EVENTTYPE_2(name, var1_type, var2_type, var1_name, var2_name): Two member variables.
	  EVENTTYPE_3(name, var1_type, var2_type, var3_type, var1_name, var2_name, var3_name): Three members.

	Comments directly above each macro will be visible in the IntelliSense tooltip.  You can use this
	to write a description which will be available to anyone trying to use the Event type.
*/

// Note: during development, I'm filling this file with hypotheticals for testing.

// Player information

// The 3D coordinates of the player's location.  This is broadcasted every update.
EVENTTYPE_3(PlayerPosition, float, float, float, X, Y, Z)
// An alert that the player is out of ammo.
EVENTTYPE(PlayerOOA)
// An alert that the player took damage last Update().
EVENTTYPE(PlayerHurt)

// Gameplay

// Broadcasted when the score changes.
EVENTTYPE_1(Score, int, NewScore)
// Broadcasted when the player pauses the game.
EVENTTYPE(PauseGame)
// Broadcasted when changing game modes.
EVENTTYPE_1(NewGameMode, int, ModeId)

// Health

EVENTTYPE_2(HealthAOE, float, int, Position, HealthDelta)
EVENTTYPE_1(Damage, int, HealthLost)

//AI

EVENTTYPE(AI_Distracted)
EVENTTYPE(AI_Curious)
EVENTTYPE(AI_Pursuit)
EVENTTYPE(AI_Bunkering)