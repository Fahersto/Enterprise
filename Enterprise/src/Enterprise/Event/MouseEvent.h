#pragma once
#include "Event.h"

namespace Enterprise
{
	class Event_MouseButtonDown : public Event 
	{
	public:
		Event_MouseButtonDown(int button)
			: m_Button(button) {}

		inline int GetButton() { return m_Button; }

		EVENT_CLASS_TYPE(EventType::MouseButtonDown)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_MouseButtons)
		EVENT_CLASS_DEBUGSTRING(m_Button)
	private:
		int m_Button;
	};

	class Event_MouseButtonUp : public Event
	{
	public:
		Event_MouseButtonUp(int button)
			: m_Button(button) {}

		inline int GetButton() { return m_Button; }

		EVENT_CLASS_TYPE(EventType::MouseButtonUp)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_MouseButtons)
		EVENT_CLASS_DEBUGSTRING(m_Button)
	private:
		int m_Button;
	};

	class Event_MouseDelta : public Event
	{
	public:
		Event_MouseDelta(int x, int y)
			: m_X(x), m_Y(y) {}

		inline int GetX() { return m_X; }
		inline int GetY() { return m_Y; }

		EVENT_CLASS_TYPE(EventType::MouseDelta)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_MouseDelta)
		EVENT_CLASS_DEBUGSTRING(m_Y << ", " << m_Y)
	private:
		int m_X, m_Y;
	};
	
	class Event_MouseScroll : public Event
	{
	public:
		Event_MouseScroll(int x, int y)
			: m_X(x), m_Y(y) {}

		inline int GetX() { return m_X; }
		inline int GetY() { return m_Y; }

		EVENT_CLASS_TYPE(EventType::MouseScroll)
		EVENT_CLASS_CATEGORY(EventCategory_Input)
		EVENT_CLASS_DEBUGSTRING(m_Y << ", " << m_Y)
	private:
		int m_X, m_Y;
	};

	class Event_MousePosition : public Event
	{
	public:
		Event_MousePosition(int x, int y)
			: m_X(x), m_Y(y) {}

		inline int GetX() { return m_X; }
		inline int GetY() { return m_Y; }

		EVENT_CLASS_TYPE(EventType::MousePosition)
		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_MousePosition)
		EVENT_CLASS_DEBUGSTRING(m_X << ", " << m_Y)
	private:
		short m_X, m_Y;
	};
}