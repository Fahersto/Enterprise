#include "EP_PCH.h"
#include "Events.h"

// Singletons -------------------------------------------------------------------------------------
// Note: these MUST be singletons, as they are invoked during static initialization.

// Vector associating EventCategorys to vectors of EventTypes.
std::vector<std::vector<EventType>>& _categoryMap() {
	static auto instance = new std::vector<std::vector<EventType>>();
	return *instance;
}
#ifdef EP_CONFIG_DEBUG
// Index-aligned vector associating EventCategorys to their string names.
std::vector<const char*>& _categoryDebugNames() {
	static auto instance = new std::vector<const char*>();
	return *instance;
}
// Index-aligned vector associating EventTypes with their string names.
std::vector<const char*>& _typeDebugNames() {
	static auto instance = new std::vector<const char*>();
	return *instance;
}
#endif

namespace Enterprise
{
	// Registration functions ---------------------------------------------------------------------

	#ifndef EP_CONFIG_DEBUG
	EventCategory Events::RegisterCategory()
	#else
	EventCategory Events::RegisterCategory(const char* debugName)
	#endif
	{
		// Generate ID for this category
		static unsigned int count = 0;
		EventCategory returnVal = count++;

		// Add empty category to _categoryMap
		_categoryMap().emplace_back(std::vector<EventType>());

		// Add debug name to _categoryDebugNames
		#ifdef EP_CONFIG_DEBUG
		_categoryDebugNames().emplace_back(debugName);
		#endif

		return returnVal;
	}

	#ifndef EP_CONFIG_DEBUG
	EventType Events::RegisterType(EventCategory categories)
	#else
	EventType Events::RegisterType(const char* debugName, EventCategory categories)
	#endif
	{
		// Generate ID for this type
		static unsigned int count = 0;
		EventType returnVal = count++;

		// Add type to appropriate _categoryMap categories
		for (auto it = categories.m_IDs.begin(); it != categories.m_IDs.end(); ++it)
			_categoryMap().at(*it).emplace_back(returnVal);

		// Add debug name to _typeDebugNames
		#ifdef EP_CONFIG_DEBUG
		_typeDebugNames().emplace_back(debugName);
		#endif

		return returnVal;
	}

	// Debug string getters -----------------------------------------------------------------------
	#ifdef EP_CONFIG_DEBUG
	std::string Events::GetCategoryDebugNames(EventCategory category)
	{
		if (category.m_IDs.empty())
			return "None";
		else
		{
			std::stringstream ss;

			auto it = category.m_IDs.begin(); //first
			ss << _categoryDebugNames().at(*it);
			for (++it; it != category.m_IDs.end(); ++it) //second through last
				ss << ", " << _categoryDebugNames().at(*it);

			return ss.str();
		}
	}

	const char* Events::GetTypeDebugName(EventType type)
	{
		try { return _typeDebugNames().at(type.m_ID); }
		catch (std::out_of_range)
		{
			EP_ERROR("Error: Events::getTypeDebugName() passed unregistered EventType.");
			return "MISSING_TYPE_NAME";
		}
	}
	#endif
}