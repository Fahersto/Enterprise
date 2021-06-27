#!/bin/bash
cd "`dirname "$0"`"
cd "../Enterprise/src"

# Delete any old documentation
rm -r ../../docs

# Create .dox files from .md files
mkdir -p ../../docs/dox
rsync -a ../doxygen/ ../../docs/dox/ --include \*/ --exclude \*

for f in $(find ../doxygen -type f -name '*.md')
do
sed -e "s#^#/// #" -e "1s#^#namespace Enterprise\n{\n///.\n#" -e '$a\
}' $f > ../../docs/dox/${f#../doxygen/}.dox
done

# Run Doxygen (assumes installation in Applications folder)
/Applications/Doxygen.app/Contents/Resources/doxygen ../doxygen/_build/Doxyfile
# Copy home.html (index page redirect)
cp ../doxygen/_build/main_redirect.html ../../docs/home.html
# Launch documentation
open ../../docs/home.html
