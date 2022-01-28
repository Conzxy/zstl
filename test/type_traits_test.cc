#include "type_traits.h"
#include <type_traits>
using namespace zstl;

class AllOk{
public:
	AllOk(int) {}
};

class AllProhibited{
public:
	AllProhibited() = delete;
	~AllProhibited() = delete;
	AllProhibited(int ){
		a_ = 1;
	}
	
	AllProhibited(char ){}

	AllProhibited(AllProhibited const&) = delete;
	AllProhibited(AllProhibited&&) = delete;
	void operator=(AllProhibited const&) = delete;
	void operator=(AllProhibited&&) = delete;
private:
	int a_;
};

class Nothrow{
public:
	Nothrow() noexcept {}
	Nothrow(Nothrow const&) noexcept {}
	Nothrow(Nothrow&&) noexcept {}	
	Nothrow& operator=(Nothrow const& ) noexcept { return *this; }
	Nothrow& operator=(Nothrow&&) noexcept { return *this; }
	~Nothrow() noexcept {}
};

int main(){
		
	//object supported operation
	static_assert(!Is_default_constructible<AllProhibited>::value, "");
	static_assert(!Is_destructible<AllProhibited>::value, "");
	static_assert(Is_constructible<AllOk, int>::value, "");
	static_assert(!Is_constructible<AllProhibited, char const*, int>::value, "");
	static_assert(!Is_copy_constructible<AllProhibited>::value, "");
	static_assert(!Is_move_constructible<AllProhibited>::value, "");
	static_assert(!Is_move_constructible<char[22]>::value, "");
	static_assert(!Is_copy_constructible<char[22]>::value, "");
	static_assert(!Is_copy_assignable<AllProhibited>::value, "");
	static_assert(!Is_move_assignable<AllProhibited>::value, "");
	static_assert(Is_move_assignable<int>::value, "");
	static_assert(Is_move_constructible<int>::value, "");
	static_assert(Is_swappable_with<int&, int&>::value, "");
	static_assert(std::is_swappable_with<int&, int&>::value, "");
	static_assert(Is_nothrow_swappable_with<int&, int&>::value, "");
	static_assert(std::is_nothrow_swappable_with<int&, int&>::value, "");
	static_assert(Is_destructible<Nothrow>::value, "");
	static_assert(Is_nothrow_destructible<Nothrow>::value, "");
	static_assert(Is_nothrow_default_constructible<Nothrow>::value, "");
	static_assert(Is_nothrow_copy_constructible<Nothrow>::value, "");
	static_assert(noexcept(Nothrow().~Nothrow()), "");
	static_assert(noexcept(declval<AllOk>().~AllOk()), "");
	
	static_assert(Is_copy_assignable<int>::value, "");
	static_assert(Is_same_v<Add_pointer_t<int()>, int(*)()>, "Add pointer failed\n");
	static_assert(Is_same_v<Add_pointer_t<int() const volatile>, int() const volatile>,
				  "Add_pointer have not effect in cv/ref qualified function type\n");
}
