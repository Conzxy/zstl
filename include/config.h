#ifndef ZSTL_CONFIG_H
#define ZSTL_CONFIG_H

#include <cstdlib>

#define STL_ zstl::
#define STD_ std::
#define TL_ mpl::TL::
#define STL_ENABLE_IF(condition, type) zstl::Enable_if_t<condition, type> = 0
#define ADDRESSOF zstl::addressof

using sz_t=std::size_t;

#if __cplusplus >= 201103L
#define CXX_STANDARD_11
#endif 

#ifndef CXX_STANDARD_11
#define CXX_STANDARD_98 // this is maybe replaced by !CXX_STANDARD_11
#endif 

#if __cplusplus >= 201402L
#define CXX_STANDARD_14
#endif

#if __cplusplus >= 201703L
#define CXX_STANDARD_17
#endif 

// In C++11, constexpr function is incomplete
// 1): constexpr used in non-static member function impiles
//  const member function
// 2): constexpr function don't allowed to declare local variable and for/while/if
//
#ifdef CXX_STANDARD_14
#define ZSTL_CONSTEXPR constexpr
#else
#define ZSTL_CONSTEXPR inline 
#endif

// Before C++11, should use throw() to indicate the function no throw
#ifdef CXX_STANDARD_11
#define ZSTL_NOEXCEPT noexcept
#else
#define ZSTL_NOEXCEPT throw()
#endif

#endif //ZSTL_CONFIG_H
