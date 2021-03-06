## Once run this will define: 
## 
## OSGTEXT_FOUND       = system has OSGTEXT_PLANET lib
##
## OSGTEXT_LIBRARY     = full path to the library
##
## OSGTEXT_INCLUDE_DIR      = where to find headers 
##
## Tim Sutton


IF(APPLE)
#SET(GUI_TYPE MACOSX_BUNDLE)
#INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
FIND_LIBRARY(OSGTEXT_LIBRARY osgText)
MARK_AS_ADVANCED (OSGTEXT_LIBRARY)
SET(EXTRA_LIBS ${OSGTEXT_LIBRARY} )
ENDIF (APPLE)


IF (OSGTEXT_LIBRARY)
   SET(OSGTEXT_FOUND TRUE)
ENDIF (OSGTEXT_LIBRARY)

IF (OSGTEXT_FOUND)
   IF (NOT OSGTEXT_FIND_QUIETLY)
     MESSAGE(STATUS "Found OSGTEXT_PLANET: ${OSGTEXT_LIBRARY}")
   ENDIF (NOT OSGTEXT_FIND_QUIETLY)
ELSE (OSGTEXT_FOUND)
   IF (OSGTEXT_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find OSGTEXT_PLANET")
   ENDIF (OSGTEXT_FIND_REQUIRED)
ENDIF (OSGTEXT_FOUND)
