#pragma once
// Core.h
// This header contains files and macros which should be included throughout the Enterprise engine library.
// In order to gain access to logging, assertions, and other low-level things, include this file.

#include "Enterprise/Core/SharedLibraryExports.h"
#include "Enterprise/Core/Exceptions.h"
#include "Enterprise/Core/Assertions.h"
#include "Enterprise/Core/Console.h"
#include "Enterprise/Core/ErrorMessageBox.h"
#include "Enterprise/Core/HashNames.h"

/// Rapidly assemble bit fields
#define BIT(x) (1ull << (x))


/// @cond DOXYGEN_SKIP
// https://stackoverflow.com/questions/5768511/using-sfinae-to-check-for-global-operator
namespace has_insertion_operator_impl
{
typedef char no;
typedef char yes[2];

struct any_t {
	template<typename T> any_t(T const&);
};

no operator<<(std::ostream const&, any_t const&);

yes& test(std::ostream&);
no test(no);

template<typename T>
struct has_insertion_operator {
	static std::ostream& s;
	static T const& t;
	static bool const value = sizeof(test(s << t)) == sizeof(yes);
};
}
/// @endcond

/// Check whether a type supports the operator "<<" for stream insertion.
/// @tparam T The type to check.
template<typename T>
struct has_insertion_operator :
	has_insertion_operator_impl::has_insertion_operator<T> {
};
