#ifndef _ZXY_INVOKE_H
#define _ZXY_INVOKE_H

#include "../type_traits.h"
#include "../stl_move.h"
#include "../config.h"

//FUNCTION TEMPLATE invoke
//applies to any function object type,such as
//function pointer or reference
//function object or with a conversion function to reference or pointer to function
//lambda expression
//(*)function pointer to member function/member data

namespace TinySTL{
    template<typename T>
    struct reference_wrapper;

    namespace detail{
#if __cplusplus >= 201703L
        template<typename >
        inline constexpr bool Is_reference_wrapper_v=false;
        template<typename T>
        inline constexpr bool Is_reference_wrapper_v<reference_wrapper<T>> =true;

        template<typename B,typename D,typename FM,typename ...Args>
        constexpr decltype(auto)
        invoke_impl(FM B::*f,D&& obj,Args&&... args)
        {
            if constexpr(Is_member_function_pointer_v<decltype(f)>)
            {
                if constexpr(Is_base_of_v<B,TinySTL::Decay_t<D>>)
                    return (TinySTL::forward<D>(obj).*f)(TinySTL::forward<Args>(args)...);
                else if constexpr(Is_reference_wrapper_v<Decay_t<D>>)
                    return (obj.get().*f)(TinySTL::forward<Args>(args)...);
                else
                    return (*TinySTL::forward<D>(obj).*f)(TinySTL::forward<Args>(args)...);
            }
            else
            {
                static_assert(Is_member_object_pointer_v<decltype(f)>);
                static_assert(sizeof...(Args)==0); 
                if constexpr(Is_base_of_v<B,Decay_t<D>>)
                    return TinySTL::forward<D>(obj).*f;  
                else if constexpr(Is_reference_wrapper_v<Decay_t<D>>)
                    return obj.get().*f;
                else
                    return *TinySTL::forward<D>(obj).*f;
            }
        }
    
#else // c++17 or pre
		template<typename T>
		struct Is_reference_wrapper : _false_type {};

		template<typename T>
		struct Is_reference_wrapper<reference_wrapper<T>> : _true_type {};

		template<typename B, typename D, typename PMD, STL_ENABLE_IF((Is_base_of<B, Decay_t<D>>::value), int)>
		auto invoke_impl(PMD B::* pmd, D&& obj) ->
			decltype(STL_FORWARD(D, obj).*pmd)
		{
			return STL_FORWARD(D, obj).*pmd;
		}

		template<typename B, typename D, typename PMD, STL_ENABLE_IF((Is_reference_wrapper<Decay_t<D>>::value), char)>
		auto invoke_impl(PMD B::*pmd, D&& obj) ->
			decltype(obj.get().*pmd)
		{
			return obj.get().*pmd;
		}

		template<typename B, typename D, typename PMD>
		auto invoke_impl(PMD B::*pmd, D&& obj) ->
			decltype(STL_FORWARD(D, obj)->*pmd)
		{
			return STL_FORWARD(D, obj)->*pmd;
		}

		template<typename B, typename D, typename PMF, STL_ENABLE_IF((Is_base_of<B, Decay_t<D>>::value), int), typename ...Args>
		auto invoke_impl(PMF B::*pmf, D&& obj, Args&&... args) ->
			decltype(STL_FORWARD(D, obj).*pmf(STL_FORWARD(Args, args)...))
		{
			return STL_FORWARD(D, obj).*pmf(STL_FORWARD(Args, args)...);
		}

		template<typename B, typename D, typename PMF, STL_ENABLE_IF((Is_reference_wrapper<Decay_t<D>>::value), char), 
			typename ...Args>
		auto invoke_impl(PMF B::* pmf, D&& obj, Args&&... args) ->
			decltype(obj.get().*pmf(STL_FORWARD(Args, args)...))
		{
			return obj.get().*pmf(STL_FORWARD(Args, args)...);
		}

		template<typename B, typename D, typename PMF, typename ...Args>
		auto invoke_impl(PMF B::* pmf, D&& obj, Args&&... args) ->
			decltype(STL_FORWARD(D, obj)->*pmf(STL_FORWARD(Args, args)...))
		{
			return STL_FORWARD(D, obj)->*pmf(STL_FORWARD(Args, args)...);
		}

#endif // __cplusplus >= 201703L
        template<typename F,typename ...Args>
        constexpr auto
        invoke_impl(F&& f,Args&&... args)
		-> decltype(TinySTL::forward<F>(f)(TinySTL::forward<Args>(args)...))
        {
            return TinySTL::forward<F>(f)(TinySTL::forward<Args>(args)...);
        }
    }  // namespace detail

    template<typename F,typename ...Args>
    constexpr auto
    invoke(F&& f,Args&&... args) noexcept(noexcept(f))
	-> decltype(detail::invoke_impl(TinySTL::forward<F>(f),TinySTL::forward<Args>(args)...))
    {
        return detail::invoke_impl(TinySTL::forward<F>(f),TinySTL::forward<Args>(args)...);
    }

}

#endif //_ZXY_INVOKE_H
