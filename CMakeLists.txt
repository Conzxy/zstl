cmake_minimum_required(VERSION 3.16)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SRC_LIST)

#add_library(TinySTL SHARED ${SRC_LIST})

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(test)
