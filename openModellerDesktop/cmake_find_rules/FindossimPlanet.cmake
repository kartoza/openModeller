## Once run this will define: 
## 
## OSSIMPLANET_FOUND       = system has OSSIMPLANET lib
##
## OSSIMPLANET_LIBRARY     = full path to the library
##
## OSSIMPLANET_INCLUDE_DIR      = where to find headers 
##
## Tim Sutton


IF(APPLE)
#SET(GUI_TYPE MACOSX_BUNDLE)
#INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
FIND_LIBRARY(OSSIMPLANET_LIBRARY ossimPlanet)
MARK_AS_ADVANCED (OSSIM_PLANET_LIBRARY)
SET(EXTRA_LIBS ${OSSIMPLANET_LIBRARY} )
ENDIF (APPLE)


IF (OSSIMPLANET_LIBRARY)
   SET(OSSIMPLANET_FOUND TRUE)
ENDIF (OSSIMPLANET_LIBRARY)

IF (OSSIMPLANET_FOUND)
   IF (NOT OSSIMPLANET_FIND_QUIETLY)
     MESSAGE(STATUS "Found OSSIMPLANET: ${OSSIMPLANET_LIBRARY}")
   ENDIF (NOT OSSIMPLANET_FIND_QUIETLY)
ELSE (OSSIMPLANET_FOUND)
   IF (OSSIMPLANET_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find OSSIMPLANET")
   ENDIF (OSSIMPLANET_FIND_REQUIRED)
ENDIF (OSSIMPLANET_FOUND)
