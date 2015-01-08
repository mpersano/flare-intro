set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(SDL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)

include_directories(${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIRS})
link_directories(${CMAKE_BINARY_DIR}/ggl)

set(INTRO_SOURCES
	util.cc
	bounding_box.cc
	frustum.cc
	mesh.cc
	scene.cc
	bezier.cc
	tube.cc
	main.cc)

add_executable(intro ${INTRO_SOURCES})
target_link_libraries(intro ggl ${SDL_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
