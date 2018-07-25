#!/bin/sh
# Copy supportibng libraries (except Qt) to openModeller bundle
# and make search paths for them relative to bundle


SRC_DIR=/Users/cria/projects/openmodeller/desktop/trunk
APP_PREFIX=/Applications/openModellerDesktop.app
BUNDLE_LIB_DIR=${APP_PREFIX}/Contents/MacOS/lib
LIB_PREFIX=/usr/local/om_universal_deps
LIB_DIR=${LIB_PREFIX}/lib
QGIS=/Applications/qgis1.1.0.app
OM_PREFIX=/usr/local
#set -x
cp -r $QGIS/Contents/MacOS/lib/libqgis_core.dylib ${BUNDLE_LIB_DIR}
cp -r $QGIS/Contents/MacOS/lib/libqgis_gui.dylib ${BUNDLE_LIB_DIR}
mkdir -p ${BUNDLE_LIB_DIR}/qgis
cp -r $QGIS/Contents/MacOS/lib/qgis/libogrprovider.so ${BUNDLE_LIB_DIR}/qgis
cp -r $QGIS/Contents/MacOS/lib/qgis/libcopyrightlabelplugin.so ${BUNDLE_LIB_DIR}/qgis
cp -r $QGIS/Contents/MacOS/lib/qgis/libnortharrowplugin.so ${BUNDLE_LIB_DIR}/qgis
cp -r $QGIS/Contents/MacOS/lib/qgis/libscalebarplugin.so ${BUNDLE_LIB_DIR}/qgis
mkdir -p ${APP_PREFIX}/Contents/MacOS/share/qgis/images
cp -r ${QGIS}/Contents/MacOS/share/qgis/images/north_arrows ${APP_PREFIX}/Contents/MacOS/share/qgis/images/
mkdir -p ${APP_PREFIX}/Contents/MacOS/share/qgis/resources
cp ${QGIS}/Contents/MacOS/share/qgis/resources/srs.db ${APP_PREFIX}/Contents/MacOS/share/qgis/resources/
cp -r ${OM_PREFIX}/lib/libopenmodeller.dylib ${BUNDLE_LIB_DIR}
mkdir -p ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libaquamaps.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libbioclim.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libenvelope_score.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libomneuralnet.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libenfa.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libenvironmental_distance.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libcsm_bs.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libdesktop_garp_bs.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libgarp.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libgarp_best_subsets.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libdesktop_garp.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libmaximum_entropy.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libniche_mosaic.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${OM_PREFIX}/lib/openmodeller/libomsvm.so ${BUNDLE_LIB_DIR}/openmodeller
cp -r ${LIB_DIR}/libexpat.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libgdal.1.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libgeos.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libgeos_c.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libproj.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libgsl.0.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libgslcblas.0.dylib  ${BUNDLE_LIB_DIR}
cp -r ${LIB_DIR}/libsqlite3.0.dylib  ${BUNDLE_LIB_DIR}
#cp -r ${LIB_DIR}/libqwt.dylib ${BUNDLE_LIB_DIR}
pushd .
cd ${BUNDLE_LIB_DIR}
ln -s libgdal.1.dylib libgdal.dylib
ln -s libsqlite3.0.dylib libsqlite3.dylib
ln -s libqgis_core.dylib libqgis_core.1.1.dylib
ln -s libqgis_gui.dylib libqgis_gui.1.1.dylib
ln -s libgsl.0.dylib libgsl.dylib
ln -s libgslcblas.0.dylib libgslcblas.dylib
ln -s libgeos.dylib libgeos.2.dylib
#ln -s libqwt.dylib libqwt.5.dylib
popd

#
# Proj and Gdal need to be rereferenced
#
install_name_tool -change ${LIB_DIR}/libgdal.1.dylib \
                   @executable_path/lib/libgdal.1.dylib \
                   ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
install_name_tool -change ${LIB_DIR}/libproj.dylib \
                   @executable_path/lib/libproj.dylib \
                   ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
