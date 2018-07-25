This document describes building a universal version of openModellerDesktop on OSX
                     Tim Sutton March 2008
-----------------------------------------------------------------------------------


Set the LIB_DIR environmental variable - we will use this everywhere to refer
to where the libs live.  Avoid using a home dir for LIB_DIR if you want the
libs to be transportable to another system.

export LIB_DIR=/usr/local/qgis_universal_deps


General procedure for creating a universal lib:
-----------------------------------------------------------------------------------

./configure --prefix=${LIB_DIR} --disable-debug && make && sudo make install
make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure -host=ppc-apple-darwin -disable-debug --prefix=${LIB_DIR}; make
sudo lipo -create .libs/libgsl.0.10.0.dylib ${LIB_DIR}/lib/libgsl.0.10.0.dylib -output ${LIB_DIR}/lib/libgsl.0.10.0.dylib

Verify that its a universal build:

[gsl-1.9] otool -h -arch all ${LIB_DIR}/lib/libgsl.0.10.0.dylib 
/usr/local/qgis_universal_deps/lib/libgsl.0.10.0.dylib (architecture i386):
Mach header
magic cputype cpusubtype   filetype ncmds sizeofcmds      flags
0xfeedface       7          3          6     8       1340 0x00002004
/usr/local/qgis_universal_deps/lib/libgsl.0.10.0.dylib (architecture ppc):
Mach header
magic cputype cpusubtype   filetype ncmds sizeofcmds      flags
0xfeedface      18          0          6     7       1264 0x00000004

Type 7 is ppc and type 18 is i386
  
Get the libs:
-----------------------------------------------------------------------------------

curl -o libpng-1.2.25.tar.bz2 ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.2.25.tar.bz2
curl -o gsl-1.9.tar.gz ftp://ftp.gnu.org/gnu/gsl/gsl-1.9.tar.gz
curl -o gdal-1.5.0.tar.gz ftp://ftp.remotesensing.org/gdal/gdal-1.5.0.tar.gz
curl -o sqlite-3.5.6.tar.gz http://www.sqlite.org/sqlite-3.5.6.tar.gz
and others as needed

Building Qt4.3.3:
-----------------------------------------------------------------------------------
Options explained:
-arch x86 -arch ppc   --> build universal binaries
-no-nis               --> cruft removal
-no-qdbus             --> cruft removal
-nomake examples      --> dont waste time / space making examples 
-nomake demos         --> dont waste time / space making demos
-qt-libpng            --> use internal copy of png lib
-qt-zlib              --> use internal copy of zip lib
-qt-sql-sqlite        --> compile sqlite support directly into QtSql instead of 
                          making it a plugin

./configure -arch x86 -arch ppc -no-qdbus -no-nis -silent -nomake examples -nomake demos -qt-libpng -qt-zlib -qt-sql-sqlite -prefix ${LIB_DIR}

make
sudo make install

Building GEOS
-----------------------------------------------------------------------------------
  
cd dev/src/geos-2.2.3/
./configure --prefix=${LIB_DIR} --disable-debug ; make; sudo make install
make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure --prefix=${LIB_DIR} -host=ppc-apple-darwin -disable-debug; make
sudo lipo -create source/geom/.libs/libgeos.2.2.3.dylib ${LIB_DIR}/lib/libgeos.2.2.3.dylib -output ${LIB_DIR}/lib/libgeos.2.2.3.dylib
sudo lipo -create source/capi/.libs/libgeos_c.1.1.1.dylib ${LIB_DIR}/lib/libgeos_c.1.1.1.dylib -output ${LIB_DIR}/lib/libgeos_c.1.1.1.dylib

Building Proj
-----------------------------------------------------------------------------------

./configure --prefix=${LIB_DIR} --disable-debug && make && sudo make install
make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure --prefix=${LIB_DIR} -host=ppc-apple-darwin -disable-debug; make
sudo lipo -create src/.libs/libproj.0.5.4.dylib ${LIB_DIR}/lib/libproj.0.5.4.dylib -output ${LIB_DIR}/lib/libproj.0.5.4.dylib

Verify:

otool -h -arch all ${LIB_DIR}/lib/libproj.0.5.4.dylib

Building Expat
-----------------------------------------------------------------------------------

./configure --prefix=${LIB_DIR} --disable-debug && make && sudo make install
make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure --prefix=${LIB_DIR} --disable-debug  && make
sudo lipo -create .libs/libexpat.1.5.2.dylib ${LIB_DIR}/lib/libexpat.1.5.2.dylib -output ${LIB_DIR}/lib/libexpat.1.5.2.dylib

Verify:

otool -h -arch all ${LIB_DIR}/lib/libexpat.1.5.2.dylib

Building sqlite3
-----------------------------------------------------------------------------------

./configure --prefix=${LIB_DIR} --disable-debug && make && sudo make install
make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure -host=ppc-apple-darwin -disable-debug --prefix=${LIB_DIR}; make


sudo lipo -create .libs/libsqlite3.0.8.6.dylib ${LIB_DIR}/lib/libsqlite3.0.8.6.dylib  -output ${LIB_DIR}/lib/libsqlite3.0.8.6.dylib 

Verify:

