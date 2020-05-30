#pragma once

/* ErrorMessageBox.h
	Contains a function which generates an error message pop-up.
	The correct platform code is selected by the preprocessor.
*/

namespace Enterprise::Platform {
	void DisplayErrorDialog(std::wstring& ErrorMessage);
    void DisplayErrorDialog(const wchar_t* ErrorMessage);
}
