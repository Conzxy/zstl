/*
 * @headfile EBCO_helper.h
 * 
 * @author Conzxy
 * @date 25-6-2021
 */

#ifndef _ZXY_EBCO_HELPER_H
#define _ZXY_EBCO_HELPER_H

#include "stl_type_traits.h"
#include <type_traits>

namespace TinySTL{

#define EBCO_HELPER(classname, fieldname) \
template<typename Base, \
	bool=Disjunction_v<std::is_empty<Base>, \
				  Negation<std::is_final<Base>>>> \
struct classname : Base{	\
	\
}; \
\
template<typename Base>	\
struct classname<Base, false>{ \
	Base filename; \
};

}//namespace TinySTL

#endif _ZXY_EBCO_HELPER_H //_ZXY_EBCO_HELPER_H
