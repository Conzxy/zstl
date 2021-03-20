#ifndef TINYSTL_STL_UTILITY_H
#define TINYSTL_STL_UTILITY_H

#include "_move.h"

template<typename InIter>
InIter Next_Iter(InIter iter){
	++iter;
}

template<typename InIter>
InIter Prev_Iter(InIter iter){
	--iter;
}
#endif //TINYSTL_STL_UTILITY_H
