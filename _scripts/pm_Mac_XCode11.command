#!/bin/bash
cd "`dirname "$0"`"
cd ".."
# Add user permission to execute premake
chmod u+x _vendor/premake/premake5
# Invoke the premake build script
_vendor/premake/premake5 xcode4

read -s -n 1 -p "Press any key to continue . . ."
echo ""
