SUBDIRS(ui lib widgets gui plugins designer)

IF (APPLE)
  SUBDIRS(mac)
ENDIF(APPLE)

IF (ENABLE_TESTS)
  SUBDIRS(tests)
ENDIF (ENABLE_TESTS)

IF (WITH_DESIGNER)
  SUBDIRS(designer)
ENDIF (WITH_DESIGNER)

IF (WITH_QGIS_PLUGIN)
  SUBDIRS(qgis_plugin)
ENDIF (WITH_QGIS_PLUGIN)

