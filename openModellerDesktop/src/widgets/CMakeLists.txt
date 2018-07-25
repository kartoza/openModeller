#############################################################
# sources

SET(OMG_WIDGETS_SRCS
  omglistwidget.cpp 
  )

SET(OMG_WIDGETS_MOC_HDRS
  omglistwidget.h 
  )


QT4_WRAP_CPP(OMG_WIDGETS_MOC_SRCS ${OMG_WIDGETS_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/../ui
  ${QT_INCLUDE_DIR}
  )

#############################################################
# library

IF (WIN32)
  # expect that classes are being EXPORTED for the lib
  IF (MSVC)
    ADD_DEFINITIONS("-DOMG_LIB_EXPORT=__declspec(dllexport)")
  ELSE (MSVC)
    ADD_DEFINITIONS("-UOMG_LIB_EXPORT \"-DOMG_LIB_EXPORT=__declspec(dllexport)\"")
  ENDIF (MSVC)
ENDIF (WIN32)

ADD_LIBRARY(omg_widgets SHARED ${OMG_WIDGETS_SRCS} ${OMG_WIDGETS_MOC_SRCS} )


TARGET_LINK_LIBRARIES(omg_widgets
  ${QT_QTCORE_LIBRARY} 
  ${QT_QTGUI_LIBRARY} 
)

IF (APPLE)
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
  INSTALL(TARGETS omg_widgets RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    LIBRARY DESTINATION ${OMG_LIB_DIR})
ELSE (APPLE)
  INSTALL(TARGETS omg_widgets
    RUNTIME DESTINATION ${OMG_LIB_DIR}
    LIBRARY DESTINATION ${OMG_LIB_DIR})
ENDIF (APPLE)


# Added by Tim to install headers

#INSTALL(CODE "MESSAGE(\"Installing WIDGETS headers...\")")
#INSTALL(FILES ${OMG_WIDGETS_HDRS} ${OMG_WIDGETS_MOC_HDRS} DESTINATION ${OMG_INCLUDE_DIR})

