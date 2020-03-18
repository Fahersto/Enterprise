#include "EP_PCH.h"
#include "Events.h"

// Singletons =========================================================================================================
// Note: these MUST be singletons, as they are invoked during static initialization!

// Lists of callback pointers, index-aligned to event type ID.
std::vector<std::vector<Enterprise::Events::EventCallbackPtr>>& Enterprise::Events::_callbackPtrs()
{
	static auto instance = new std::vector<std::vector<Enterprise::Events::EventCallbackPtr>>();
	return *instance;
}
// Vector associating category IDs to vectors of EventTypes.
std::vector<std::vector<EventType>>& _categoryMap() {
	static auto instance = new std::vector<std::vector<EventType>>();
	return *instance;
}
// Index-aligned vector associating category IDs to their string names.
std::vector<const char*>& _categoryDebugNames() {
	static auto instance = new std::vector<const char*>();
	return *instance;
}
// Index-aligned vector associating event type IDs with their string names.
std::vector<const char*>& _typeDebugNames() {
	static auto instance = new std::vector<const char*>();
	return *instance;
}


// EventCategory OR operator
EventCategory EventCategory::operator|(const EventCategory& other) const
{
	EventCategory returnVal(*this);
	// For every ID in other...
	for (auto otherit = other.m_IDs.begin(); otherit != other.m_IDs.end(); ++otherit)
	{
		// ...check that there are no matching IDs in this EventCategory.
		for (auto thisit = m_IDs.begin(); thisit != m_IDs.end(); ++thisit)
		{
			if ((*otherit) == (*thisit))
				goto NextOther;
		}
		returnVal.m_IDs.emplace_back(*otherit);
		NextOther: ;
	}
	return returnVal;
}

// EventType comparison operator
bool operator == (const EventType& left, const EventType& right) { return left.m_ID == right.m_ID; }


namespace Enterprise
{
	// Event Type and Category Registration ===========================================================================

	EventCategory Events::NewCategory(const char* debugName)
	{
		// Number of previously registered categories.
		static unsigned int count = 0;

		// If this category has already been registered, return its ID.
		for (auto it = _categoryDebugNames().begin(); it != _categoryDebugNames().end(); ++it)
		{
			if (!strcmp(debugName, *it))
				return EventCategory(it - _categoryDebugNames().begin());
		}

		EventCategory returnVal = count++;				// Generate ID for this category
		_categoryDebugNames().emplace_back(debugName);	// Add debug name to _categoryDebugNames
		_categoryMap().emplace_back();					// Add empty category to _categoryMap
		return returnVal;
	}

	EventType Events::NewType(const char* debugName, EventCategory categories)
	{
		// Number of previously registered types.
		static unsigned int count = 0;

		// If this type has already been registered, return its ID.
		for (auto it = _typeDebugNames().begin(); it != _typeDebugNames().end(); ++it)
		{
			if (!strcmp(debugName, *it))
				return EventType(it - _typeDebugNames().begin());
		}

		EventType returnVal = count++;				// Generate ID for this type
		_callbackPtrs().emplace_back();				// Create new callback stack
		_typeDebugNames().emplace_back(debugName);	// Add debug name to _typeDebugNames

		// Add type to appropriate _categoryMap categories
		for (auto it = categories.IDs().begin(); it != categories.IDs().end(); ++it)
			_categoryMap().at(*it).emplace_back(returnVal);
		
		return returnVal;
	}

	// Debug string getters ===========================================================================================
	std::string Events::GetCategoryDebugNames(EventCategory category)
	{
		if (category.IDs().empty())
			return "None";
		else
		{
			try 
			{
				std::stringstream ss;

				auto it = category.IDs().begin(); //first
				ss << _categoryDebugNames().at(*it);
				for (++it; it != category.IDs().end(); ++it) //second through last
					ss << ", " << _categoryDebugNames().at(*it);

				return ss.str();
			}
			catch (std::out_of_range)
			{
				EP_ERROR("Error: Events::GetCategoryDebugNames() was passed an unregistered category ID.");
				return "MISSING_CATEGORY_NAMES";
			}
		}
	}

	const char* Events::GetTypeDebugName(EventType type)
	{
		try { return _typeDebugNames().at(type.ID()); }
		catch (std::out_of_range)
		{
			EP_ERROR("Error: Events::GetTypeDebugName() passed unregistered event type ID.");
			return "MISSING_TYPE_NAME";
		}
	}

	// Event subscription functions ===================================================================================

	void Events::SubscribeToType(EventType type, EventCallbackPtr callback)
	{
		// For every callback already registered for this type...
		for (auto it = _callbackPtrs().at(type.ID()).begin(); it != _callbackPtrs().at(type.ID()).end(); ++it)
		{
			// ...check that it isn't equal to the new callback.
			if ((*it) == callback)
			{
				EP_WARN("Events: An attempt was made to register the same callback twice for a single event type.  Type: {}", type);
				return; //TODO: Make this warning, and/or this check, optional.
			}
		}
		_callbackPtrs().at(type.ID()).emplace_back(callback);
	}
	void Events::SubscribeToCategories(EventCategory category, EventCallbackPtr callback)
	{
		// For every category requested...
		for (auto it = category.IDs().begin(); it != category.IDs().end(); ++it)
		{
			// ...subscribe to the types within.
			for (auto typeIt = _categoryMap().at(*it).begin(); typeIt != _categoryMap().at(*it).end(); ++typeIt)
				SubscribeToType(*typeIt, callback);
		}
	}
}