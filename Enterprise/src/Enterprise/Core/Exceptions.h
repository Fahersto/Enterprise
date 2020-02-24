#pragma once
#include "EP_PCH.h"

/* Exceptions.h
	Contains the custom Enterprise exception classes.
*/
namespace Enterprise::Exceptions
{
	// Thrown when Enterprise encounters a fatal error.  Handled in main function.
	struct AssertFailed : public std::exception
	{
		const char* what() const noexcept {
			return "An EP_ASSERT has failed.  Handle this exception in the main function by terminating the application.";
		}
	};
}