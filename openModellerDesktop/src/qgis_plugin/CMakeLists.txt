
########################################################
# Files

SET (qgisomplugin_SRCS
     qgisomplugin.cpp
     omgqgisexperimentdesigner.cpp
     omgexperimentprogress.cpp
     omgqgispluginoptions.cpp 
     omgreportdialog.cpp
     ../gui/omgabout.cpp
     ../gui/omgtermsandconditions.cpp 
     ../gui/omgexperimentselector.cpp
     ../gui/omgalgorithmmanager.cpp
     ../gui/omglayersetmanager.cpp
     ../gui/omglayerselector.cpp
     ../gui/omglocalitiesoptions.cpp
     ../gui/omgsupportedfileformats.cpp
     ../gui/omgtextfilesplitter.cpp 
     ../gui/omgrasterthreshold.cpp 
     ../gui/omgexperimentdesigner.cpp
     ../gui/omgmodeltestoptions.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizard.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardlicensepage.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardlicensewidget.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardprogresspage.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardprogresswidget.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardsearchpage.cpp
     ../gui/dataFetcherWizard/omgdatafetcherwizardsearchwidget.cpp
     omgwizard.cpp
     omgwizardpages.cpp
)

SET (qgisomplugin_MOC_HDRS
     qgisomplugin.h
     omgqgisexperimentdesigner.h
     omgexperimentprogress.h
     omgqgispluginoptions.h
     omgreportdialog.h
     ../gui/omgabout.h
     ../gui/omgtermsandconditions.h
     ../gui/omgexperimentselector.h
     ../gui/omgalgorithmmanager.h
     ../gui/omglayersetmanager.h
     ../gui/omglayerselector.h
     ../gui/omgsupportedfileformats.h
     ../gui/omgtextfilesplitter.h
     ../gui/omgrasterthreshold.h
     ../gui/omgexperimentdesigner.h
     ../gui/omglocalitiesoptions.h
     ../gui/omgmodeltestoptions.h
     ../gui/dataFetcherWizard/omgdatafetcherwizard.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardlicensepage.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardlicensewidget.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardprogresspage.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardprogresswidget.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardsearchpage.h
     ../gui/dataFetcherWizard/omgdatafetcherwizardsearchwidget.h
     omgwizard.h
     omgwizardpages.h
)

SET (qgisomplugin_RCCS  
    ../resources/resources.qrc
    )

SET (qgisomplugin_UIS
  omgwizardbase.ui
  omgexperimentpagebase.ui
  omgoccurrencespagebase.ui
  omgfieldspagebase.ui
  omgtaxapagebase.ui
  omgalgorithmpagebase.ui
  omglayersetpagebase.ui
  omgprojectionpagebase.ui
  omgqgispluginoptionsbase.ui
  omgreportdialogbase.ui
  )

########################################################
# Build

QT4_WRAP_UI (qgisomplugin_UIS_H  ${qgisomplugin_UIS})

QT4_WRAP_CPP (qgisomplugin_MOC_SRCS  ${qgisomplugin_MOC_HDRS})

QT4_ADD_RESOURCES(qgisomplugin_RCC_SRCS ${qgisomplugin_RCCS})

ADD_LIBRARY (qgisomplugin MODULE 
             ${qgisomplugin_SRCS} 
             ${qgisomplugin_MOC_SRCS} 
             ${qgisomplugin_RCC_SRCS} 
             ${openModellerDesktop_UIS_H} 
             ${qgisomplugin_UIS_H}
             )

INCLUDE_DIRECTORIES(
     #binary dirs needed for mocs / uis
     ${CMAKE_CURRENT_BINARY_DIR} 
     ${CMAKE_CURRENT_BINARY_DIR}/../gui
     ${CMAKE_CURRENT_BINARY_DIR}/../lib
     ${CMAKE_CURRENT_BINARY_DIR}/../ui
     #source dirs
     ${CMAKE_CURRENT_SOURCE_DIR}/../../
     ${CMAKE_CURRENT_SOURCE_DIR}/../lib
     ${CMAKE_CURRENT_SOURCE_DIR}/../gui
     ${CMAKE_CURRENT_SOURCE_DIR}/../widgets
     ${CMAKE_CURRENT_SOURCE_DIR}/../gui/dataFetcherWizard
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${QGIS_INCLUDE_DIR}
     ${GDAL_INCLUDE_DIR}
     ${GEOS_INCLUDE_DIR}
     ${OM_INCLUDE_DIR}
     ..
)

TARGET_LINK_LIBRARIES(qgisomplugin
  omg_core
  omg_widgets
  ${QGIS_CORE_LIBRARY}
  ${QGIS_GUI_LIBRARY}
  ${OM_LIBRARY}
)

IF (WIN32)
  # expect that classes are being EXPORTED for the lib
  IF (MSVC)
    ADD_DEFINITIONS("-DOMG_LIB_EXPORT=__declspec(dllimport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UOMG_LIB_EXPORT \"-DOMG_LIB_EXPORT=__declspec(dllimport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

########################################################
# Install

INSTALL(TARGETS qgisomplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})

