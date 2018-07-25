#!/bin/sh
# Display all paths to supporting libraries
# Output should be visually inspected for paths which haven't been made relative (such as /usr/local)

#PREFIX=qgis.app/Contents/MacOS
PREFIX=/Applications/openModellerDesktop.app/Contents/MacOS

for file in $PREFIX/lib/*.dylib
do
  otool -L ${file}
done

for file in $PREFIX/lib/openmodeller/*.so
do
  otool -L ${file}
done

for file in $PREFIX/lib/openModellerDesktop/modeller/*.dylib
do
  otool -L ${file}
done

for file in $PREFIX/lib/openModellerDesktop/scraper/*.dylib
do
  otool -L ${file}
done

for file in $PREFIX/lib/qgis/*.so
do
  otool -L ${file}
done

