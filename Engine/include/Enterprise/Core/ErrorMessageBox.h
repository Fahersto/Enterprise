#pragma once
#include <string>
#include "Enterprise/Core/SharedLibraryExports.h"

namespace Enterprise::Platform
{

/// Display a modal error pop-up.
/// @param ErrorMessage The error message.
EP_API void DisplayErrorDialog(std::wstring& ErrorMessage);

/// Display a modal error pop-up.
/// @param ErrorMessage The error message.
EP_API void DisplayErrorDialog(const wchar_t* ErrorMessage);

}
