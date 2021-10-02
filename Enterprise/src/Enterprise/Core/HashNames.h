#pragma once
#include "EP_PCH.h"

/// The hashed version of a string name.  Used throughout Enterprise.
typedef uint64_t HashName;
/// A HashName literal.  Can be used in compile-time situations, but is not subject to collision detection.
constexpr uint64_t operator ""_HN(const char* stringname, size_t len)
{
	return CTSpookyHash::Hash64(stringname, len, 0);
}

#ifdef EP_CONFIG_DEBUG

/// Convert a C string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The C string to convert.
/// @param length The number of characters in @c stringname.
/// @return The string's associated HashName.
HashName HN(const char* stringname, size_t length);

/// Convert a C string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The C string to convert.
/// @return The string's associated HashName.
HashName HN(const char* stringname);

/// Convert a string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The string to convert.
/// @return The string's associated HashName.
HashName HN(std::string stringname);

/// Returns the string associated with a HashName.
/// @param hashname The HashName to look up.
/// @return The HashName's associated string.
/// @note Because strings aren't interned in Dev and Release builds, this function 
/// just returns the hash if invoked in Dev or Dist builds.
std::string HN_ToStr(HashName hashname);

#else // EP_CONFIG_DEV, EP_CONFIG_DIST

/// Convert a C string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The C string to convert.
/// @param length The number of characters in @c stringname.
/// @return The string's associated HashName.
constexpr HashName HN(const char* stringname, size_t length)
{
	return CTSpookyHash::Hash64(stringname, length, 0);
}

/// Convert a C string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The C string to convert.
/// @return The string's associated HashName.
constexpr HashName HN(const char* stringname)
{
	return CTSpookyHash::Hash64(stringname, std::char_traits<char>::length(stringname), 0);
}

/// Convert a string into a HashName.  Performs collision checks in Debug builds.
/// @param stringname The string to convert.
/// @return The string's associated HashName.
inline HashName HN(std::string stringname)
{
	return CTSpookyHash::Hash64(stringname.c_str(), stringname.size(), 0);
}

/// Returns the string associated with a HashName.
/// @param hashname The HashName to look up.
/// @return The HashName's associated string.
/// @note Because strings aren't interned in Dev and Release builds, this function 
/// just returns the hash if invoked in Dev or Dist builds.
inline std::string HN_ToStr(HashName hashname)
{
	return std::to_string(hashname);
}

#endif

/// A null HashName.  Equivalent to HashName of empty string.
#define HN_NULL HN("")
