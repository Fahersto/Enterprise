@echo off
pushd %~dp0\..\Enterprise\src

:: Delete any old documentation
rmdir /S /Q ..\..\docs
:: Run Doxygen (assumes installation in Program Files)
CALL "%PROGRAMFILES%\doxygen\bin\doxygen.exe" ../doxygen/_build/Doxyfile
:: Copy home.html (index page redirect)
copy ..\doxygen\_build\main_redirect.html ..\..\docs\home.html
:: Launch documentation
start "" ..\..\docs\home.html

popd
