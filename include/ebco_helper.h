#ifndef _ZXY_EBCO_HELPER_H
#define _ZXY_EBCO_HELPER_H

#include "type_traits.h"
#include "stl_move.h"

// This is just a EBCO generic scheme demo
// Usage:
// EBCO(unique-identifier)
//
// // assume unique-identifier is Allocator in following
// // If Allocator is empty class,
// // then static_assert(sizeof(A<EmptyAllocator>) == 1) must be passed,
// // otherwise, it is no effect like a ordinary base class (and it is more complicated to access data member)
//
// template<typename Allocator>
// struct A : EBCO_Allocator<Allocator>
// { 
//   template<typename... U>
//   A(U&&... t)
//     : EBCO_Allocator<Allocator>(STL_FORWARD(U, t)...)
//   { }
//   
// };
//
// Thus, I think this is very difficult to use for convenient
// since the restriction of template.
// (I guess this is the reason for why stl don't adopt it)

#define EBCO(Base) \
namespace detail { \
template<typename B, typename = zstl::_true_type> \
struct EBCO_##Base \
{ \
	EBCO_##Base(B const& b) \
		: ebco_##Base(b) \
	{ } \
	B& get_##Base() { return ebco_##Base; } \
	B ebco_##Base; \
}; \
 \
template<typename B> \
struct EBCO_##Base < B, zstl::Conjunction_t< \
	zstl::Is_empty<B>,  \
	zstl::Negation<zstl::Is_final<B>>> > \
	: public B \
{ \
  EBCO_##Base() = default; \
	B& get_##Base() { return *this; } \
}; \
 \
} /* namespace detail */ \
 \
template<typename B> \
struct EBCO_##Base : ::detail::EBCO_##Base<B> { \
  typedef ::detail::EBCO_##Base<B> base; \
  template<typename... Args> \
  explicit EBCO_##Base(Args&&... args) \
    : base{ STL_FORWARD(Args, args)... } \
  { } \
\
};

#endif //_ZXY_EBCO_HELPER_H
