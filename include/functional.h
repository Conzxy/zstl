#ifndef TEST_TYPE_ERASURE_H
#define TEST_TYPE_ERASURE_H

#include "config.h"
#include "stl_utility.h"
#include "type_traits.h"
#include <stdexcept>
#include "func/function.h"

namespace zstl {

namespace detail {
// @note
// here, SFINAE specialization and overload is useless
// because such type like lambda experession or function pointer, their default construct is prohibited
//
// 1) lambda expression if no capture, it's constructor is only called by compiler
// 2) if capture not empty, default constructor is implicitly deleted, 
// and specified constructor also only called by compiler
// 3) function pointer default construct just a wild pointer
// call it will trigger segment fault
//
// so, you must use function accept a callable object
// but SFINAE specialization and overload dependent on type,
// you should use SFINAE expression to solve, use tailing-return trick, you can use function arguments
template<typename R, typename... Args>
struct Callable_
{
	constexpr Callable_(zstl::function<R(Args...)> const&)
	{ }
};

// need user to specify return type and types of parameters 
template<typename R, typename F, typename ...Args>
constexpr auto Is_Callable_(F&& f)
	-> decltype(Callable_<R, Args...>(STL_FORWARD(F, f)), false)
{
	return true;
}

template<typename R, typename ...Args>
constexpr bool Is_Callable_(...)
{ return false; }

} // namespace detail

template<typename R, typename F, typename ...Args>
constexpr bool Is_Callable(F&& f)
{ return detail::Is_Callable_<R, F, Args...>(STL_FORWARD(F, f)); }

template<typename R, typename A, typename F>
constexpr bool Is_UnaryCallable(F&& f)
{ return Is_Callable<R, F, A>(STL_FORWARD(F, f)); }

template<typename R, typename A, typename B, typename F>
constexpr bool Is_BinaryCallable(F&& f)
{ return Is_Callable<R, F, A, B>(STL_FORWARD(F, f)); }

template<typename A, typename F>
constexpr bool Is_UnaryPredicate(F&& f)
{ return Is_UnaryCallable<bool, A, F>(STL_FORWARD(F, f)); }

template<typename A, typename B, typename F>
constexpr bool Is_BinaryPredicate(F&& f)
{ return Is_BinaryCallable<bool, A, B, F>(STL_FORWARD(F, f)); }

template<typename T=void>
struct plus {
	constexpr T operator()(T const& lhs, T const& rhs)const 
	noexcept(noexcept(lhs+rhs)){
		return lhs+rhs;
	}
};

template<>
struct plus<void> {
	template<typename T1,typename T2>
	constexpr  auto operator()(T1&& lhs, T2&& rhs)const noexcept(
		noexcept(
			static_cast<T1&&>(lhs)+static_cast<T2&&>(rhs)
			)
		) 
	-> decltype(static_cast<T1&&>(lhs)+static_cast<T2&&>(rhs)) {
		return static_cast<T1&&>(lhs)+static_cast<T2&&>(rhs);
	}
};

template<typename T=void>
struct minus {
	constexpr T operator()(T const& lhs,T const& rhs)const
	noexcept(noexcept(lhs-rhs)){
		return lhs-rhs;
	}
};

template<>
struct minus <void>{
	template<typename T1,typename T2>
	constexpr auto operator()(T1&& lhs, T2&& rhs) const 
	noexcept(noexcept(static_cast<T1&&>(lhs)-static_cast<T2&&>(rhs)))
	-> decltype(static_cast<T1&&>(lhs)-static_cast<T2&&>(rhs)) {
		return static_cast<T1&&>(lhs)-static_cast<T2&&>(rhs);
	}
};

template<typename T=void>
struct multiplies {
	constexpr T operator()(T const& lhs, T const& rhs)const
		noexcept(noexcept(lhs*rhs)) {
		return lhs*rhs;
	}
};

template<>
struct multiplies<void> {
	template<typename T1,typename T2>
	constexpr auto operator()(T1&& lhs, T2&& rhs)const
		noexcept(noexcept(static_cast<T1&&>(lhs)*static_cast<T2&&>(rhs))) 
	-> decltype(static_cast<T1&&>(lhs)*static_cast<T2&&>(rhs)){
		return static_cast<T1&&>(lhs)*static_cast<T2&&>(rhs);
	}
};

template<typename T>
struct less {
	constexpr bool operator()(T const& lhs, T const& rhs)const {
		return lhs<rhs;
	}
};

template<typename T>
struct equal_to{
	using first_argument_type   =T;
	using second_argument_type  =T;
	using result_type           =bool;

	bool operator()(T const& lhs,T const& rhs) const{
		return lhs == rhs;
	}
};

template<typename T>
struct identity{
	T operator()(T const& x) const{
		return x;
	}
};

template<typename T1,typename T2>
struct get_first{
	using argument_type =pair<T1,T2>;
	using result_type   =T1;

	T1 operator()(pair<T1,T2> const& p){
		return p.first;
	} 
};

}
#include "func/invoke.h"
#include "func/reference_wrapper.h"
#endif //TEST_TYPE_ERASURE_H
