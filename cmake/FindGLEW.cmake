#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
# 

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
	/usr/include
	/usr/local/include
	/sw/include
	/opt/local/include
	DOC "The directory where GL/glew.h resides")
FIND_LIBRARY(GLEW_LIBRARY
	NAMES GLEW glew
	PATHS
	/usr/lib64
	/usr/lib
	/usr/local/lib64
	/usr/local/lib
	/sw/lib
	/opt/local/lib
	DOC "The GLEW library")

IF (GLEW_INCLUDE_DIR)
	SET( GLEW_FOUND 1 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
ELSE (GLEW_INCLUDE_DIR)
	SET( GLEW_FOUND 0 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
ENDIF (GLEW_INCLUDE_DIR)

MARK_AS_ADVANCED(GLEW_FOUND)
