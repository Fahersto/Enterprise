#pragma once
#include "EP_PCH.h"

namespace Enterprise::Exceptions
{
	/// Thrown when an Enterprise assertion fails.
	struct AssertFailed : public std::exception
	{
		const char* what() const noexcept {
			return "An EP_ASSERT() has failed.";
		}
	};
}