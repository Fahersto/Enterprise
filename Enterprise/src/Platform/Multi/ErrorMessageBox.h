#pragma once

/* ErrorMessageBox.h
	Contains a function which generates an error message pop-up.
	The correct platform code is selected by the preprocessor.
*/

namespace Enterprise::Platform {
	void DisplayErrorDialog(std::wstring& ErrorMessage); //TODO: add a version which accepts std::string
}