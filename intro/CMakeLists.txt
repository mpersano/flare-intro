set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DGLM_SWIZZLE -DGLM_FORCE_RADIANS")
if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -U__STRICT_ANSI__ -DGLEW_STATIC -mwindows")
endif()

find_package(SDL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLM REQUIRED)
find_package(OggVorbis REQUIRED)
find_package(OpenAL REQUIRED)
find_package(PNG REQUIRED)

include_directories(
	${SDL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLM_INCLUDE_DIRS}
	${VORBIS_INCLUDE_DIR}
	${OGG_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS})

link_directories(${CMAKE_BINARY_DIR}/ggl)

set(INTRO_SOURCES
	util.cc
	program_manager.cc
	bounding_box.cc
	ogg_player.cc
	fft.cc
	spectrum.cc
	frustum.cc
	scene.cc
	bezier.cc
	tube.cc
	text.cc
	boid.cc
	dragon.cc
	quadtree.cc
	main.cc
	particle_texture.cc)

add_executable(intro ${INTRO_SOURCES})

target_link_libraries(
	intro
	ggl
	${SDL_LIBRARY}
	${GLEW_LIBRARY}
	${OPENGL_LIBRARIES}
	${VORBIS_LIBRARY}
	${OGG_LIBRARY}
	${VORBISFILE_LIBRARY}
	${OPENAL_LIBRARY}
	${PNG_LIBRARIES})
