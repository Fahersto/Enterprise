#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise {
	// What this Event does
	enum class EventType
	{
		None = 0,

		// Window events
		WindowClose, WindowMove, WindowResize, WindowFocus, WindowLostFocus,

		// Input events
		KeyDown, KeyUp, KeyChar,
		MouseButtonDown, MouseButtonUp, MouseDelta, MouseScroll, MousePosition,
		GamepadButtonDown, GamepadButtonUp, GamepadAxis, GamepadVec2,

		// File events
		FileOpenRequest, FileOpenComplete, FileSaveRequest, FileSaveComplete,

		// Audio events
		PlayAudioAsset, PauseAudioAsset, StepAudioAsset, AudioParameter, StopAudioAsset
	};

	// Event Category flags (bitfield)
	enum EventCategory
	{
		None = 0,

		EventCategory_Application	= BIT(0),

		EventCategory_Input			= BIT(1),
		EventCategory_KeyboardRaw	= BIT(2),
		EventCategory_KeyboardChar	= BIT(3),
		EventCategory_MouseButtons	= BIT(4),
		EventCategory_MousePosition		= BIT(5),
		EventCategory_MouseDelta	= BIT(6),
		EventCategory_Gamepad		= BIT(7),

		EventCategory_File			= BIT(8),

		EventCategory_Audio			= BIT(9),

		EventCategory_Gameplay		= BIT(10)
	};
	
	// Helper macros for creating new Event classes
	#ifdef EP_CONFIG_DEBUG
	#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType() { return type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }
	#define EVENT_CLASS_DEBUGSTRING(...)	std::string ToString() const override \
									{ \
										std::stringstream ss; \
										ss << GetName() << " " << __VA_ARGS__; \
										return ss.str(); \
									}
	#else
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }
	#define EVENT_CLASS_DEBUGSTRING(...)	
	#endif

	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;
		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }

		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif

	protected:
		bool isHandled = false;
	};

	// Output stream operator (allows direct logging of Event objects)
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	#endif
}