#pragma once
#include "Core.h"
#include "Events.h"

namespace Enterprise::Events::EventCategories
{
	//System categories
	EP_EVENTCATEGORY(Window); //Events generated when the player interacts with the game window.
	//EP_EVENTCATEGORY(File);
	//EP_EVENTCATEGORY(Network);
	//
	EP_EVENTCATEGORY(Input); //All Input events.
	EP_EVENTCATEGORY(Keyboard); //Direct keyboard events (for text entry, use EventType KeyChar).
	EP_EVENTCATEGORY(MouseDelta); //Mouse input events used in pointer-less control (e.g. FPS-style).
	EP_EVENTCATEGORY(MouseCursor); //Mouse input events used in pointer-based control.

	//EP_EVENTCATEGORY(Graphics);
	//EP_EVENTCATEGORY(Audio);
	//EP_EVENTCATEGORY(ECS);
	//EP_EVENTCATEGORY(StateStack);
}

namespace Enterprise::Events::EventTypes
{
	EP_EVENTTYPE(WindowClose, EventCategories::Window); //User clicked the close button on the game window.
	EP_EVENTTYPE(WindowMove, EventCategories::Window); //User has dragged the window to a new location.
	EP_EVENTTYPE(WindowResize, EventCategories::Window); //User has resized the window.
	EP_EVENTTYPE(WindowFocus, EventCategories::Window); //The game window has gained focus.
	EP_EVENTTYPE(WindowLostFocus, EventCategories::Window); //The game window has lost focus.

	EP_EVENTTYPE(KeyChar, EventCategories::Input); //A character was just entered with the keyboard.
	EP_EVENTTYPE(MousePosition, EventCategories::Input | EventCategories::MouseCursor); //The user has moved the cursor.
}