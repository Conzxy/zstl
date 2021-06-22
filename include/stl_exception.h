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
#define TRY_BEGIN try{
#define TRY_END }
#define CATCH_ALL_BEGIN catch(...){
#define CATCH_END   }
#define CATCH_BEGIN(type) catch(type const&){
#define RETHROW throw;
#endif //TINYSTL_STL_EXCEPTION_H
