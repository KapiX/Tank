#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# 

IF (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		HINTS
		$ENV{LIBRARYDIR}
		PATH_SUFFIXES include
		PATHS
		$ENV{PROGRAMFILES}/GLEW/include
		DOC "The directory where GL/glew.h resides")
		FIND_LIBRARY( GLEW_LIBRARY
			NAMES glew GLEW glew32 glew32s
			HINTS
			$ENV{LIBRARYDIR}
			PATH_SUFFIXES lib64 lib
			PATHS
			$ENV{PROGRAMFILES}/GLEW/lib
			DOC "The GLEW library"
		)
ELSE (WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		PATHS
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARY
		NAMES GLEW glew
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")
ENDIF (WIN32)

IF (GLEW_INCLUDE_PATH)
	SET(GLEW_FOUND TRUE)
ELSE (GLEW_INCLUDE_PATH)
	SET(GLEW_FOUND FALSE)
ENDIF (GLEW_INCLUDE_PATH)

IF(GLEW_FIND_REQUIRED)
  IF(NOT GLEW_FOUND)
    MESSAGE(FATAL_ERROR "GLEW not found")
  ENDIF(NOT GLEW_FOUND)
ENDIF(GLEW_FIND_REQUIRED)


MARK_AS_ADVANCED( GLEW_FOUND )
