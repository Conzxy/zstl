cmake_minimum_required(VERSION 3.17)
project(TinySTL)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)

add_library(TinySTL SHARED
        ${src} )