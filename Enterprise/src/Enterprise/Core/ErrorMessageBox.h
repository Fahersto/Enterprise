#pragma once

namespace Enterprise::Platform
{

/// Display a modal error pop-up.
/// @param ErrorMessage The error message.
void DisplayErrorDialog(std::wstring& ErrorMessage);

/// Display a modal error pop-up.
/// @param ErrorMessage The error message.
void DisplayErrorDialog(const wchar_t* ErrorMessage);

}
