/* StartClasses.h
	Include this file before including a "_TypeList.h" to define new Event classes for the Event system.

	WARNINGS:
	- Do not add a header guard to this file.  It is included in multiple files.
	- After you've used this file in conjunction with a "_TypeList.h" file, you must include EndClasses.h
	  to undefine these definitions.
*/

// Macros for event logging code
#ifdef EP_CONFIG_DEBUG
#define EVENTCLASS_DEBUGMEMBERS(name) \
		virtual const char* GetName() const override { return #name; }
#define EVENTCLASS_DEBUGMEMBERS_1(name, var1_type, var1_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ")"; \
			return ss.str(); \
		}
#define EVENTCLASS_DEBUGMEMBERS_2(name, var1_type, var1_name, var2_type, var2_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ", " << #var2_name << "=" << m_##var2_name << ")"; \
			return ss.str(); \
		}
#define EVENTCLASS_DEBUGMEMBERS_3(name, var1_type, var1_name, var2_type, var2_name, var3_type, var3_name) \
		virtual const char* GetName() const override { return #name; } \
		std::string ToString() const override \
		{ \
			std::stringstream ss; \
			ss << GetName() << " (" << #var1_name << "=" << m_##var1_name << ", " << \
				#var2_name << "=" << m_##var2_name << ", " << #var3_name << "=" << m_##var3_name << ")"; \
			return ss.str(); \
		}
#else
#define EVENTCLASS_DEBUGMEMBERS(...) 
#define EVENTCLASS_DEBUGMEMBERS_1(...) 
#define EVENTCLASS_DEBUGMEMBERS_2(...) 
#define EVENTCLASS_DEBUGMEMBERS_3(...) 
#endif

// Memberless classes
#define EVENTTYPE(name) class name : public Enterprise::Event::Event \
	{ \
	public: \
		static unsigned int GetStaticTypeID() { return TypeIDs::##name; } \
		virtual unsigned int GetTypeID() const override { return GetStaticTypeID(); } \
		EVENTCLASS_DEBUGMEMBERS(name) \
	};
	// One-member classes
#define EVENTTYPE_1(name, var1_type, var1_name) class name : public Enterprise::Event::Event \
	{ \
	public: \
		name(var1_type var1_name) \
			: m_##var1_name##(var1_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		static unsigned int GetStaticTypeID() { return TypeIDs::##name; } \
		virtual unsigned int GetTypeID() const override { return GetStaticTypeID(); } \
		EVENTCLASS_DEBUGMEMBERS_1(name, var1_type, var1_name) \
	private: \
		var1_type m_##var1_name; \
	};
	// Two-member classes
#define EVENTTYPE_2(name, var1_type, var1_name, var2_type, var2_name) class name : public Enterprise::Event::Event \
	{ \
	public: \
		name(var1_type var1_name, var2_type var2_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		inline var2_type Get_##var2_name##() { return m_##var2_name; } \
		static unsigned int GetStaticTypeID() { return TypeIDs::##name; } \
		virtual unsigned int GetTypeID() const override { return GetStaticTypeID(); } \
		EVENTCLASS_DEBUGMEMBERS_2(name, var1_type, var1_name, var2_type, var2_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
	};
	// Three-member classes
#define EVENTTYPE_3(name, var1_type, var1_name, var2_type, var2_name, var3_type, var3_name) class name : public Enterprise::Event::Event \
	{ \
	public: \
		name(var1_type var1_name, var2_type var2_name, var3_type var3_name) \
			: m_##var1_name##(var1_name), m_##var2_name##(var2_name), m_##var3_name##(var3_name) {} \
		inline var1_type Get_##var1_name##() { return m_##var1_name; } \
		inline var2_type Get_##var2_name##() { return m_##var2_name; } \
		inline var3_type Get_##var3_name##() { return m_##var3_name; } \
		static unsigned int GetStaticTypeID() { return TypeIDs::##name; } \
		virtual unsigned int GetTypeID() const override { return GetStaticTypeID(); } \
		EVENTCLASS_DEBUGMEMBERS_3(name, var1_type, var1_name, var2_type, var2_name, var3_type, var3_name) \
	private: \
		var1_type m_##var1_name; \
		var2_type m_##var2_name; \
		var3_type m_##var3_name; \
	};