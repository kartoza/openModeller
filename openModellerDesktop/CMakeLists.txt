PROJECT(openModellerDesktop1.1.0)
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(COMPLETE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
SET(PROJECT_VERSION ${COMPLETE_VERSION})
#############################################################
# CMake settings

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3)
IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
  cmake_policy(SET CMP0005 OLD)
ENDIF(COMMAND cmake_policy)

SET(CMAKE_COLOR_MAKEFILE ON)

# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_find_rules ${CMAKE_MODULE_PATH})

SET (WITH_TERRALIB FALSE CACHE BOOL "Determines whether to use terralib related code")
SET (WITH_QGIS     TRUE CACHE BOOL "Determines whether to use QGIS related code")
SET (WITH_OSSIMPLANET_QT FALSE CACHE BOOL "Determines whether to use ossimPlanet related code")
SET (WITH_EXPERIMENTAL FALSE CACHE BOOL "Determines whether to use experimental features in the code")
SET (WITH_WEBCATALOGUE FALSE CACHE BOOL "Determines whether to use webcataloge features in the code")
SET (WITH_QGIS_PLUGIN FALSE CACHE BOOL "Determines whether to build openModeller Desktop as a QGIS plugin")
SET (PEDANTIC FALSE CACHE BOOL "Determines whether treat all warnings as errors or not")
SET (ENABLE_TESTS FALSE CACHE BOOL "Build unit tests (use 'make test' to run them")
SET (WITH_DESIGNER FALSE CACHE BOOL "Determines whether to build designer plugins")

# #####################
# ossimPlanetQt Mac notes:
# download ossimPlanetQt from 
# http://ossim.telascience.org/ossimdata/MacOSX/ossimplanet.app-1.6.8-3.dmg
# Install to /Applications
#cp -R /Applications/ossimplanet.app/Contents/Frameworks/os* /Library/Frameworks/
#cp -R /Applications/ossimplanet.app/Contents/Frameworks/wms.framework* /Library/Frameworks/
#cp -R /Applications/ossimplanet.app/Contents/Frameworks/OpenThreads.framework /Library/Frameworks/

#############################################################
# Check if we should enable unit test builds
IF (ENABLE_TESTS)
  ENABLE_TESTING()
  SET (TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_data")
  #enable posting to dart server
  INCLUDE (CTest)
  INCLUDE (Dart)
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  # With the above enabled you can run tests and automatically
  # post to the dart server using 
  #
  #  make Experimental
  #
  # see also http://public.kitware.com/Dart/HTML/Client.shtml
ENDIF (ENABLE_TESTS)


#############################################################
# search for dependencies

# required
FIND_PACKAGE(GDAL REQUIRED)
FIND_PACKAGE(EXPAT REQUIRED)
#optional
IF (WITH_OSSIMPLANET_QT)
  FIND_PACKAGE(ossimPlanetQt REQUIRED)
  FIND_PACKAGE(ossimPlanet REQUIRED)
  FIND_PACKAGE(ossim REQUIRED)
  FIND_PACKAGE(osg REQUIRED)
  FIND_PACKAGE(osgGA REQUIRED)
  FIND_PACKAGE(osgUtil REQUIRED)
  FIND_PACKAGE(osgDB REQUIRED)
  FIND_PACKAGE(osgText REQUIRED)
  FIND_PACKAGE(OpenThreads REQUIRED)
  FIND_PACKAGE(WMS REQUIRED)
  ADD_DEFINITIONS(-DWITH_OSSIMPLANET_QT)
ENDIF (WITH_OSSIMPLANET_QT)
FIND_PACKAGE(openModeller)
FIND_PACKAGE(TerraLib)

IF (TERRALIB_FOUND AND WITH_TERRALIB)
  ADD_DEFINITIONS(-DWITH_TERRALIB)
ENDIF (TERRALIB_FOUND AND WITH_TERRALIB)

FIND_PACKAGE(QWT)
IF (QWT_FOUND)
  ADD_DEFINITIONS(-DWITH_QWT)
ENDIF (QWT_FOUND)

IF (NOT GDAL_FOUND)
  MESSAGE (SEND_ERROR "gdal dependency was not found!")
ENDIF (NOT GDAL_FOUND)
IF (NOT OM_FOUND)
  MESSAGE (SEND_ERROR "openModeller dependency was not found!")
ENDIF (NOT OM_FOUND)

IF (WITH_QGIS)
  FIND_PACKAGE(QGIS)
ENDIF (WITH_QGIS)
IF (QGIS_FOUND AND WITH_QGIS)
  ADD_DEFINITIONS(-DWITH_QGIS)
  FIND_PACKAGE(GEOS REQUIRED)
  FIND_PACKAGE(Proj REQUIRED)
ELSE (QGIS_FOUND AND WITH_QGIS)
  MESSAGE ("QGIS dependency was not found! - GIS integration will not be built")
ENDIF (QGIS_FOUND AND WITH_QGIS)

IF (NOT WITH_EXPERIMENTAL)
  ADD_DEFINITIONS(-DOMG_NO_EXPERIMENTAL)
ENDIF (NOT WITH_EXPERIMENTAL)

IF (WITH_WEBCATALOGUE)
  ADD_DEFINITIONS(-DOMG_WEBCATALOGUE)
  MESSAGE ("WEB CATALOGUE support will be built")
ENDIF (WITH_WEBCATALOGUE)

#############################################################
# search for Qt4

SET( QT_USE_QT3SUPPORT TRUE )
SET( QT_USE_QTXML      TRUE )
SET( QT_USE_QTNETWORK  TRUE )
SET( QT_USE_QTDESIGNER TRUE )
SET( QT_USE_QTSQL    TRUE )

#qtsvg is only needed when building with qgis support
IF (QGIS_FOUND AND WITH_QGIS)
  SET( QT_USE_QTSVG      TRUE )
ELSE (QGIS_FOUND AND WITH_QGIS)
  SET( QT_USE_QTSVG      FALSE )
ENDIF (QGIS_FOUND AND WITH_QGIS)

#if we are build with osgplanet support we need
#qt open gl lib too...
IF (WITH_OSSIMPLANET_QT)
  SET( QT_USE_QTOPENGL  TRUE)
ENDIF (WITH_OSSIMPLANET_QT)



FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE( ${QT_USE_FILE} )

FIND_PROGRAM(QT_LRELEASE_EXECUTABLE
    NAMES lrelease
    PATHS ${QT_BINARY_DIR}
    NO_DEFAULT_PATH
    )

#############################################################
# Enable the use of RPATH so that the executeable
# library search paths are relative to the executeable
#

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#############################################################
# enable warnings, halt compile on warnings
IF (PEDANTIC)
  ADD_DEFINITIONS( -Wall -Werror )
ENDIF (PEDANTIC)
IF (CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DOMG_NO_EXPERIMENTAL)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)


