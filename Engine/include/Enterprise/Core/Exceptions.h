#pragma once
#include <exception>

namespace Enterprise::Exceptions
{
	/// Thrown when an Enterprise assertion fails.
	struct AssertFailed : public std::exception
	{
		const char* what() const noexcept {
			return "An Enterprise assertion has failed.";
		}
	};

	/// Thrown in the case of a fatal error.
	struct FatalError : public std::exception
	{
		const char* what() const noexcept {
			return "A fatal error has occurred.";
		}
	};
}