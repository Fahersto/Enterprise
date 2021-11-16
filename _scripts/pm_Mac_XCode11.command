#!/bin/bash
projectname="Sandbox"
cd "`dirname "$0"`"
cd ".."

# Run Premake
_vendor/premake/premake5 xcode4

# Parse the BlueprintIdentifier from the generated game project
lineno=$(sed -n '/Begin PBXNativeTarget section/=' "./$projectname/$projectname.xcodeproj/project.pbxproj")
((lineno+=1))
bpid=$(sed -n "$lineno p" "./$projectname/$projectname.xcodeproj/project.pbxproj" | cut -c 3-26)

# Copy template.xcscheme to the game project, substituting the project name and BlueprintIdentifier
mkdir -p "$projectname/$projectname.xcodeproj/xcshareddata/xcschemes"
sed "s/EP_BLUEPRINTID/$bpid/g" "_scripts/template.xcscheme" | sed "s/EP_PROJECTNAME/$projectname/g" > "./$projectname/$projectname.xcodeproj/xcshareddata/xcschemes/$projectname.xcscheme"

read -s -n 1 -p "Press any key to continue . . ."
echo ""