#
# As does qwt
#
#install_name_tool -change libqwt.5.dylib \
#                   @executable_path/lib/libqwt.5.dylib \
#                   ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
#install_name_tool -change libqwt.5.dylib \
#                   @executable_path/lib/libqwt.5.dylib \
#                   ${APP_PREFIX}/Contents/MacOS/lib/libomg_core.dylib
#install_name_tool -change libqwt.5.dylib \
#                   @executable_path/lib/libqwt.5.dylib \
#                   ${APP_PREFIX}/Contents/MacOS/lib/openModellerDesktop/modeller/libomg_soapmodellerplugin.dylib
#install_name_tool -change libqwt.5.dylib \
#                   @executable_path/lib/libqwt.5.dylib \
#                   ${APP_PREFIX}/Contents/MacOS/lib/openModellerDesktop/modeller/libomg_localmodellerplugin.dylib
#
# And omg code and widgets to (todo: fix this in cmake build...!)
# 
install_name_tool -change \
  ${SRC_DIR}/build_universal/src/lib/libomg_core.dylib \
  @executable_path/lib/libomg_core.dylib \
  ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
install_name_tool -change \
  ${SRC_DIR}/build_universal/src/widgets/libomg_widgets.dylib \
  @executable_path/lib/libomg_widgets.dylib \
  ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
install_name_tool -change \
  libopenmodeller.dylib \
  @executable_path/lib/libopenmodeller.dylib \
  ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
install_name_tool -change \
  ${LIB_PREFIX}/lib/libgeos.2.dylib \
  @executable_path/lib/libgeos.dylib \
  ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
install_name_tool -change \
  /usr/local/lib/libproj.0.dylib \
  @executable_path/lib/libproj.dylib \
  ${APP_PREFIX}/Contents/MacOS/openModellerDesktop

LIBS="libqgis_core.dylib
      libqgis_gui.dylib
      libopenmodeller.dylib
      libexpat.dylib
      libomg_core.dylib
      libqgis_core.dylib
      libgsl.0.dylib
      libomg_widgets.dylib
      libqgis_gui.dylib
      libgslcblas.0.dylib
      libopenmodeller.dylib
      libexpat.dylib
      libgdal.dylib
      libgdal.1.dylib
      libgeos.dylib
      libgeos.2.dylib
      libgeos_c.dylib
      libproj.dylib
      libproj.0.dylib
      libsqlite3.dylib
      libqgis_core.dylib
      libqgis_gui.dylib
      openmodeller/libaquamaps.so
      openmodeller/libdesktop_garp_bs.so
      openmodeller/libgarp_best_subsets.so
      openmodeller/libbioclim.so
      openmodeller/libenvelope_score.so
      openmodeller/libomneuralnet.so
      openmodeller/libcsm_bs.so
      openmodeller/libenfa.so
      openmodeller/libenvironmental_distance.so
      openmodeller/libdesktop_garp.so
      openmodeller/libgarp.so
      openmodeller/libmaximum_entropy.so
      openmodeller/libniche_mosaic.so
      openmodeller/libomsvm.so
      openModellerDesktop/modeller/libomg_localmodellerplugin.dylib
      openModellerDesktop/modeller/libomg_soapmodellerplugin.dylib
      openModellerDesktop/scraper/libomg_gbifrest_plugin.dylib
      openModellerDesktop/scraper/libomg_splink_plugin.dylib
      qgis/libogrprovider.so
      qgis/libcopyrightlabelplugin.so
      qgis/libscalebarplugin.so
      qgis/libnortharrowplugin.so"

#
# First we install all the libs except Qt and QGIS libs 
# and update the library ids and paths for these libs
#
for LIB in $LIBS
do
  echo "--------------------- ***"
  echo "Checking $LIB"
  install_name_tool -id @executable_path/lib/$LIB ${BUNDLE_LIB_DIR}/${LIB}
  echo "install_name_tool -id @executable_path/lib/$LIB ${BUNDLE_LIB_DIR}/${LIB}"
  # for debugging only
  for LIBPATH in `otool -L ${BUNDLE_LIB_DIR}/${LIB} \
                  | sed 's/(\([a-zA-Z0-9\., ]*\))//g' \
                  | grep  $LIB_DIR \
                  | grep -v framework` #frameworks (in particular qt frameworks) 
                                       #get dealt with in another script
  do 
    echo $LIBPATH 
    BASELIB=`basename "$LIBPATH"`
    #echo $BASELIB
    install_name_tool -change ${LIBPATH} \
                      @executable_path/lib/${BASELIB} \
                      ${BUNDLE_LIB_DIR}/${LIB}
    install_name_tool -change ${LIBPATH} \
                      @executable_path/lib/${BASELIB} \
                      ${BUNDLE_LIB_DIR}/${LIB}
  done
  #otool -L ${BUNDLE_LIB_DIR}/${LIB}
  echo "${LIB} check done"
