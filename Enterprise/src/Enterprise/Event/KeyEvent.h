#pragma once
#include "Event.h"

namespace Enterprise
{

	class Event_KeyDown : public Event
	{
	public:
		Event_KeyDown(int keycode)
			: m_KeyCode(keycode) {}

		inline int GetKeyCode() const { return m_KeyCode; }
		
		EVENT_CLASS_TYPE(EventType::KeyDown)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_Keyboard)
		EVENT_CLASS_DEBUGSTRING(m_KeyCode)

	private:
		int m_KeyCode;
	};

	class Event_KeyUp : public Event
	{
	public:
		Event_KeyUp(int keycode)
			: m_KeyCode(keycode) {}

		EVENT_CLASS_TYPE(EventType::KeyUp)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_Keyboard)
		EVENT_CLASS_DEBUGSTRING(m_KeyCode)
	
	private:
		int m_KeyCode;
	};
	
	class Event_KeyChar : public Event
	{
	public:
		inline int GetCharEntry() const { return m_Char; }

		Event_KeyChar(int character, int modifiers)
			: m_Char(character), m_Modifiers(modifiers) {}

		EVENT_CLASS_TYPE(EventType::KeyChar)
		EVENT_CLASS_CATEGORY(EventCategory_Input)
		EVENT_CLASS_DEBUGSTRING(m_Char << ", modifiers not yet handled")

	private:
		int m_Char, m_Modifiers;
	};

}