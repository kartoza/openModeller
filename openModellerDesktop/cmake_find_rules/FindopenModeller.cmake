## Once run this will define: 
## 
## OM_FOUND       = system has OM lib
##
## OM_LIBRARY     = full path to the library
##
## OM_INCLUDE_DIR      = where to find headers 
##
## Tim Sutton


IF(WIN32)

  FIND_PATH(OM_INCLUDE_DIR openmodeller/openmodeller.hh 
    /usr/local/openModeller/include
	  "$ENV{LIB_DIR}/include"
	  "C:/program files/openmodeller/include" 
	  )
  FIND_LIBRARY(OM_LIBRARY NAMES openmodeller PATHS 
    /usr/local/openModeller/lib
	  "$ENV{LIB_DIR}/lib"
	  "C:/program files/openmodeller/" 
	  )

  
ELSE(WIN32)
  IF(UNIX) 

    FIND_PROGRAM(OM om_console
      $ENV{LIB_DIR}/bin
      /usr/local/openModeller/bin
      /usr/local/bin/
      /usr/bin/
      )
    
    IF (OM) 
      ## remove suffix om_console because we need the pure directory
      STRING(REGEX REPLACE "/bin/om_console" "" OM_PREFIX ${OM} )
      # set INCLUDE_DIR to prefix+include
      SET(OM_INCLUDE_DIR ${OM_PREFIX}/include CACHE STRING INTERNAL)

      ## extract link dirs 
      SET(OM_LIB_DIR ${OM_PREFIX}/lib CACHE STRING INTERNAL)

      IF (APPLE)
        SET(OM_LIBRARY ${OM_LIB_DIR}/libopenmodeller.dylib CACHE STRING INTERNAL)
      ELSE (APPLE)
        SET(OM_LIBRARY ${OM_LIB_DIR}/libopenmodeller.so CACHE STRING INTERNAL)
      ENDIF (APPLE)
    ELSE(OM)
      MESSAGE("FindOM.cmake: OM not found. Please set it manually.")
    ENDIF(OM)
  ENDIF(UNIX)
ENDIF(WIN32)


IF (OM_INCLUDE_DIR AND OM_LIBRARY)
   SET(OM_FOUND TRUE)
ENDIF (OM_INCLUDE_DIR AND OM_LIBRARY)

IF (OM_FOUND)

   IF (NOT OM_FIND_QUIETLY)
      MESSAGE(STATUS "Found OM: ${OM_LIBRARY}")
   ENDIF (NOT OM_FIND_QUIETLY)

ELSE (OM_FOUND)

   IF (OM_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find OM")
   ENDIF (OM_FIND_REQUIRED)

ENDIF (OM_FOUND)