done
#
# Next update all libs to look for
# libopenmodeller and libproj in the correct place...
#
for LIB in $LIBS
do
    install_name_tool -change libopenmodeller.dylib \
                      @executable_path/lib/libopenmodeller.dylib \
                      ${BUNDLE_LIB_DIR}/${LIB}
    install_name_tool -change /usr/local/lib/libproj.0.dylib \
                      @executable_path/lib/libproj.dylib \
                      ${BUNDLE_LIB_DIR}/${LIB}
done
#
# Special case for gsl and gslcblas which doesnt get picked up by above stuff...
#
install_name_tool -change /usr/local/lib/libgslcblas.0.dylib \
                      @executable_path/lib/libgslcblas.0.dylib \
                      ${BUNDLE_LIB_DIR}/openmodeller/libcsm_bs.so
install_name_tool -change /usr/local/lib/libgsl.0.dylib \
                      @executable_path/lib/libgsl.0.dylib \
                      ${BUNDLE_LIB_DIR}/openmodeller/libcsm_bs.so
install_name_tool -change /usr/local/lib/libgslcblas.0.dylib \
                      @executable_path/lib/libgslcblas.0.dylib \
                      ${BUNDLE_LIB_DIR}/openmodeller/libenfa.so
install_name_tool -change /usr/local/lib/libgsl.0.dylib \
                      @executable_path/lib/libgsl.0.dylib \
                      ${BUNDLE_LIB_DIR}/openmodeller/libenfa.so
#
# Next sort out the qgis plugins....
#
LIBS="libogrprovider.so libcopyrightlabelplugin.so libnortharrowplugin.so libscalebarplugin.so"
for LIB in $LIBS
do
  install_name_tool -id @executable_path/lib/qgis/${LIB} ${BUNDLE_LIB_DIR}/qgis/${LIB}
  # for debugging only
  for LIBPATH in `otool -L ${BUNDLE_LIB_DIR}/qgis/${LIB} \
                  | sed 's/(\([a-zA-Z0-9\., ]*\))//g' \
                  | grep  $LIB_DIR \
                  | grep -v framework` #frameworks (in particular qt frameworks) get
                                       #dealt with in another script
  do 
    #echo "------------"
    #echo $LIBPATH 
    #echo "------------"
    BASELIB=`basename "$LIBPATH"`
    #echo $BASELIB
    install_name_tool -change ${LIBPATH} @executable_path/lib/${BASELIB} ${BUNDLE_LIB_DIR}/qgis/${LIB}
  done
  otool -L ${BUNDLE_LIB_DIR}/qgis/${LIB}
  echo "----------------------------------"
done

# Special case for libgeos
install_name_tool -change @executable_path/lib/libgeos_c.1.dylib \
                   @executable_path/lib/libgeos_c.dylib \
                   ${APP_PREFIX}/Contents/MacOS/lib/libqgis_core.1.1.dylib
install_name_tool -change @executable_path/lib/libgeos_c.1.dylib \
                   @executable_path/lib/libgeos_c.dylib \
                   ${APP_PREFIX}/Contents/MacOS/lib/libqgis_gui.1.1.dylib
#
# Add the bundle icon
#
mkdir -p ${APP_PREFIX}/Contents/Resources/
cp ../src/mac/Contents/Resources/openModellerDesktop.icns ${APP_PREFIX}/Contents/Resources/

#
# Strip binaries - disable for debugging
#
#pushd .
#cd ${APP_PREFIX}
#for FILE in `find . -name *.dylib`; do echo "Stripping $FILE"; strip -x $FILE;  done 
#for FILE in `find . -name *.so`; do echo "Stripping $FILE"; strip -x $FILE;  done
#strip -x ${APP_PREFIX}/Contents/MacOS/openModellerDesktop
#popd

#
# Install GDAL and Proj support files
# 
cp -r ${LIB_PREFIX}/share/proj /Applications/openModellerDesktop.app/Contents/MacOS/share/
cp -r ${LIB_PREFIX}/share/gdal/*.wkt /Applications/openModellerDesktop.app/Contents/MacOS/share/
cp -r ${LIB_PREFIX}/share/gdal/*.csv /Applications/openModellerDesktop.app/Contents/MacOS/share/
cp -r ${LIB_PREFIX}/share/gdal/*.dgn /Applications/openModellerDesktop.app/Contents/MacOS/share/

