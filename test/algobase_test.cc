#include "stl_algobase.h"
#include "stl_iterator.h"

#include <stdio.h>

struct A {
	A() = default;
	
	A& operator=(A&&) noexcept {
		printf("A move assignment\n");
		return *this;
	}
};

using namespace zstl;

int main() {
	A buf[4096];
	A buf2[2];
	
	A* pa = &buf[0];
	*pa = *make_move_if_noexcept_iterator(static_cast<A*>(buf2));
	// copy(make_move_if_noexcept_iterator((A*)buf2), 
	// 	 make_move_if_noexcept_iterator((A*)(buf2 + sizeof buf2)),
	// 	 buf);
}