#############################################################
# platform specific stuff

IF (WIN32)

  SET (OMG_BIN_DIR    ${CMAKE_INSTALL_PREFIX})
  SET (OMG_LIB_DIR    ${CMAKE_INSTALL_PREFIX})
  SET (OMG_DATA_DIR   ${CMAKE_INSTALL_PREFIX})
  SET (OMG_PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/plugins")
  SET (OMG_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
  SET (OMG_SOURCE_DIR ${CMAKE_SOURCE_DIR})

  IF (MSVC)
    #tell msvc compiler to use main instead of winmain as the
    #application entry point
    SET(QT_USE_QTMAIN TRUE) 
	
	SET(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
	SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
	SET(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})
	
  ENDIF (MSVC)
  #  SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-runtime-pseudo-reloc")
  #SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-runtime-pseudo-reloc")
  
  # expect that classes are being imported by default
  # if they need to be exported, they will be exported
  # in the specific dir, e.g in lib
  # Note: MSVC doesn't like when the macros are quotes
  # and MSYS doesn't like them unqouted (bacause of braces)
  IF (MSVC)
    ADD_DEFINITIONS("-DOMG_LIB_EXPORT=__declspec(dllimport)")
    ADD_DEFINITIONS("-DOMG_PLUGIN_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("\"-DOMG_LIB_EXPORT=__declspec(dllimport)\"")
    ADD_DEFINITIONS("\"-DOMG_PLUGIN_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
  # dll exports brought over from openmodeller source tree
  IF (MSVC)
    ADD_DEFINITIONS("-Ddllexp=__declspec(dllimport)")
    ADD_DEFINITIONS("-DOM_ALG_DLL_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-Ddllexp\"=__declspec(dllimport)\"")
    ADD_DEFINITIONS("-DOM_ALG_DLL_EXPORT\"=__declspec(dllimport)\"")
  ENDIF (MSVC)
  # import qgis classes
  IF (MSVC)
    ADD_DEFINITIONS("-DGUI_EXPORT=__declspec(dllimport)")
    ADD_DEFINITIONS("-DCORE_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("\"-DGUI_EXPORT=__declspec(dllimport)\"")
    ADD_DEFINITIONS("\"-DCORE_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)

ELSE (WIN32)
  # On non windows we dont need all those ugly dllexport things... 
  ADD_DEFINITIONS(-DOMG_LIB_EXPORT=)
  ADD_DEFINITIONS(-DSOAP_EXPORT=)
  ADD_DEFINITIONS(-DOMG_PLUGIN_EXPORT=)
  ADD_DEFINITIONS(-Ddllexp=)
  ADD_DEFINITIONS(-DOM_ALG_DLL_EXPORT=)
  ADD_DEFINITIONS(-DGUI_EXPORT=)
  ADD_DEFINITIONS(-DCORE_EXPORT=)
    
  IF (APPLE)
    #this will define ${APP_SERVICES_LIBRARY}
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
    # for Mac OS X, everything is put inside an application bundle
    SET (CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/)
    # path for library references
    SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
    #set info.plist properties on mac
    SET( PROGNAME ${PROJECT_NAME})
    SET( MACOSX_BUNDLE_ICON_FILE openModellerDesktop.icns)
    SET( MACOSX_BUNDLE_SHORT_VERSION_STRING ${COMPLETE_VERSION} )
    SET( MACOSX_BUNDLE_VERSION ${COMPLETE_VERSION} )
    SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version ${COMPLETE_VERSION} )
    SET (OMG_LIB_DIR    ${CMAKE_INSTALL_PREFIX}/openModellerDesktop.app/Contents/MacOS/lib)
    SET (OMG_DATA_DIR   ${CMAKE_INSTALL_PREFIX}/openModellerDesktop.app/Contents/MacOS/share/openModellerDesktop)
    SET (OMG_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/openModellerDesktop.app/Contents/MacOS/lib/openModellerDesktop)
  ENDIF (APPLE)

  SET (OMG_BIN_DIR    ${CMAKE_INSTALL_PREFIX}/bin)
  SET (OMG_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/openModellerDesktop")
  SET (OMG_SOURCE_DIR ${CMAKE_SOURCE_DIR})
  IF (UNIX AND NOT APPLE)
    SET (OMG_LIB_DIR    ${CMAKE_INSTALL_PREFIX}/lib)
    SET (OMG_DATA_DIR   ${CMAKE_INSTALL_PREFIX}/share/openModellerDesktop)
    SET (OMG_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib/openModellerDesktop)
  ENDIF (UNIX AND NOT APPLE)

ENDIF (WIN32)
#Things applying to all platforms:
#where unit test binaries should be installed to and run from
SET (OMG_TEST_DIR   ${CMAKE_INSTALL_PREFIX}/tests)

#############################################################
# create omgconfig.h

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake_templates/omgconfig.h.in ${CMAKE_BINARY_DIR}/omgconfig.h)
INSTALL(FILES ${CMAKE_BINARY_DIR}/omgconfig.h DESTINATION ${OMG_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

#############################################################
# create qgssvnversion.h
# see http://public.kitware.com/pipermail/cmake/2006-July/010299.html

# We only do this if we are in a .svn dir

FIND_FILE(SVN_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.svn)

IF (NOT SVN_MARKER)
  SET (SVN_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
ENDIF (NOT SVN_MARKER)
# Add a custom command to drive the svn script whenever the svn entries
# file changes.
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/svnscript.cmake.in_cmake
               ${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake
	       @ONLY)

ADD_CUSTOM_COMMAND (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/omgsvnversion.h
  DEPENDS ${SVN_MARKER}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake
)

# Add a custom target to drive the custom command.
ADD_CUSTOM_TARGET(svnversion ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/omgsvnversion.h)



#############################################################
# process subdirs

SUBDIRS(src i18n) 


#############################################################
# install stuff

IF (ENABLE_TESTS)
  IF (WIN32)
    FILE (GLOB xmlfiles sample_data/*.xml)
    FILE (GLOB txtfiles sample_data/*.txt)
    INSTALL (FILES ${xmlfiles} ${txtfiles} DESTINATION ${CMAKE_INSTALL_PREFIX}/sample_data)
  ENDIF (WIN32)
ENDIF (ENABLE_TESTS)

#INSTALL (FILES AUTHORS SPONSORS
#         DESTINATION ${OMG_DATA_DIR}/doc)

# manual page... install also on windows?
#INSTALL (FILES qgis.man
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1)

############################################################
# Uninstall stuff
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

#############################################################
# Enable packaging

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "openModeller Desktop")
SET(CPACK_PACKAGE_VENDOR "Tim Sutton")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "openModellerDesktop ${COMPLETE_VERSION}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "openModeller Desktop")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\openmodeller.sf.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\openmodeller.sf.net")
  SET(CPACK_NSIS_CONTACT "tim@linfiniti.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_PACKAGE_FILE_NAME "openModellerDesktop-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_STRIP_FILES "")
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "openModellerDesktop-src-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.svn/;/install_win/;/install_linux/;/install_mac/;/debian/;.*~")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "openModellerDesktop" "openModeller Desktop")
INCLUDE(CPack)
