#!/bin/bash
cd "`dirname "$0"`"
cd "../Enterprise/src"
# Run Doxygen (assumes installation is in Applications folder)
/Applications/Doxygen.app/Contents/Resources/doxygen ../doxygen/_build/Doxyfile
# Copy home.html (index page redirect)
cp ../doxygen/_build/main_redirect.html ../../docs/home.html

read -s -n 1 -p "Press any key to continue . . ."
echo ""
