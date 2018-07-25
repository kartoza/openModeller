SET( QT_USE_QTTEST     TRUE )
#####################################################
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} 
# binary make output dirs for uics and mocs
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/../../
  ${CMAKE_CURRENT_BINARY_DIR}/../ui
# source dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../plugins/modellerplugins
  ${CMAKE_CURRENT_SOURCE_DIR}/../widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/../gui
  ${QT_INCLUDE_DIR}
  ${GDAL_INCLUDE_DIR}
  ${OM_INCLUDE_DIR}
  ${QGIS_INCLUDE_DIR}
  ${PROJ_INCLUDE_DIR}
  ${GEOS_INCLUDE_DIR}
  )

#MESSAGE("Test Data Dir: ${TEST_DATA_DIR}")
ADD_DEFINITIONS(-DTEST_DATA_DIR="\\"${TEST_DATA_DIR}\\"")

#############################################################
# libraries

# because of htonl
IF (WIN32)
  SET(PLATFORM_LIBRARIES wsock32)
ENDIF (WIN32)

# Since the tests are not actually installed, but rather
# run directly from the build/src/tests dir we need to
# ensure the omg libs can be found.
IF (APPLE)
  # For Mac OS X, the executable must be at the root of the bundle's executable folder
  SET (CMAKE_INSTALL_NAME_DIR @executable_path/../lib)
ENDIF (APPLE)


SET (REQUIRED_LIBS 
      ${QT_QTCORE_LIBRARY} 
      ${QT_QTTEST_LIBRARY} 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTXML_LIBRARY} 
      ${QT_QTSVG_LIBRARY} 
      ${QT_QTNETWORK_LIBRARY} 
      ${QT_QTMAIN_LIBRARY} 
      omg_core
      ${OM_LIBRARY}
    ) 
#note for tests we should not include the moc of our
#qtests in the executable file list as the moc is
#directly included in the sources
#and should not be compiled twice. Trying to include
#them in will cause an error at build time 

