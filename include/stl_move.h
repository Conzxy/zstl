#ifndef ZSTL_STL_MOVE_H
#define ZSTL_STL_MOVE_H

#include "type_traits.h"
#include "config.h"

namespace zstl{
   
	// need remove_reference of T to avoid reference collapse 
    template<typename T>
    constexpr auto move(T&& t) noexcept
		-> decltype(static_cast<Remove_reference_t<T>&&>(t)){
        return static_cast<Remove_reference_t<T>&&>(t);
    }

#define STL_MOVE(val) zstl::move(val)

    // trick: reference collapse
	// you should use it with universal reference
	// e.g.
	// template<typename T> void g(T&& x) { return f(zstl::forward<T>(x)) }
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>& t) noexcept{
        return static_cast<T&&>(t);
    }//overload 1

    //prohibit reference to rvalue(end lifetime), trigger static_assert
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>&& t) noexcept{
        static_assert(!Is_lvalue_reference<T>::value,
                "template argument substituting T is an lvalue reference type");

        return static_cast<T&&>(t);
    }//overload 2

#define STL_FORWARD(type, args) zstl::forward<type>(args)

	// addressof
	template<typename T>
	inline T* addressof(T& p) noexcept
	{ return __builtin_addressof(p); }

	template<typename T>
	inline T* addressof(T&& p) = delete;
	
	// If T is throw move constructible and copyable
	// return T const&, else T&&
	// NOTE:
	// If move assignment operator is user-defined
	// the move constructor can be defined by compiler.
	// And vice, just move ctor is user-defined,
	// the compiler don't define move assignment operator
	// (I don't deeply view the standard about this topic)
	//
	// Therefore, in this case, we only check the move constructor is OK.
	template<typename T>
	struct move_if_noexcept_cond 
		: Conjunction_t<
			Negation<Is_nothrow_move_constructible<T>>,
			Is_copy_constructible<T>>
	{ };

	template<typename T>
	constexpr Conditional_t<move_if_noexcept_cond<T>::value,  
	T const&, T&&>
	move_if_noexcept(T&& x) noexcept
	{ return zstl::move(x); }

	// swap
	template<typename T>
	auto swap(T& x, T& y) 
	noexcept(Conjunction<Is_nothrow_move_constructible<T>,
			Is_nothrow_move_assignable<T>>::value) ->
		STL_ Enable_if_t<Conjunction<Is_move_constructible<T>,
				Is_move_assignable<T>>::value>
	{
		T tmp = STL_MOVE(x);
		x = STL_MOVE(y);
		y = STL_MOVE(tmp);
	}

	template<typename T, size_t N>
	auto swap(T(&x)[N], T(&y)[N])
	noexcept(Is_nothrow_swappable<T>::value) ->
		STL_ Enable_if_t<Is_swappable<T>::value>
	{
		for(size_t i = 0; i != N; ++i)
			swap(x[i], y[i]);
	}

#define STL_SWAP(val1,val2) zstl::swap(val1,val2)

} // namespace zstl

#endif //ZSTL_STL_MOVE_H
