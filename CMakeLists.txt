cmake_minimum_required(VERSION 2.6)

project(intro)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(ggl)
add_subdirectory(intro)
