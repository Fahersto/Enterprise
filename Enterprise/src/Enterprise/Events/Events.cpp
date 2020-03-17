#include "EP_PCH.h"
#include "Events.h"

// Singletons =========================================================================================================
// Note: these MUST be singletons, as they are invoked during static initialization.

// Vector associating EventCategorys to vectors of EventTypes.
std::vector<std::vector<EventType>>& _categoryMap() {
	static auto instance = new std::vector<std::vector<EventType>>();
	return *instance;
}
std::vector<std::vector<Enterprise::Events::EventCallbackPtr>>& Enterprise::Events::_callbackPtrs()
{
	static auto instance = new std::vector<std::vector<Enterprise::Events::EventCallbackPtr>>();
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

bool operator == (const EventType& left, const EventType& right) { return left.m_ID == right.m_ID; }

namespace Enterprise
{
	// Static Type and Category Registration =============================================================================

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
		_categoryMap().emplace_back();

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

		// Create new callback stack
		_callbackPtrs().emplace_back();

		// Add type to appropriate _categoryMap categories
		for (auto it = categories.m_IDs.begin(); it != categories.m_IDs.end(); ++it)
			_categoryMap().at(*it).emplace_back(returnVal);

		// Add debug name to _typeDebugNames
		#ifdef EP_CONFIG_DEBUG
		_typeDebugNames().emplace_back(debugName);
		#endif

		return returnVal;
	}

	// Debug string getters ===========================================================================================
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
		try { return _typeDebugNames().at(type.ID()); }
		catch (std::out_of_range)
		{
			EP_ERROR("Error: Events::getTypeDebugName() passed unregistered EventType.");
			return "MISSING_TYPE_NAME";
		}
	}
	#endif

	// Event subscription functions ===================================================================================

	void Events::SubscribeToType(EventType type, EventCallbackPtr callback)
	{
		// For every callback already registered for this type...
		for (auto it = _callbackPtrs().at(type.ID()).begin(); it != _callbackPtrs().at(type.ID()).end(); ++it)
		{
			// ...check that it isn't equal to the new callback.
			if ((*it) == callback)
			{
				//TODO: Find a way to turn off warnings for this from the client.
				EP_WARN("Events: An attempt was made to register the same callback for a single EventType twice.\n"
					"Are you attempting to subscribe to overlapping event categories? Type:{}", type);
				return;
			}
		}
		_callbackPtrs().at(type.ID()).emplace_back(callback);
	}
	void Events::SubscribeToCategories(EventCategory category, EventCallbackPtr callback)
	{
		// For every category requested...
		for (auto it = category.m_IDs.begin(); it != category.m_IDs.end(); ++it)
		{
			// ...subscribe to the types within.
			for (auto typeIt = _categoryMap().at(*it).begin(); typeIt != _categoryMap().at(*it).end(); ++typeIt)
				SubscribeToType(*typeIt, callback);
		}
	}
}