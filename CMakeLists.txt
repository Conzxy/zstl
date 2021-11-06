cmake_minimum_required(VERSION 3.16)
project(TinySTL)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CXX_FLAGS 
	-g
	-pthread
	-std=c++14)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)
set(TINYSTL_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(TINYSTL_SRC ${CMAKE_SOURCE_DIR}/src)

link_libraries(/usr/local/lib/libgtest.a)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/func)
include_directories(${CMAKE_SOURCE_DIR}/include/util)

#set(LIB_SRC ${TINYSTL_SRC}/alloc.cpp ${TINYSTL_SRC}/stl_tree.cc ${TINYSTL_SRC}/rw.cc)

aux_source_directory(${TINYSTL_SRC} LIB_SRC)

#add_library(TinySTL SHARED ${SRC_LIST})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(test)