otool -h -arch all  ${LIB_DIR}/lib/libsqlite3.0.8.6.dylib


Building GDAL
-----------------------------------------------------------------------------------


./configure --prefix=${LIB_DIR} --disable-debug --with-png=internal --with-expat=${LIB_DIR} --with-sqlite3=${LIB_DIR} --with-geos=${LIB_DIR} --with-pg=no --with-static-proj4=${LIB_DIR} --with-netcdf=no && make && sudo make install

* Note: the with-static-proj option above is misleadingly named. If you set it
  to no it will try at run time to dlload() the proj library by looking for it
  in the default system path or using
  CPLSetConfigOption("PROJSO","foopath/proj.dylib"); at runtime to set the
  search path. On the other hand if you use --with-static-proj4=yes it will
  dynamically link the proj dll to the gdal binary, or if you use
  --with-static-proj=${LIBS} as we do above it will dynamically link the
  specific gdal in the path you give. In not cases will proj be statically
  linked.

For some reason it uses a relative path for expat so we need to undo that or other apps witn compile

sudo install_name_tool -change @executable_path/lib/libexpat.dylib ${LIB_DIR}/lib/libexpat.dylib ${LIB_DIR}/lib/libgdal.1.dylib

and for proj

sudo install_name_tool -change @executable_path/lib/libproj.dylib ${LIB_DIR}/lib/libproj.dylib ${LIB_DIR}/lib/libgdal.dylib

make distclean
CC="gcc -arch ppc" CXX="g++ -arch ppc" ./configure --prefix=${LIB_DIR} --disable-debug --with-png=internal --with-expat=${LIB_DIR} --with-sqlite3=${LIB_DIR} --with-geos=${LIB_DIR} --with-pg=no --with-static-proj4=${LIB_DIR} --with-netcdf=no && make

Once again we need to fix the relative expat path:

install_name_tool -change @executable_path/lib/libexpat.dylib ${LIB_DIR}/lib/libexpat.dylib .libs/libgdal.1.dylib

and for proj

install_name_tool -change @executable_path/lib/libproj.dylib ${LIB_DIR}/lib/libproj.dylib .libs/libgdal.1.dylib

sudo lipo -create .libs/libgdal.1.dylib ${LIB_DIR}/lib/libgdal.1.dylib -output ${LIB_DIR}/lib/libgdal.1.dylib



Verify:

otool -h -arch all ${LIB_DIR}/lib/libgdal.dylib



Building QGIS
-----------------------------------------------------------------------------------

export LIB_DIR=/usr/local/qgis_universal_deps; ccmake -DCMAKE_INSTALL_PREFIX=/Applications -DCMAKE_OSX_ARCHITECTURES="i386;ppc" -DQT_QMAKE_EXECUTEABLE=${LIB_DIR}/bin/qmake -DGDAL_CONFIG=${LIB_DIR}/bin/gdal-config -DGDAL_CONFIG_PREFER_PATH=${LIB_DIR}/bin/ -DGDAL_INCLUDE_DIR=${LIB_DIR}/include -DGDAL_LIBRARY=${LIB_DIR}/lib/libgdal.dylib -DWITH_BINDINGS=OFF -DWITH_GRASS=OFF -DWITH_POSTGRESQL=OFF -DGSL_CONFIG=${LIB_DIR}/bin/gsl-config -DGSL_CONFIG_PREFER_PATH=${LIB_DIR}/bin -DQT_LRELEASE_EXECUTABLE=${LIB_DIR}/bin/lrelease -DQT_MKSPECS_DIR=${LIB_DIR}/mkspecs -DQT_PLUGINS_DIR=${LIB_DIR}/plugins -DQT_QMAKE_EXECUTABLE=${LIB_DIR}/bin/qmake -DWITH_INTERNAL_SQLITE3=on ..

Note: you'll likely need to create the following symbolic link before compiling qgis:

sudo ln -s /usr/local /Developer/SDKs/MacOSX10.4u.sdk/usr/local

Building openModellerDesktop
-----------------------------------------------------------------------------------

export LIB_DIR=/usr/local/qgis_universal_deps; ccmake -DCMAKE_INSTALL_PREFIX=/Applications -DCMAKE_OSX_ARCHITECTURES="i386;ppc" -DQT_QMAKE_EXECUTEABLE=${LIB_DIR}/bin/qmake -DGDAL_CONFIG=${LIB_DIR}/bin/gdal-config -DGDAL_CONFIG_PREFER_PATH=${LIB_DIR}/bin/ -DGDAL_INCLUDE_DIR=${LIB_DIR}/include -DGDAL_LIBRARY=${LIB_DIR}/lib/libgdal.dylib -DWITH_BINDINGS=OFF -DWITH_GRASS=OFF -DWITH_POSTGRESQL=OFF -DGSL_CONFIG=${LIB_DIR}/bin/gsl-config -DGSL_CONFIG_PREFER_PATH=${LIB_DIR}/bin -DQT_LRELEASE_EXECUTABLE=${LIB_DIR}/bin/lrelease -DQT_MKSPECS_DIR=${LIB_DIR}/mkspecs -DQT_PLUGINS_DIR=${LIB_DIR}/plugins -DQT_QMAKE_EXECUTABLE=${LIB_DIR}/bin/qmake ..

