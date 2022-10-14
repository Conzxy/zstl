cmake_minimum_required(VERSION 3.10)
project(zstl)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CXX_FLAGS 
	-g
	-pthread
	-std=c++14)

set(CMAKE_CXX_FLAGS CXX_FLAGS)
# string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(ZSTL_INCLUDE ${PROJECT_SOURCE_DIR}/zstl)
set(ZSTL_SRC ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/zstl)
include_directories(${PROJECT_SOURCE_DIR}/zstl/func)
include_directories(${PROJECT_SOURCE_DIR}/zstl/util)

add_subdirectory(src)
add_subdirectory(test)