#############################################################
# sources
# omgui test
SET(omguitest_SRCS omguitest.cpp)
SET(omguitest_MOC_CPPS omguitest.cpp)
QT4_WRAP_CPP(omguitest_MOC_SRCS ${omguitest_MOC_CPPS})
ADD_CUSTOM_TARGET(omguitestmoc ALL DEPENDS ${omguitest_MOC_SRCS})
ADD_EXECUTABLE(omguitest ${omguitest_SRCS})
ADD_DEPENDENCIES(omguitest omguitestmoc)
TARGET_LINK_LIBRARIES(omguitest ${REQUIRED_LIBS})
INSTALL(TARGETS omguitest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(omgui ${OMG_TEST_DIR}/omguitest)

# omgmodel test
SET(modeltest_SRCS omgmodeltest.cpp)
SET(modeltest_MOC_CPPS omgmodeltest.cpp)
QT4_WRAP_CPP(modeltest_MOC_SRCS ${modeltest_MOC_CPPS})
ADD_CUSTOM_TARGET(modeltestmoc ALL DEPENDS ${modeltest_MOC_SRCS})
ADD_EXECUTABLE(modeltest ${modeltest_SRCS})
ADD_DEPENDENCIES(modeltest modeltestmoc)
TARGET_LINK_LIBRARIES(modeltest ${REQUIRED_LIBS})
INSTALL(TARGETS modeltest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(model ${OMG_TEST_DIR}/modeltest)

# omglocalitiesmodel test
SET(localitiesmodeltest_SRCS omglocalitiesmodeltest.cpp)
SET(localitiesmodeltest_MOC_CPPS omglocalitiesmodeltest.cpp)
QT4_WRAP_CPP(localitiesmodeltest_MOC_SRCS ${localitiesmodeltest_MOC_CPPS})
ADD_CUSTOM_TARGET(localitiesmodeltestmoc ALL DEPENDS ${localitiesmodeltest_MOC_SRCS})
ADD_EXECUTABLE(localitiesmodeltest ${localitiesmodeltest_SRCS})
ADD_DEPENDENCIES(localitiesmodeltest localitiesmodeltestmoc)
TARGET_LINK_LIBRARIES(localitiesmodeltest ${REQUIRED_LIBS})
INSTALL(TARGETS localitiesmodeltest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(localitiesmodel ${OMG_TEST_DIR}/localitiesmodeltest)

# graph test
SET(graphtest_SRCS omggraphtest.cpp)
SET(graphtest_MOC_CPPS omggraphtest.cpp)
QT4_WRAP_CPP(graphtest_MOC_SRCS ${graphtest_MOC_CPPS})
ADD_CUSTOM_TARGET(graphtestmoc ALL DEPENDS ${graphtest_MOC_SRCS})
ADD_EXECUTABLE(graphtest ${graphtest_SRCS})
ADD_DEPENDENCIES(graphtest graphtestmoc)
TARGET_LINK_LIBRARIES(graphtest ${REQUIRED_LIBS})
INSTALL(TARGETS graphtest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(graph ${OMG_TEST_DIR}/graphtest)

# gradientfactory test
SET(gradientfactorytest_SRCS omggradientfactorytest.cpp)
SET(gradientfactorytest_MOC_CPPS omggradientfactorytest.cpp)
QT4_WRAP_CPP(gradientfactorytest_MOC_SRCS ${gradientfactorytest_MOC_CPPS})
ADD_CUSTOM_TARGET(gradientfactorytestmoc ALL DEPENDS ${gradientfactorytest_MOC_SRCS})
ADD_EXECUTABLE(gradientfactorytest ${gradientfactorytest_SRCS})
ADD_DEPENDENCIES(gradientfactorytest gradientfactorytestmoc)
TARGET_LINK_LIBRARIES(gradientfactorytest ${REQUIRED_LIBS})
INSTALL(TARGETS gradientfactorytest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(gradientfactory ${OMG_TEST_DIR}/gradientfactorytest)

# dataseries test
SET(dataseriestest_SRCS omgdataseriestest.cpp)
SET(dataseriestest_MOC_CPPS omgdataseriestest.cpp)
QT4_WRAP_CPP(dataseriestest_MOC_SRCS ${dataseriestest_MOC_CPPS})
ADD_CUSTOM_TARGET(dataseriestestmoc ALL DEPENDS ${dataseriestest_MOC_SRCS})
ADD_EXECUTABLE(dataseriestest ${dataseriestest_SRCS})
ADD_DEPENDENCIES(dataseriestest dataseriestestmoc)
TARGET_LINK_LIBRARIES(dataseriestest ${REQUIRED_LIBS})
INSTALL(TARGETS dataseriestest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(dataseries ${OMG_TEST_DIR}/dataseriestest)

# experiment test
SET(experimenttest_SRCS omgexperimenttest.cpp)
SET(experimenttest_MOC_CPPS omgexperimenttest.cpp)
QT4_WRAP_CPP(experimenttest_MOC_SRCS ${experimenttest_MOC_CPPS})
ADD_CUSTOM_TARGET(experimenttestmoc ALL DEPENDS ${experimenttest_MOC_SRCS})
ADD_EXECUTABLE(experimenttest ${experimenttest_SRCS})
ADD_DEPENDENCIES(experimenttest experimenttestmoc)
TARGET_LINK_LIBRARIES(experimenttest ${REQUIRED_LIBS})
INSTALL(TARGETS experimenttest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(experiment ${OMG_TEST_DIR}/experimenttest)

# experimentprinter test
SET(experimentprintertest_SRCS omgexperimentprintertest.cpp)
SET(experimentprintertest_MOC_CPPS omgexperimentprintertest.cpp)
QT4_WRAP_CPP(experimentprintertest_MOC_SRCS ${experimentprintertest_MOC_CPPS})
ADD_CUSTOM_TARGET(experimentprintertestmoc ALL DEPENDS ${experimentprintertest_MOC_SRCS})
ADD_EXECUTABLE(experimentprintertest ${experimentprintertest_SRCS})
ADD_DEPENDENCIES(experimentprintertest experimentprintertestmoc)
TARGET_LINK_LIBRARIES(experimentprintertest ${REQUIRED_LIBS})
INSTALL(TARGETS experimentprintertest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(experimentprinter ${OMG_TEST_DIR}/experimentprintertest)

# localities to csv test
#localities2csvtest.cpp
SET(localities2csvtest_SRCS localities2csvtest.cpp)
SET(localities2csvtest_MOC_CPPS localities2csvtest.cpp)
QT4_WRAP_CPP(localities2csvtest_MOC_SRCS ${localities2csvtest_MOC_CPPS})
ADD_CUSTOM_TARGET(localities2csvtestmoc ALL DEPENDS ${localities2csvtest_MOC_SRCS})
ADD_EXECUTABLE(localities2csvtest ${localities2csvtest_SRCS})
ADD_DEPENDENCIES(localities2csvtest localities2csvtestmoc)
TARGET_LINK_LIBRARIES(localities2csvtest ${REQUIRED_LIBS})
INSTALL(TARGETS localities2csvtest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(localities2csv ${OMG_TEST_DIR}/localities2csvtest)

#openmodeller log tests (using omg callbacks)
# openmodellerlogtest.cpp
SET(openmodellerlogtest_SRCS openmodellerlogtest.cpp)
SET(openmodellerlogtest_MOC_CPPS openmodellerlogtest.cpp)
QT4_WRAP_CPP(openmodellerlogtest_MOC_SRCS ${openmodellerlogtest_MOC_CPPS})
ADD_CUSTOM_TARGET(openmodellerlogtestmoc ALL DEPENDS ${openmodellerlogtest_MOC_SRCS})
ADD_EXECUTABLE(openmodellerlogtest ${openmodellerlogtest_SRCS})
ADD_DEPENDENCIES(openmodellerlogtest openmodellerlogtestmoc)
TARGET_LINK_LIBRARIES(openmodellerlogtest ${REQUIRED_LIBS} ${OM_LIBRARY} ${EXPAT_LIBRARY})
INSTALL(TARGETS openmodellerlogtest RUNTIME DESTINATION ${OMG_TEST_DIR})
ADD_TEST(openmodellerlog ${OMG_TEST_DIR}/openmodellerlogtest)


#SET (MODELLERPLUGINDEST ${OMG_TEST_DIR}/plugins/modeller/)
SET (MODELLERPLUGINDEST ${OMG_TEST_DIR})
#local plugin tests
#  omgmodellerlocalplugintest.cpp
SET(openmodellerlocalplugintest_SRCS omgmodellerlocalplugintest.cpp)
SET(openmodellerlocalplugintest_MOC_CPPS omgmodellerlocalplugintest.cpp)
QT4_WRAP_CPP(openmodellerlocalplugintest_MOC_SRCS ${openmodellerlocalplugintest_MOC_CPPS})
ADD_CUSTOM_TARGET(openmodellerlocalplugintestmoc ALL DEPENDS ${openmodellerlocalplugintest_MOC_SRCS})
ADD_EXECUTABLE(openmodellerlocalplugintest ${openmodellerlocalplugintest_SRCS})
ADD_DEPENDENCIES(openmodellerlocalplugintest openmodellerlocalplugintestmoc)
TARGET_LINK_LIBRARIES(openmodellerlocalplugintest ${REQUIRED_LIBS} omg_localmodellerplugin)
INSTALL(TARGETS openmodellerlocalplugintest RUNTIME DESTINATION ${MODELLERPLUGINDEST})
ADD_TEST(openmodellerlocalplugin ${MODELLERPLUGINDEST}/openmodellerlocalplugintest)

IF (NOT WIN32)
# ws plugin tests
#  omgmodellerwebservicesplugintest.cpp
SET(openmodellerwebservicesplugintest_SRCS omgmodellerwebservicesplugintest.cpp)
SET(openmodellerwebservicesplugintest_MOC_CPPS omgmodellerwebservicesplugintest.cpp)
QT4_WRAP_CPP(openmodellerwebservicesplugintest_MOC_SRCS ${openmodellerwebservicesplugintest_MOC_CPPS})
ADD_CUSTOM_TARGET(openmodellerwebservicesplugintestmoc ALL DEPENDS ${openmodellerwebservicesplugintest_MOC_SRCS})
ADD_EXECUTABLE(openmodellerwebservicesplugintest ${openmodellerwebservicesplugintest_SRCS})
ADD_DEPENDENCIES(openmodellerwebservicesplugintest openmodellerwebservicesplugintestmoc)
TARGET_LINK_LIBRARIES(openmodellerwebservicesplugintest ${REQUIRED_LIBS} omg_soapmodellerplugin)
INSTALL(TARGETS openmodellerwebservicesplugintest RUNTIME DESTINATION ${MODELLERPLUGINDEST})
ADD_TEST(openmodellerwebservicesplugin ${MODELLERPLUGINDEST}/openmodellerwebservicesplugintest)
ENDIF (NOT WIN32)


# Put the libs needed into the test dir
#MESSAGE ("Current Binary Dir: ${CMAKE_CURRENT_BINARY_DIR}")
FILE (GLOB AlgFiles ${CMAKE_INSTALL_PREFIX}/algs/*.dll)
IF (WIN32)
INSTALL (FILES ${CMAKE_INSTALL_PREFIX}/omg_core.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
INSTALL (FILES ${CMAKE_INSTALL_PREFIX}/qgis_core.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
INSTALL (FILES ${CMAKE_INSTALL_PREFIX}/openmodeller.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
INSTALL (FILES ${AlgFiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/algs/)
INSTALL (FILES ${CMAKE_INSTALL_PREFIX}/plugins/modeller/omg_localmodellerplugin.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/plugins/modeller/)
ENDIF (WIN32)

IF (APPLE)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/lib/libomg_core.dylib DESTINATION ${OMG_TEST_DIR}/lib/)
INSTALL (FILES ${QGIS_CORE_LIBRARY} DESTINATION ${OMG_TEST_DIR}/lib/)
INSTALL (FILES ${QGIS_GUI_LIBRARY} DESTINATION ${OMG_TEST_DIR}/lib/)
#experiment tests need modeller plugins here
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/plugins/modellerplugins/libomg_localmodellerplugin.dylib DESTINATION ${OMG_TEST_DIR}/lib/plugins/modeller/)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/plugins/modellerplugins/libomg_soapmodellerplugin.dylib DESTINATION ${OMG_TEST_DIR}/lib/plugins/modeller/)
#and direct modeller plugin tests need them in @executeable_path/lib so 
#we make a second copy until I can work out how to automate
#CMAKE_INSTALL_NAME properly...
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/plugins/modellerplugins/libomg_localmodellerplugin.dylib DESTINATION ${OMG_TEST_DIR}/lib/)
INSTALL (FILES ${CMAKE_BINARY_DIR}/src/plugins/modellerplugins/libomg_soapmodellerplugin.dylib DESTINATION ${OMG_TEST_DIR}/lib//)
ENDIF (APPLE)
