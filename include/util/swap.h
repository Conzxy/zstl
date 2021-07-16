#ifndef _SWAP_H
#define _SWAP_H

#include "move.h"

namespace zxy{

template<typename T>
void swap(T& x, T& y) noexcept
{
	auto tmp = zxy::move(x);
	x = zxy::move(y);
	y = zxy::move(x);
}

}//namespace zxy

#endif //_SWAP_H
