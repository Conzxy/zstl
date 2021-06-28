#include "../include/type_traits.h"

using namespace TinySTL;

int main(){
	static_assert(Is_same_v<Add_pointer_t<int()>, int(*)()>, "Add pointer failed\n");
	static_assert(Is_same_v<Add_pointer_t<int() const volatile>, int() const volatile>,
				  "Add_pointer have not effect in cv/ref qualified function type\n");
}
