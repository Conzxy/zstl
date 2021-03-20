#ifndef TINYSTL_STL_EXCEPTION_H
#define TINYSTL_STL_EXCEPTION_H

#include <stdexcept>
#include <cassert>
#include "type_traits.h"
#define TINYSTL_DEBUG(expr) \
        assert(expr)

#define THROW_RANGE_ERROR_IF(expr,what) \
        if((expr)) throw std::range_error(what)

#define THROW_LENGTH_ERROR_IF(expr,what) \
        if((expr)) throw std::length_error(what)

#define STL_TRY try

#endif //TINYSTL_STL_EXCEPTION_H
