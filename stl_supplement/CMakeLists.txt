cmake_minimum_required(VERSION 3.16)
project(stl_supplement)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CXX_FLAGS
  -g
  -pthread
  -std=c++11)

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
set(SRC_PATH ${CMAKE_SOURCE_DIR}/src)

include_directories(${INCLUDE_PATH})

message("INCLUDE_PATH: ${INCLUDE_PATH}")
aux_source_directory(${SRC_PATH}/forward_list FORWARD_LIST_SRC)

message("FORWARD_LIST_SRC: ${FORWARD_LIST_SRC}")

add_subdirectory(test)
