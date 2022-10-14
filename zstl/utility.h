#ifndef _ZXY_ZSTL_UTILITY_H
#define _ZXY_ZSTL_UTILITY_H

#include "stl_move.h"
#include "type_traits.h"
#include "config.h"

namespace zstl{

	template<typename T1,typename T2>
	struct pair{
		T1 first;
		T2 second;

		pair(T1 const& _first,T2 const& _second)
			: first{_first}
			, second{_second}
		{ }
		
		explicit pair(T1&& _first=T1{},T2&& _second=T2{})
			: first{zstl::move(_first)}
			, second{zstl::move(_second)}
		{ }
	};

	template<typename T1, typename T2>
	inline pair<Decay_t<T1>, Decay_t<T2>> make_pair(T1&& x, T2&& y){
		return pair<Decay_t<T1>, Decay_t<T2>>(
			zstl::forward<T1>(x), zstl::forward<T2>(y)
		);
	}


	template<typename InIter>
	InIter NextIter(InIter iter){
		return ++iter;
	}

	template<typename InIter>
	InIter PrevIter(InIter iter){
		return --iter;
	}
}

#endif // _ZXY_ZSTL_UTILITY_H
