cmake_minimum_required(VERSION 3.16)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(TINYSTL_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(TINYSTL_SRC ${CMAKE_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/func)
include_directories(${CMAKE_SOURCE_DIR}/include/IO)
include_directories(${CMAKE_SOURCE_DIR}/include/util)
include_directories(${CMAKE_SOURCE_DIR}/include/time)

#set(LIB_SRC ${TINYSTL_SRC}/alloc.cpp ${TINYSTL_SRC}/stl_tree.cc ${TINYSTL_SRC}/rw.cc)

aux_source_directory(${TINYSTL_SRC} LIB_SRC)

#add_library(TinySTL SHARED ${SRC_LIST})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(test)
