cmake_minimum_required(VERSION 3.16)
project(test)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(C://lib//fmt//include)

aux_source_directory(${CMAKE_SOURCE_DIR}/src LIB_SRC)

add_executable(test_queue test_queue.cpp ${LIB_SRC})
add_executable(test_vector test_vector.cpp ${LIB_SRC})
add_executable(test_algo test_algo.cpp ${LIB_SRC})
add_executable(test_hashtable test_hashtable.cpp ${LIB_SRC})
add_executable(test_other test_other.cpp ${LIB_SRC})

target_link_libraries(test_other C://lib//fmt//libfmt.a)
