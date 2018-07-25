#############################################################
# sources

SET(OMG_CORE_SRCS
  omgalgorithm.cpp
  omgalgorithmparameter.cpp
  omgalgorithmset.cpp
  omgclimatedataprocessor.cpp
  omgclimatedataprocessorcontroller.cpp
  omgclimatefilegroup.cpp
  omgclimatefilereader.cpp
  omgclimateimporter.cpp
  omgdataseries.cpp
  omgexperiment.cpp
  omgexperimentprinter.cpp
  omgfilewriter.cpp
  omggdal.cpp
  omggradientfactory.cpp
  omggraph.cpp
  omgguid.cpp
  omglayer.cpp
  omglayerset.cpp
  omglocalitiesmodel.cpp
  omglocality.cpp
  omgmeridianswitcher.cpp
  omgmodel.cpp
  omgmodeltest.cpp
  omgmodellerplugininterface.cpp
  omgmodellerpluginregistry.cpp
  omgpluginmessenger.cpp
  omgpreanalysis.cpp
  omgpreanalysisalgorithm.cpp
  omgpreanalysisalgorithmset.cpp
  omgprojection.cpp
  omgrandom.cpp
  omgscraperplugininterface.cpp
  omgscraperpluginregistry.cpp
  omgserialisable.cpp
  omgspatialreferencesystem.cpp
  omgtip.h
  omgtipfactory.cpp
  omgui.cpp
  omgwebpagefetcher.cpp
  )
IF (TERRALIB_FOUND AND WITH_TERRALIB)
  SET(OMG_CORE_SRCS
     omgterralib.cpp
     )
ENDIF (TERRALIB_FOUND AND WITH_TERRALIB)

SET(OMG_CORE_MOC_HDRS
  omgclimatedataprocessorcontroller.h
  omgclimatefilegroup.h
  omgclimatefilereader.h
  omgclimateimporter.h
  omgexperiment.h
  omgexperimentprinter.h
  omggdal.h
  omgmeridianswitcher.h
  omgmodel.h
  #omgmodellerplugininterface.h
  omgmodellerpluginregistry.h
  omgpluginmessenger.h
  omgpreanalysis.h
  #omgscraperplugininterface.h
  omgscraperpluginregistry.h
  omgtipfactory.h
  omgwebpagefetcher.h
  )


QT4_WRAP_CPP(OMG_CORE_MOC_SRCS ${OMG_CORE_MOC_HDRS})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${QT_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${QGIS_INCLUDE_DIR}
  ${OM_INCLUDE_DIR}
)
IF (QWT_FOUND) 
  INCLUDE_DIRECTORIES(
  ${QWT_INCLUDE_DIR}
  )
ENDIF (QWT_FOUND)
IF (TERRALIB_FOUND AND WITH_TERRALIB)
  INCLUDE_DIRECTORIES(
  ${TERRALIB_INCLUDE_DIR}
     )
ENDIF (TERRALIB_FOUND AND WITH_TERRALIB)

#############################################################
# library


ADD_LIBRARY(omg_core SHARED ${OMG_CORE_SRCS} ${OMG_CORE_MOC_SRCS} )

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
  IF (MSVC)
    #needed for linking to gdal which needs odbc
    SET(PLATFORM_LIBRARIES ${PLATFORM_LIBRARIES} odbc32 odbccp32)
  ENDIF (MSVC)
ENDIF (WIN32)

TARGET_LINK_LIBRARIES(omg_core
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTCORE_LIBRARY} 
  ${QT_QTNETWORK_LIBRARY} 
  ${QT_QTXML_LIBRARY} 
  ${QT_QTSQL_LIBRARY} 
  ${PROJ_LIBRARY}
  ${GEOS_LIBRARY}
  ${GDAL_LIBRARY}
  ${QGIS_CORE_LIBRARY}
  ${QGIS_GUI_LIBRARY}
  ${PLATFORM_LIBRARIES}
  ${OM_LIBRARY}
)
IF (QWT_FOUND)
  TARGET_LINK_LIBRARIES(
    omg_core
    ${QWT_LIBRARY}
  )
ENDIF (QWT_FOUND)

IF (TERRALIB_FOUND AND WITH_TERRALIB)
  TARGET_LINK_LIBRARIES(
    omg_core
    ${TERRALIB_LIBRARY}
  )
ENDIF (TERRALIB_FOUND AND WITH_TERRALIB)

IF (APPLE)
   TARGET_LINK_LIBRARIES(
     omg_core
     ${APP_SERVICES_LIBRARY}
   )
ENDIF (APPLE)

# --------------------------------------

IF (APPLE)
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
  INSTALL(TARGETS omg_core 
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    LIBRARY DESTINATION ${OMG_LIB_DIR})
ELSE (APPLE)
  INSTALL(TARGETS omg_core
    RUNTIME DESTINATION ${OMG_LIB_DIR}
    LIBRARY DESTINATION ${OMG_LIB_DIR}
    ARCHIVE DESTINATION ${OMG_LIB_DIR})
ENDIF (APPLE)

IF (WIN32)
  # expect that classes are being EXPORTED for the lib
  IF (MSVC)
    ADD_DEFINITIONS("-DOMG_LIB_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UOMG_LIB_EXPORT \"-DOMG_LIB_EXPORT=__declspec(dllexport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

# Added by Tim to install headers

#INSTALL(CODE "MESSAGE(\"Installing CORE headers...\")")
#INSTALL(FILES ${OMG_CORE_HDRS} ${OMG_CORE_MOC_HDRS} DESTINATION ${OMG_INCLUDE_DIR})
INSTALL(CODE "MESSAGE(\"Installing CORE lib from ${CMAKE_CFG_INTDIR}...\")")

