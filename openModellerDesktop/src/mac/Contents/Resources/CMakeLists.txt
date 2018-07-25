FILE (GLOB ICONS *.icns)
INSTALL (FILES ${ICONS}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/openModellerDesktop.app/Contents/Resources)
