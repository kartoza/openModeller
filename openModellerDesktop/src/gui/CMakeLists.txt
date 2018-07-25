#############################################################
# sources

SET(openModellerDesktop_SRCS
  dataFetcherWizard/omgdatafetcherwizard.cpp
  dataFetcherWizard/omgdatafetcherwizardlicensepage.cpp
  dataFetcherWizard/omgdatafetcherwizardlicensewidget.cpp
  dataFetcherWizard/omgdatafetcherwizardprogresspage.cpp
  dataFetcherWizard/omgdatafetcherwizardprogresswidget.cpp
  dataFetcherWizard/omgdatafetcherwizardsearchpage.cpp
  dataFetcherWizard/omgdatafetcherwizardsearchwidget.cpp
  main.cpp 
  omgabout.cpp 
  omgalgorithmmanager.cpp 
  omgclimateconverter.cpp 
  omgclimatedataprocessorgui.cpp 
  omgexperimentdesigner.cpp 
  omgexperimentselector.cpp 
  omggdalconverter.cpp 
  omggdalrastercontour.cpp 
  omggdalwarp.cpp 
  omglayerselector.cpp 
  omglayersetmanager.cpp 
  omglocalitiesoptions.cpp
  omgmainwindow.cpp
  omgmodeltestoptions.cpp  
  omgmodeltests.cpp 
  omgoptions.cpp 
  omgpreanalysisalgorithmmanager.cpp
  omgpublishtocatalogue.cpp
  omgrasterthreshold.cpp 
  omgsupportedfileformats.cpp 
  omgsurveywizard.h
  omgtermsandconditions.cpp 
  omgtextfilesplitter.cpp 
  qgisappinterface.cpp
  qgspluginmetadata.cpp
  qgspluginregistry.cpp
  #this is a copy over from QGIS sources
  
  )

SET(openModellerDesktop_MOC_HDRS
  dataFetcherWizard/omgdatafetcherwizard.h
  dataFetcherWizard/omgdatafetcherwizardlicensepage.h
  dataFetcherWizard/omgdatafetcherwizardlicensewidget.h
  dataFetcherWizard/omgdatafetcherwizardprogresspage.h
  dataFetcherWizard/omgdatafetcherwizardprogresswidget.h
  dataFetcherWizard/omgdatafetcherwizardsearchpage.h
  dataFetcherWizard/omgdatafetcherwizardsearchwidget.h
  omgabout.h
  omgalgorithmmanager.h 
  omgclimateconverter.h 
  omgclimatedataprocessorgui.h 
  omgexperimentdesigner.h 
  omgexperimentselector.h 
  omggdalconverter.h 
  omggdalrastercontour.h 
  omggdalwarp.h 
  omglayerselector.h 
  omglayersetmanager.h 
  omglocalitiesoptions.h
  omgmainwindow.h
  omgmodeltestoptions.h
  omgmodeltests.h
  omgoptions.h 
  omgpreanalysisalgorithmmanager.h
  omgpublishtocatalogue.h
  omgrasterthreshold.h 
  omgsupportedfileformats.h 
  omgsurveywizard.h
  omgtermsandconditions.h 
  omgtextfilesplitter.h 
  qgisappinterface.h
  )


SET(openModellerDesktop_RCCS 
    ../resources/resources.qrc
    )

QT4_WRAP_CPP(openModellerDesktop_MOC_SRCS ${openModellerDesktop_MOC_HDRS})

