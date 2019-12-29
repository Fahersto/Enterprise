//#pragma once
//#include "Event.h"
//
//namespace Enterprise {
//	class Event_WindowClose : public Event
//	{
//	public:
//		EVENT_CLASS_TYPE(EventType::WindowClose)
//		EVENT_CLASS_CATEGORY(EventCategory_Application)
//	};
//
//	class Event_WindowMove : public Event
//	{
//	public:
//		Event_WindowMove(int x, int y)
//			: m_X(x), m_Y(y) {}
//
//		inline int GetX() { return m_X; }
//		inline int GetY() { return m_Y; }
//
//		EVENT_CLASS_TYPE(EventType::WindowMove)
//		EVENT_CLASS_CATEGORY(EventCategory_Application)
//		EVENT_CLASS_DEBUGSTRING(m_X << ", " << m_Y)
//	private:
//		int m_X, m_Y;
//	};
//
//	class Event_WindowResize : public Event
//	{
//	public:
//		Event_WindowResize(int width, int height) 
//			: m_Width(width), m_Height(height) {}
//
//		inline int GetWidth() { return m_Width; }
//		inline int GetHeight() { return m_Height; }
//
//		EVENT_CLASS_TYPE(EventType::WindowResize)
//		EVENT_CLASS_CATEGORY(EventCategory_Application)
//		EVENT_CLASS_DEBUGSTRING(m_Width << ", " << m_Height)
//	private:
//		unsigned int m_Width, m_Height;
//	};
//
//	class Event_WindowFocus : public Event
//	{
//	public:
//		EVENT_CLASS_TYPE(EventType::WindowFocus)
//		EVENT_CLASS_CATEGORY(EventCategory_Application)
//	};
//
//	class Event_WindowLostFocus : public Event
//	{
//	public:
//		EVENT_CLASS_TYPE(EventType::WindowLostFocus)
//		EVENT_CLASS_CATEGORY(EventCategory_Application)
//	};
//}