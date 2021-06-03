#pragma once
// Core.h
// This header contains files and macros which should be included throughout the Enterprise engine library.
// In order to gain access to logging, assertions, and other low-level things, include this file.

#include "Enterprise/Core/Exceptions.h"
#include "Enterprise/Core/Assertions.h"
#include "Enterprise/Core/Console.h"
#include "Enterprise/Core/ErrorMessageBox.h"
#include "Enterprise/Core/HashNames.h"

/// Rapidly assemble bit fields
#define BIT(x) (1ull << (x))