# tell cmake to generate resources
QT4_ADD_RESOURCES(openModellerDesktop_RCC_SRCS ${openModellerDesktop_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/dataFetcherWizard
  ${CMAKE_CURRENT_BINARY_DIR}/../../
  ${CMAKE_CURRENT_BINARY_DIR}/../ui
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/dataFetcherWizard
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../widgets
  ${GDAL_INCLUDE_DIR}
  # Temporarily hard coded
  /usr/local/include/ossimPlanetQt
  #${OSSIMPLANET_QT_INCLUDE_DIR}
  #${OSG_INCLUDE_DIR}
  #${OSGGA_INCLUDE_DIR}
  ${QGIS_INCLUDE_DIR}
  ${PROJ_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  ${OM_INCLUDE_DIR}
  )

#############################################################
# application icon
IF (WIN32)
  IF( MINGW )
    # resource compilation for MinGW
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icon.o
      COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/openModellerDesktop.rc 
      -o ${CMAKE_CURRENT_BINARY_DIR}/icon.o )
    SET(openModellerDesktop_SRCS ${openModellerDesktop_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/icon.o)
  ELSE( MINGW )
    SET(openModellerDesktop_SRCS ${openModellerDesktop_SRCS} openModellerDesktop.rc)
  ENDIF( MINGW )
  #todo add msvc icon support
ENDIF (WIN32)
#############################################################
# library


# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

#Now add these generated files to the ADD_EXECUTABLE step
# If this is NOT done, then the ui_*.h files will not be generated
# ditto for moc, rc etc files
# create an executable file named "openModellerDesktop" from the source 
# files in the variable "openModellerDesktop_SRCS".
IF( APPLE )
  add_executable(openModellerDesktop 
               MACOSX_BUNDLE
               ${openModellerDesktop_SRCS} 
               ${openModellerDesktop_UIS_H} 
               ${openModellerDesktop_MOC_SRCS}
               ${openModellerDesktop_RCC_SRCS}
               )
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  INSTALL(TARGETS openModellerDesktop RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ELSE( NOT APPLE )

  # Remove console if on WIN32
 IF (WIN32)
      add_executable(openModellerDesktop
	       WIN32
               ${openModellerDesktop_SRCS} 
               ${openModellerDesktop_UIS_H} 
               ${openModellerDesktop_MOC_SRCS}
               ${openModellerDesktop_RCC_SRCS}
               )
  ELSE (WIN32)
      add_executable(openModellerDesktop 
               ${openModellerDesktop_SRCS} 
               ${openModellerDesktop_UIS_H} 
               ${openModellerDesktop_MOC_SRCS}
               ${openModellerDesktop_RCC_SRCS}
               )
  ENDIF (WIN32)
  
ENDIF( APPLE )


# link the "openModellerDesktop" target against the Qt libraries. which libraries 
# exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
SET( QT_USE_QTGUI TRUE )
TARGET_LINK_LIBRARIES(openModellerDesktop
    omg_core
    omg_widgets
    ${QT_LIBRARIES}
    ${QGIS_CORE_LIBRARY}
    ${QGIS_GUI_LIBRARY}
    ${PROJ_LIBRARY}
    ${GEOS_LIBRARY}
    ${GDAL_LIBRARY}
    ${OM_LIBRARY}
)
IF (QWT_FOUND)
  TARGET_LINK_LIBRARIES(
    openModellerDesktop
    ${QWT_LIBRARY}
    )
ENDIF (QWT_FOUND)

IF (OSSIMPLANET_QT_FOUND AND WITH_OSSIMPLANET_QT)
  TARGET_LINK_LIBRARIES(openModellerDesktop
    # Temporarily hard coded
    ossimPlanetQt
    #${OSSIMPLANET_QT_LIBRARY}
    #${OSSIMPLANET_LIBRARY}
    #${OSSIM_LIBRARY}
    #${OSG_LIBRARY}
    #${OSGGA_LIBRARY}
    #${OSGUTIL_LIBRARY}
    #${OSGDB_LIBRARY}
    #${OSGTEXT_LIBRARY}
    #${OPENTHREADS_LIBRARY}
    #${WMS_LIBRARY}
    #${OSG_LIBRARY}
    #${OSGGA_LIBRARY}
  )
ENDIF (OSSIMPLANET_QT_FOUND AND WITH_OSSIMPLANET_QT)

IF (MSVC)
  # Very important or you get all kinds of odd side
  #effects like app crash on start up saying qtgui.dll
  TARGET_LINK_LIBRARIES(openModellerDesktop qtmain)
ENDIF (MSVC)

# 
# Install
#

IF (APPLE)
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  #SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
  
  INSTALL(TARGETS openModellerDesktop RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ELSE (APPLE)
  INSTALL(TARGETS openModellerDesktop RUNTIME DESTINATION ${OMG_BIN_DIR})
ENDIF (APPLE)
  # Added by Tim to install headers

  #INSTALL(CODE "MESSAGE(\"Installing GUI headers...\")")
  #INSTALL(FILES ${openModellerDesktop_HDRS} ${openModellerDesktop_MOC_HDRS} DESTINATION ${OMG_INCLUDE_DIR})
