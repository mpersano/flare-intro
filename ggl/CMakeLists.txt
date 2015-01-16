set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__ -DGLEW_STATIC")
endif()

find_package(SDL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${PNG_INCLUDE_DIRS})

set(GGL_SOURCES
	window.cc
	panic.cc
	file.cc
	program.cc
	shader.cc
	texture.cc
	pixmap.cc)

add_library(ggl ${GGL_SOURCES})
