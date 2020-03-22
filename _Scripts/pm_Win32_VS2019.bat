@echo off
pushd %~dp0\..\
CALL vendor\bin\premake\premake5.exe vs2019
popd
PAUSE