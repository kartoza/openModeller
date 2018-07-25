#!/bin/sh
# Copy Qt frameworks to openModellerDesktop bundle
# and make search paths for them relative to bundle

APP_PREFIX=/Applications/openModellerDesktop.app/Contents/MacOS
FRAMEWORKPREFIX=/Applications/openModellerDesktop.app/Contents/Frameworks
QTPREFIX=/usr/local
mkdir -p $APP_PREFIX
rm -rf $FRAMEWORKPREFIX
mkdir -p $FRAMEWORKPREFIX
cd $FRAMEWORKPREFIX

# Edit version when any library is upgraded
QTFRAMEWORKS="QtCore QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner QtScript"
#QtTest"

#
# Copy supporting frameworks to application bundle
#

cd $FRAMEWORKPREFIX
for FRAMEWORK in $QTFRAMEWORKS
do
	LIBFRAMEWORK=$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	if test ! -f $LIBFRAMEWORK; then
		mkdir -p $FRAMEWORK.framework/Versions/4
		cp $QTPREFIX/lib/$LIBFRAMEWORK $LIBFRAMEWORK
		install_name_tool -id @executable_path/../Frameworks/$LIBFRAMEWORK $LIBFRAMEWORK
	fi
done

# Update path to supporting frameworks
for FRAMEWORK in QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner QtScript 
#QtTest
do
	install_name_tool -change ${QTPREFIX}/lib/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

for FRAMEWORK in QtSvg Qt3Support QtDesigner
do
	install_name_tool -change ${QTPREFIX}/lib/QtGui.framework/Versions/4/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change ${QTPREFIX}/lib/QtXml.framework/Versions/4/QtXml \
		@executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

for FRAMEWORK in Qt3Support
do
	install_name_tool -change ${QTPREFIX}/lib/QtNetwork.framework/Versions/4/QtNetwork \
		@executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
	install_name_tool -change ${QTPREFIX}/lib/QtSql.framework/Versions/4/QtSql \
		@executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql \
		$FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

for FRAMEWORK in QtDesigner
do
        install_name_tool -change ${QTPREFIX}/lib/QtScript.framework/Versions/4/QtScript \
                @executable_path/../Frameworks/QtScript.framework/Versions/4/QtScript \
                $FRAMEWORK.framework/Versions/4/$FRAMEWORK
done

#
# Update openModeller related libs and binaries
#

cd $APP_PREFIX
for FILE in openModellerDesktop \
  lib/libomg_core.dylib \
  lib/openModellerDesktop/scraper/libomg_splink_plugin.dylib \
  lib/openModellerDesktop/scraper/libomg_gbifrest_plugin.dylib \
  lib/openModellerDesktop/modeller/libomg_localmodellerplugin.dylib \
  lib/openModellerDesktop/modeller/libomg_soapmodellerplugin.dylib \
  lib/libqgis_core.0.10.dylib \
  lib/libqgis_gui.0.10.dylib \
  lib/qgis/libogrprovider.so \
  lib/qgis/libnortharrowplugin.so \
  lib/qgis/libcopyrightlabelplugin.so \
  lib/qgis/libscalebarplugin.so \
  lib/libomg_widgets.dylib
  #lib/libqwt.dylib
do
	for FRAMEWORK in QtCore QtGui QtNetwork QtSql QtSvg QtXml Qt3Support QtDesigner 
#QtTest
	do
		install_name_tool -change ${QTPREFIX}/lib/$FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			@executable_path/../Frameworks/$FRAMEWORK.framework/Versions/4/$FRAMEWORK \
			$APP_PREFIX/$FILE
	done
done



#
# Update qt imageformat plugin paths
#


cd ${APP_PREFIX}/../
mkdir -p plugins/imageformats
cd plugins/imageformats
LIBJPEG=libqjpeg.dylib
LIBQJPEG=${QTPREFIX}/plugins/imageformats/libqjpeg.dylib
cp $LIBQJPEG $LIBJPEG
install_name_tool -change $LIBQJPEG @executable_path/../plugins/imageformats/$LIBJPEG $LIBJPEG
# Update path to supporting libraries
for FRAMEWORK in QtGui QtCore QtSvg Qt3Support 
do
  install_name_tool -change ${QTPREFIX}/lib/${FRAMEWORK}.framework/Versions/4/${FRAMEWORK} \
    @executable_path/../Frameworks/${FRAMEWORK}.framework/Versions/4/${FRAMEWORK} \
    $LIBJPEG
done
LIBGIF=libqgif.dylib
LIBQGIF=${QTPREFIX}/plugins/imageformats/libqgif.dylib
cp $LIBQGIF $LIBGIF
install_name_tool -change $LIBQGIF @executable_path/../plugins/imageformats/$LIBGIF $LIBGIF
# Update path to supporting libraries
for FRAMEWORK in QtGui QtCore QtSvg Qt3Support 
do
	install_name_tool -change ${QTPREFIX}/lib/${FRAMEWORK}.framework/Versions/4/${FRAMEWORK} \
    @executable_path/../Frameworks/${FRAMEWORK}.framework/Versions/4/${FRAMEWORK} \
    $LIBGIF
done
strip -x ${FRAMEWORKPREFIX}/QtGui.framework/Versions/4/QtGui 
strip -x ${FRAMEWORKPREFIX}/QtCore.framework/Versions/4/QtCore 
strip -x ${FRAMEWORKPREFIX}/Qt3Support.framework/Versions/4/Qt3Support 
strip -x ${FRAMEWORKPREFIX}/QtSql.framework/Versions/4/QtSql 
strip -x ${FRAMEWORKPREFIX}/QtSvg.framework/Versions/4/QtSvg 
strip -x ${FRAMEWORKPREFIX}/QtXml.framework/Versions/4/QtXml 
strip -x ${FRAMEWORKPREFIX}/QtNetwork.framework/Versions/4/QtNetwork 
strip -x ${FRAMEWORKPREFIX}/QtDesigner.framework/Versions/4/QtDesigner
