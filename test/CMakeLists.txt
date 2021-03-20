cmake_minimum_required(VERSION 3.17)
project(test)

set(CMAKE_CXX_STANDARD 17)


# 引入头文件目录
include_directories(${CMAKE_SOURCE_DIR}/TinySTL/incl)
include_directories(${PROJECT_SOURCE_DIR})
message(${CMAKE_SOURCE_DIR})

#link_directories(${CMAKE_SOURCE_DIR}/cmake-build-debug/TinySTL)
#add_executable(test)
add_executable(test_queue test_queue.cpp)
add_executable(test_vector test_vector.cpp)

# 引入实现，即静态库或动态库
# TinySTL对应TinySTL project
target_link_libraries(test_vector PUBLIC
        TinySTL
        )

#target_include_directories(test PUBLIC
#        ${CMAKE_SOURCE_DIR}/TinySTL/incl
#        )
#target_include_directories(test PUBLIC
#        "${PROJECT_BINARY_DIR}"
#        "${PROJECT_SOURCE_DIR}/incl"
#        )