/* Event/EventCategoryList.h (Sandbox)
	This header file is a list of all the client EventCategorys, and what EventTypes they are associated with.
	This file deliberately has no header guard, as it is inserted into multiple locations in Enterprise.

	WARNING: despite your compiler's warnings, do not define CLIENTEVENTCATEGORY in this file.  If you do, it will
	abort the definitions in the files where this header is included.
*/

CLIENTEVENTCATEGORY(PlayerInfo)
CLIENTEVENTCATEGORY(Gameplay)
CLIENTEVENTCATEGORY(Gunplay)
CLIENTEVENTCATEGORY(AI)