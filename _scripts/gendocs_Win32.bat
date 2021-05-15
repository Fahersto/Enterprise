@echo off
pushd %~dp0\..\Enterprise\src
:: Run Doxygen (assumes installation is in default location)
CALL "%PROGRAMFILES%\doxygen\bin\doxygen.exe" ../doxygen/_build/Doxyfile
:: Copy home.html (index page redirect)
copy ..\doxygen\_build\main_redirect.html ..\..\docs\home.html
popd

PAUSE
