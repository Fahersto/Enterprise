@echo off
pushd %~dp0\Engine\src

:: Delete any old documentation
rmdir /S /Q ..\..\docs

:: Create .dox files from .md files
powershell -command "Copy-Item ..\doxygen ..\..\docs\dox\ -Filter {PSIsContainer} -Recurse -Force"
setlocal enabledelayedexpansion
for /R ..\doxygen %%f in (*.md) do (
    powershell -command "Get-Content '%%f' | %%{ \"namespace Enterprise`n{`n///.\" }{ '/// ' + $_ }{ '}' } | "^
    "Set-Content (('..\..\docs\dox\' + ('%%f').Substring(('%%f').LastIndexOf('Engine\doxygen\') + 15)) + '.dox') -Force"
)
setlocal disabledelayedexpansion

:: Run Doxygen (assumes installation in Program Files)
CALL "%PROGRAMFILES%\doxygen\bin\doxygen.exe" ../doxygen/build/Doxyfile
:: Copy home.html (index page redirect)
copy ..\doxygen\build\main_redirect.html ..\..\docs\home.html
:: Launch documentation
start "" ..\..\docs\home.html

popd
