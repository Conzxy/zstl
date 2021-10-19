/*
 * @headfile EBCO_helper.h
 * 
 * @author Conzxy
 * @date 25-6-2021
 */

#ifndef _ZXY_EBCO_HELPER_H
#define _ZXY_EBCO_HELPER_H

#include "type_traits.h"

namespace TinySTL{

#define EBCO(Base) \
template<typename B, typename = _true_type> \
struct EBCO_##Base##_ \
{ \
	B& get_##Base() { return ebco; } \
	B ebco; \
}; \
\
template<typename B> \
struct EBCO_##Base##_ < B, Conjunction_t< \
	Is_empty<B>,  \
	Negation<Is_final<B>>> > \
	: B \
{ \
	B& get_##Base() { return *this; } \
}; \
 \
struct EBCO_##Base : EBCO_##Base##_<Base> {}; 

}//namespace TinySTL

#endif //_ZXY_EBCO_HELPER_H
