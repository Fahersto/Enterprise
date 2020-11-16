#pragma once
#include "EP_PCH.h"

/// The hashed version of a string name.  Used throughout Enterprise.
typedef uint64_t HashName;
/// The hashed version of a string name.  Used throughout Enterprise.
constexpr uint64_t operator ""_HN(const char* string, size_t len)
{
	return SpookyHash::Hash64(string, len, 0);
}

/// Convert a string into a HashName.  Performs collision checks.
/// @param stringname The string to convert.
/// @return The string's associated HashName.
HashName HN(std::string stringname);

/// Returns the string associated with a HashName.
/// @param hashname The HashName to look up.
/// @return The HashName's associated string.
std::string SN(HashName hashname);
