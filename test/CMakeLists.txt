cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})
aux_source_directory(${CMAKE_SOURCE_DIR}/src LIB_SRC)

add_executable(test_queue test_queue.cpp ${LIB_SRC})
add_executable(test_vector test_vector.cpp ${LIB_SRC})
add_executable(test_algo test_algo.cpp ${LIB_SRC})
