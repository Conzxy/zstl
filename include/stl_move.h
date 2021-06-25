#ifndef TINYSTL_STL_MOVE_H
#define TINYSTL_STL_MOVE_H

#include "type_traits.h"

namespace TinySTL{
    //Remove_reference_t是为了避开引用折叠
    //通过static_cast<&&>使之强制获得右值性
    template<typename T>
    constexpr decltype(auto) move(T&& t) noexcept{
        return static_cast<Remove_reference_t<T>&&>(t);
    }

    //Remove_reference_t是为了避开引用折叠
    //forward是根据值类别有选择性使之获得右值性，从而实现perfect forwarding
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>& t) noexcept{
        return static_cast<T&&>(t);
    }//overload 1

    //rvalue->lvalue，触发static_assert
    template<typename T>
    constexpr T&& forward(Remove_reference_t<T>&& t) noexcept{
        static_assert(!Is_lvalue_reference_v<T>,
                "template argument substituting T is an lvalue reference type");

        return static_cast<T&&>(t);
    }//overload 2
}
#endif //TINYSTL_STL_MOVE_H
