#ifndef TINYSTL_STL_UTILITY_H
#define TINYSTL_STL_UTILITY_H

#include "stl_move.h"

namespace TinySTL{
	template<typename T1,typename T2>
	struct pair{
		T1 first;
		T2 second;

		pair(T1 const& _first,T2 const& _second)
			: first{_first},second{_second}{}
		pair(T1&& _first=T1{},T2&& _second=T2{})
			: first{_first},second{_second}{}
	};

	template<typename InIter>
	InIter Next_Iter(InIter iter){
		++iter;
	}

	template<typename InIter>
	InIter Prev_Iter(InIter iter){
		--iter;
	}
}
#endif //TINYSTL_STL_UTILITY_H
