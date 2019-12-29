/* Event/EventTypeList.h (Sandbox)
	This header file is a list of all the client EventTypes.  It is a list of macros which will be inserted in locations
	which will include a definition that suits that context.
	This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.

	WARNING: despite your compiler's warnings, do not define CLIENTEVENTTYPE in this file.  If you do, it will
	abort the definitions in the files where this header is included.
*/

// Example:
// CLIENTEVENTTYPE(EventTypeName)

// Currently, I'm filling this file with hypotheticals for testing.

// Player information
CLIENTEVENTTYPE(PlayerPosition)
CLIENTEVENTTYPE(PlayerOOA)
CLIENTEVENTTYPE(PlayerHurt)

// Gameplay
CLIENTEVENTTYPE(Score)
CLIENTEVENTTYPE(PauseGame)
CLIENTEVENTTYPE(NewGameMode)

// Gunplay
CLIENTEVENTTYPE(AOE)
CLIENTEVENTTYPE(Damage)

//AI
CLIENTEVENTTYPE(AI_Distracted)
CLIENTEVENTTYPE(AI_Curious)
CLIENTEVENTTYPE(AI_Pursuit)
CLIENTEVENTTYPE(AI_Bunkering)
