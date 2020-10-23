cmake_minimum_required(VERSION 3.17)
project(TinySTL)

set(CMAKE_CXX_STANDARD 17)

add_executable(TinySTL _move.h Detail/stl_type_traits.h stl_utility.h
        stl_iterator.h Iterator.h stl_construct.h Memory.h
        type_traits.h alloc.h Detail/_alloc.cpp stl_exception.h
        Allocator.h Detail/stl_type_traits_base.h stl_algorithm.h
        stl_uninitialized.h stl_vector.h Detail/Vector_impl.h
        vector.h
        main.cpp)