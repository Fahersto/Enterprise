#!/bin/bash
cd "`dirname "$0"`"
cd "../Enterprise/src"

# Delete any old documentation
rm -r ../../docs
# Run Doxygen (assumes installation in Applications folder)
/Applications/Doxygen.app/Contents/Resources/doxygen ../doxygen/_build/Doxyfile
# Copy home.html (index page redirect)
cp ../doxygen/_build/main_redirect.html ../../docs/home.html
# Launch documentation
open ../../docs/home.html
