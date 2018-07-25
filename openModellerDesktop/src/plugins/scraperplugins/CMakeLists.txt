#############################################################
# sources

#SET(OMG_GBIF_PLUGIN_SRCS
#  omgscrapergbif.cpp 
#  )

#SET(OMG_GBIF_PLUGIN_MOC_HDRS
#  omgscrapergbif.h 
#  )

SET(OMG_GBIFREST_PLUGIN_SRCS
  omgscrapergbifrest.cpp 
  )

SET(OMG_GBIFREST_PLUGIN_MOC_HDRS
  omgscrapergbifrest.h 
  )

SET(OMG_SPLINK_PLUGIN_SRCS
  omgscrapersplink.cpp 
  )

SET(OMG_SPLINK_PLUGIN_MOC_HDRS
  omgscrapersplink.h 
  )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  ${QT_QTSVG_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/
  ${CMAKE_CURRENT_BINARY_DIR}/../../lib/
)

#QT4_WRAP_CPP(OMG_GBIF_PLUGIN_MOC_SRCS ${OMG_GBIF_PLUGIN_MOC_HDRS})
QT4_WRAP_CPP(OMG_GBIFREST_PLUGIN_MOC_SRCS ${OMG_GBIFREST_PLUGIN_MOC_HDRS})
QT4_WRAP_CPP(OMG_SPLINK_PLUGIN_MOC_SRCS ${OMG_SPLINK_PLUGIN_MOC_HDRS})

SET(scraper_RCCS 
    splink_resources.qrc
    gbif_resources.qrc
    )

# tell cmake to generate resources
QT4_ADD_RESOURCES(scraper_RCC_SRCS ${scraper_RCCS})

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

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)


#############################################################
# library

#ADD_LIBRARY(omg_gbif_plugin SHARED 
#            ${OMG_GBIF_PLUGIN_SRCS} 
#            ${OMG_GBIF_PLUGIN_MOC_SRCS} 
#            ${scraper_RCC_SRCS}
#            )
ADD_LIBRARY(omg_gbifrest_plugin SHARED 
           ${OMG_GBIFREST_PLUGIN_SRCS} 
           ${OMG_GBIFREST_PLUGIN_MOC_SRCS} 
           ${scraper_RCC_SRCS}
           )
ADD_LIBRARY(omg_splink_plugin SHARED 
           ${OMG_SPLINK_PLUGIN_SRCS} 
           ${OMG_SPLINK_PLUGIN_MOC_SRCS} 
           ${scraper_RCC_SRCS}
           )

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

#TARGET_LINK_LIBRARIES(omg_gbif_plugin
#  ${QT_LIBRARIES}
#  omg_core
#)

TARGET_LINK_LIBRARIES(omg_gbifrest_plugin
  ${QT_LIBRARIES}
  omg_core
)

TARGET_LINK_LIBRARIES(omg_splink_plugin
  ${QT_LIBRARIES}
  omg_core
)

#INSTALL(TARGETS omg_gbif_plugin
#  RUNTIME DESTINATION ${OMG_PLUGIN_DIR}/scraper
#  LIBRARY DESTINATION ${OMG_PLUGIN_DIR}/scraper)

INSTALL(TARGETS omg_gbifrest_plugin
  RUNTIME DESTINATION ${OMG_PLUGIN_DIR}/scraper
  LIBRARY DESTINATION ${OMG_PLUGIN_DIR}/scraper)

INSTALL(TARGETS omg_splink_plugin
  RUNTIME DESTINATION ${OMG_PLUGIN_DIR}/scraper
  LIBRARY DESTINATION ${OMG_PLUGIN_DIR}/scraper)

# Added by Tim to install headers

#INSTALL(CODE "MESSAGE(\"Installing GBIF_PLUGIN headers...\")")
#INSTALL(FILES ${omg_gbif_plugin_HDRS} ${OMG_GBIF_PLUGIN_MOC_HDRS} DESTINATION ${OMG_INCLUDE_DIR})
