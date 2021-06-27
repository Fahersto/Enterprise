@echo off
pushd %~dp0\..\Enterprise\src

:: Delete any old documentation
rmdir /S /Q ..\..\docs

:: Create .dox files from .md files
powershell -command "Copy-Item ..\doxygen ..\..\docs\dox\ -Filter {PSIsContainer} -Recurse -Force"
setlocal enabledelayedexpansion
for /R ..\doxygen %%f in (*.md) do (
    powershell -command "Get-Content '%%f' | %%{ \"namespace Enterprise`n{`n///.\" }{ '/// ' + $_ }{ '}' } | "^
    "Set-Content (('..\..\docs\dox\' + ('%%f').Substring(('%%f').LastIndexOf('Enterprise\doxygen\') + 19)) + '.dox') -Force"
)
setlocal disabledelayedexpansion

:: Run Doxygen (assumes installation in Program Files)
CALL "%PROGRAMFILES%\doxygen\bin\doxygen.exe" ../doxygen/_build/Doxyfile
:: Copy home.html (index page redirect)
copy ..\doxygen\_build\main_redirect.html ..\..\docs\home.html
:: Launch documentation
start "" ..\..\docs\home.html

popd
