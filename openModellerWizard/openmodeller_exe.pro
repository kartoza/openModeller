####################################################################
# Qmake project file for QGIS plugin
# This file is used by qmake to generate the Makefiles for building
# the QGIS copyright plugin on Windows
# $Id: openmodeller_exe.pro 1669 2006-05-04 15:21:53Z rdg $
####################################################################

TEMPLATE = app

# config for standalone mode
CONFIG += qt rtti thread release
win32 {
#CONFIG += qt rtti thread debug console

# place release temp build objects somewhere else to avoid conflict with debug build
OBJECTS_DIR = build\tmp

# for icon file under windows
# see http://www.qiliang.net/qt/appicon.html
#1
RC_FILE = omguistandalone.rc

DEFINES+=WIN32
DEFINES+=_WINDOWS
DEFINES+=CORE_DLL_IMPORT 
}


#inc path for standalone app:
INCLUDEPATH += . 
INCLUDEPATH += $(GDAL_HOME)\include
INCLUDEPATH += $(OM_HOME)\src 
INCLUDEPATH += $(OM_HOME)\src\openmodeller
INCLUDEPATH += $(OM_HOME)\console

#libs for standalone mode
LIBS += $(GDAL_HOME)\lib\gdal_i.lib

win32 {
  contains( CONFIG, debug ) {
    DESTDIR = build_debug
    LIBS += $(OM_HOME)\windows\vc7\build_debug\libopenmodeller_debug.lib   
    TARGET = omgui_debug
  } else {  
    DESTDIR = build
    LIBS += $(OM_HOME)\windows\vc7\build\libopenmodeller.lib   
    TARGET = omgui 
  }
}
macx {
    LIBS += ${QTDIR}/libqt-mt.3.dylib
    TARGET = omgui 
    frameworks.path = omgui.app/Contents/Frameworks
    frameworks.files = ${QTDIR}/libqt-mt.3.dylib
    INSTALLS += frameworks
}

#used by both plugin and exe
HEADERS += occurrences_file.hh \
           openmodellergui.h \
           openmodellerguibase.ui.h \
           omguimain.h \
           omguimainbase.ui.h \
	   omguiabout.h \
           imagewriter.h \
           layerselector.h \
           request_file.hh \
           file_parser.hh \
	   omggdal.h

INTERFACES += openmodellerguibase.ui omguimainbase.ui layerselectorbase.ui omguireportbase.ui omguiaboutbase.ui

SOURCES += main.cpp \ 
           occurrences_file.cpp \
           openmodellergui.cpp \
           omguimain.cpp \
	   omguiabout.cpp \
           imagewriter.cpp \
           layerselector.cpp \
           request_file.cpp \
           file_parser.cpp \
	   omggdal.cpp
win32 {

# -------------------------------------------
# check for Expat lib files
# -------------------------------------------
!exists( $$(EXPAT_HOME)\Libs\libexpat.lib ) {  
    message( "Could not find Expat library file." )  
    message( "Check whether the EXPAT_HOME environment variable is set correctly. ")  
    message( "Current value: EXPAT_HOME=$$(EXPAT_HOME)" )  
    error  ( "Expat library file is missing." )
}

# -------------------------------------------
# check for GDAL include and lib files
# -------------------------------------------
!exists( $$(GDAL_HOME)\include\gdal.h ) {
  message( "Could not find GDAL include files." )
  message( "Check whether the GDAL environment variable is set correctly. ")
  message( "Current value: GDAL=$$(GDAL)" )
  error  ( "GDAL include files are missing." )
}

!exists( $$(GDAL_HOME)\lib\gdal_i.lib ) {
  message( "Could not find GDAL library file." )
  message( "Check whether the GDAL environment variable is set correctly. ")
  message( "Current value: GDAL=$$(GDAL)" )
  error  ( "GDAL library file is missing." )
}

# -------------------------------------------
# check for OM include and lib files
# -------------------------------------------
!exists( $$(OM_HOME)\src\openmodeller\OpenModeller.hh ) {
  message( "Could not find OpenModeller include files." )
  message( "Check whether the OM_HOME env. variable is set correctly. ")
  message( "Current value: OM_HOME=$$(OM_HOME)" )
  error  ( "OpenModeller include files are missing." )
}

!exists( $$(OM_HOME)\windows\vc7\build\libopenmodeller.lib ) {
  message( "Could not find OpenModeller library file." )
  message( "Check whether the OM_HOME env. variable is set correctly. ")
  message( "Current value: OM_HOME=$$(OM_HOME)" )
  error  ( "OpenModeller library file is missing." )
}
}
# -------------------------------------------
