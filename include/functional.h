#ifndef TEST_TYPE_ERASURE_H
#define TEST_TYPE_ERASURE_H

#include "stl_utility.h"
#include "type_traits.h"
#include <stdexcept>

namespace TinySTL {
    

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
        constexpr auto operator()(T1&& lhs, T2&& rhs)const
            noexcept(noexcept(static_cast<T1&&>(lhs)+static_cast<T2&&>(rhs))) {
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
        noexcept(noexcept(static_cast<T1&&>(lhs)-static_cast<T2&&>(rhs))){
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
            noexcept(noexcept(static_cast<T1&&>(lhs)*static_cast<T2&&>(rhs))) {
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
#include "func/function.h"
#include "func/invoke.h"
#include "func/reference_wrapper.h"
#endif //TEST_TYPE_ERASURE_H
