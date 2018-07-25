#############################################################
# sources

SET(OMG_LOCALMODELLERPLUGIN_SRCS
    omgmodellerlocalplugin.cpp
    omgomlogcallback.cpp
   )

SET(OMG_LOCALMODELLERPLUGIN_MOC_HDRS
    omgmodellerlocalplugin.h
    omgomlogcallback.h
   )

SET(OMG_SOAPMODELLERPLUGIN_SRCS
    omgmodellerwebservicesplugin.cpp 
    #soapH.h
    #soapStub.h
    stdsoap2.cpp
    soapC.cpp
    soapClient.cpp
   )

SET(OMG_SOAPMODELLERPLUGIN_MOC_HDRS
    omgmodellerwebservicesplugin.h 
   )


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  ${QT_QTSVG_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
  ${OM_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/
  ${CMAKE_CURRENT_BINARY_DIR}/../../lib/
  ${PROJ_INCLUDE_DIR}
)
QT4_WRAP_CPP(OMG_LOCALMODELLERPLUGIN_MOC_SRCS ${OMG_LOCALMODELLERPLUGIN_MOC_HDRS})

QT4_WRAP_CPP(OMG_SOAPMODELLERPLUGIN_MOC_SRCS ${OMG_SOAPMODELLERPLUGIN_MOC_HDRS})

IF (MSVC)
  #under msvc80 it will try to use its own internal
  #wchar representation but for the web services plugin
  #we need to use the qt wchar implementation so this
  #next line tries to turn it off. You can do this manually
  #too in MSVC express by clicking on teh omg_soapmodellerplugin
  #project, then from the top menu go:
  # Project -> Properties -> Configuration Properties ->
  # C/C++ -> Language -> Treat wchar_t as Built-in type
  # and change it to No (/Zc:wchar_t)
  IF (MSVC80)
    ADD_DEFINITIONS("/Zc:wchar_t")
  ELSE (MSVC80)
    ADD_DEFINITIONS("/Zc:wchar_t- /Zm300")
  ENDIF (MSVC80)
ENDIF (MSVC)

IF (WIN32)
  # Note: MSVC doesn't like when the macros are quotes
  # and MSYS doesn't like them unqouted (bacause of braces)
  # for plugins they need to IMPORT all external symbols
  # (see top level CMakeLists.txt)
  # and EXPORT their own symbols
  IF (MSVC)
    ADD_DEFINITIONS("-DOMG_PLUGIN_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UOMG_PLUGIN_EXPORT \"-DOMG_PLUGIN_EXPORT=__declspec(dllexport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

#############################################################
# library

# These libs are qt plugins so we need to make sure qt knows this:

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

REMOVE_DEFINITIONS( -Wall -Werror )

ADD_LIBRARY(omg_localmodellerplugin SHARED ${OMG_LOCALMODELLERPLUGIN_SRCS} ${OMG_LOCALMODELLERPLUGIN_MOC_SRCS} )
#IF (NOT WIN32)
ADD_LIBRARY(omg_soapmodellerplugin SHARED ${OMG_SOAPMODELLERPLUGIN_SRCS} ${OMG_SOAPMODELLERPLUGIN_MOC_SRCS} )
#ENDIF (NOT WIN32)

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(omg_localmodellerplugin
  ${QT_QTCORE_LIBRARY} 
  ${GDAL_LIBRARY}
  ${PROJ_LIBRARY}
  ${EXPAT_LIBRARY}
  ${GEOS_LIBRARY}
  omg_core
  ${OM_LIBRARY}
)

TARGET_LINK_LIBRARIES(omg_soapmodellerplugin
  ${QT_QTCORE_LIBRARY} 
  ${GDAL_LIBRARY}
  omg_core
  #omg_soap
)

INSTALL(TARGETS omg_localmodellerplugin
  RUNTIME DESTINATION ${OMG_PLUGIN_DIR}/modeller
  LIBRARY DESTINATION ${OMG_PLUGIN_DIR}/modeller)

INSTALL(TARGETS omg_soapmodellerplugin
  RUNTIME DESTINATION ${OMG_PLUGIN_DIR}/modeller
  LIBRARY DESTINATION ${OMG_PLUGIN_DIR}/modeller)

# Added by Tim to install headers

#INSTALL(CODE "MESSAGE(\"Installing LOCALMODELLERPLUGIN headers...\")")
#INSTALL(FILES ${OMG_LOCALMODELLERPLUGIN_HDRS} ${OMG_LOCALMODELLERPLUGIN_MOC_HDRS} DESTINATION ${OMG_INCLUDE_DIR})
