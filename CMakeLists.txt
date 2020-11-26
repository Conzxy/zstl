cmake_minimum_required(VERSION 3.17)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)

set(src
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Detail/_alloc.cpp
    )

add_executable(TinySTL ${src})

target_include_directories(TinySTL
    PRIVATE
    ${PROJECT_SOURCE_DIR}/Detail
    ${PROJECT_SOURCE_DIR}/src
    )