#pragma once
// Core.h
// This header contains files and macros which should be included throughout the Enterprise engine library.
// In order to gain access to logging, assertions, and other low-level things, include this file.

#include "Enterprise/Core/Exceptions.h"
#include "Enterprise/Core/Assertions.h"
#include "Enterprise/Core/Console.h"
#include "Enterprise/Core/ErrorMessageBox.h"
#include "Enterprise/Core/HashNames.h"

// Quickly assemble bit fields
#define BIT(x) (1ull << (x))

/// Quickly identify if all of a group of types are the same.
template<typename T, typename... Rest>
struct is_all_same : std::false_type {};
/// Quickly identify if all of a group of types are the same.
template<typename T, typename First>
struct is_all_same<T, First> : std::is_same<T, First> {};
/// Quickly identify if all of a group of types are the same.
template<typename T, typename First, typename... Rest>
struct is_all_same<T, First, Rest...> : 
	std::integral_constant<bool, 
		std::is_same<T, First>::value &&
		is_all_same<T, Rest...>::value> {};
