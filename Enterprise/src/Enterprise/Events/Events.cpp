#include "EP_PCH.h"
#include "Events.h"

#include "Enterprise/Application/Application.h" //needed for Application::Quit(). (GET RID OF THIS!)

// Singletons -------------------------------------------------------------------------------------
// Note: these MUST be singletons, as they are invoked during static initialization.

// std::map associating EventCategorys to vectors of associated EventTypes.
std::map<EventCategory, std::vector<EventType>>& _categoryMap() {
	static auto instance = new std::map<EventCategory, std::vector<EventType>>();
	return *instance;
}
// Current number of registered categories.
unsigned int& _categoryCount() {
	static auto count = new unsigned int(0);
	return *count;
}
#ifdef EP_CONFIG_DEBUG
// Map associating EventCategorys to their string names.
std::map<EventCategory, const char*>& _categoryDebugNames() {
	static auto instance = new std::map<EventCategory, const char*>();
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
	// Registration functions ------------------------------------------------------------------------

	#ifndef EP_CONFIG_DEBUG
	EventCategory Events::RegisterCategory()
	#else
	EventCategory Events::RegisterCategory(const char* debugName)
	#endif
	{
		// Generate bit field for this category
		EventCategory returnVal = BIT(_categoryCount()++); // TODO: Disallow duplicate entries.
		EP_ASSERT(_categoryCount() < 64); //Limit is 64 bits for bit fields.

		// Add category to _categoryMap
		auto result = _categoryMap().try_emplace(returnVal, std::vector<EventType>());
		EP_ASSERT(result.second); //If this is false, emplace failed.

		// Add debug name to _categoryStringNames
		#ifdef EP_CONFIG_DEBUG
		_categoryDebugNames().emplace(returnVal, debugName);
		#endif

		return returnVal;
	}

	#ifndef EP_CONFIG_DEBUG
	EventType Events::RegisterType(EventCategory categories)
	#else
	EventType Events::RegisterType(EventCategory categories, const char* debugName)
	#endif
	{
		// Generate ID for this type
		static unsigned int count = 0;
		EventType returnVal = count++;

		// Add type to appropriate _categoryMap categories
		try
		{
			for (unsigned long long i = 0; i < _categoryCount(); ++i)
			{
				if (categories & BIT(i))
					_categoryMap().at(BIT(i)).emplace_back(returnVal);
			}
		}
		catch (std::out_of_range) //Only expected to be thrown by _categoryMap.at() failing
		{
			EP_FATAL("Fatal error: Event::RegisterType() was passed an undefined EventCategory.");
			Application::Quit(); // TODO: This situation won't do at all, since this is called during static initialization.
		}

		// Add debug name to _typeStringNames
		#ifdef EP_CONFIG_DEBUG
		_typeDebugNames().emplace_back(debugName);
		#endif

		return returnVal;
	}

	// Debug string getters --------------------------------------------------------------------------
	#ifdef EP_CONFIG_DEBUG
	const char* Events::GetCategoryDebugName(EventCategory category)
	{
		// TODO: Handle combinations of categories
		try { return _categoryDebugNames().at(category); }
		catch (std::out_of_range)
		{
			EP_ERROR("Error: Events::getCategoryDebugName() passed unregistered EventCategory.");
			return "MISSING_CATEGORY_NAME";
		}
	}
	const char* Events::GetTypeDebugName(EventType type)
	{
		try { return _typeDebugNames().at(type); }
		catch (std::out_of_range)
		{
			EP_ERROR("Error: Events::getTypeDebugName() passed unregistered EventType.");
			return "MISSING_TYPE_NAME";
		}
	}
	#endif
}