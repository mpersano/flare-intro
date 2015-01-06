set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(SDL REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${SDL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

set(GGL_SOURCES
	window.cc
	panic.cc
	program.cc
	shader.cc
	texture.cc
	framebuffer.cc)

add_library(ggl ${GGL_SOURCES})
