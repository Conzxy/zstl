#pragma once

#include "../type_traits.h"
#include "reference_wrapper.h"
//FUNCTION TEMPLATE invoke
//applies to any function object type,such as
//function pointer or reference
//function object or with a conversion function to reference or pointer to function
//lambda expression
//(*)function pointer to member function/member data

namespace TinySTL{

    namespace detail{
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
    

        template<typename F,typename ...Args>
        constexpr decltype(auto)
        invoke_impl(F&& f,Args&&... args)
        {
            return TinySTL::forward<F>(f)(TinySTL::forward<Args>(args)...);
        }
    }

    template<typename F,typename ...Args>
    constexpr decltype(auto)
    invoke(F&& f,Args&&... args) noexcept(noexcept(f))
    {
        return detail::invoke_impl(std::forward<F>(f),std::forward<Args>(args)...);
    }

}