#pragma once
#include "EP_PCH.h"
#include "Core.h"

namespace Enterprise {
	
	enum class EventType
	{
		None = 0, //This is never used.  TODO: Should I remove this?

		// Application events -----------------------------------------------------------
		WindowClose, WindowMove, WindowResize, WindowFocus, WindowLostFocus,

		// Input events -----------------------------------------------------------------
		// Keyboard
		KeyDown, KeyUp, KeyChar,
		// Mouse
		MouseButtonDown, MouseButtonUp, MouseDelta, MouseScroll, MousePosition,

		// File events ------------------------------------------------------------------
		FileOpenRequest, FileOpenComplete, FileSaveRequest, FileSaveComplete,

		// Audio events -----------------------------------------------------------------
		PlayAudioAsset, PauseAudioAsset, StepAudioAsset, AudioParameter, StopAudioAsset,

		// Used to allocate resources in Dispatcher
		NumOfTypes
	};

	enum class EventCategory
	{
		None = 0,

		Application,
		Input, Keyboard, MouseDelta, MouseCursor,
		File,
		Audio,

		NumOfTypes
	};

	//int testCategories[(int)EventCategory::NumOfTypes];

	const int EventCategory_Application = BIT(0),
		EventCategory_Input = BIT(1), EventCategory_Keyboard = BIT(2), EventCategory_MouseDelta = BIT(3), EventCategory_MouseCursor = BIT(4),
		EventCategory_File = BIT(5), EventCategory_Audio = BIT(6);
	
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
		inline bool IsInCategory(int category) { return GetCategoryFlags() & category; }

		//shared pointer getter function (in progress)
		//static inline std::shared_ptr<Event> Ptr() { return std::make_shared<Event>(); }

		#ifdef EP_CONFIG_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
		#endif
	};

	// Output stream operator (allows direct logging of Event objects and shared_ptr's to Events)
	#ifdef EP_CONFIG_DEBUG
	inline std::ostream& operator << (std::ostream& os, const Event& e) { return os << e.ToString(); }
	inline std::ostream& operator << (std::ostream& os, std::shared_ptr<Event> e) { return os << e->ToString(); }
	#endif
}