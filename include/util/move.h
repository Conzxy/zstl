#ifndef TINYSTL__MOVE_H
#define TINYSTL__MOVE_H

#include <type_traits>

namespace zxy{
    //Remove_reference_t是为了避开引用折叠
    //通过static_cast<&&>使之强制获得右值性
    template<typename T>
    constexpr decltype(auto) move(T&& t) noexcept{
        return static_cast<std::remove_reference_t<T>&&>(t);
    }

    //Remove_reference_t是为了避开引用折叠
    //forward是根据值类别有选择性使之获得右值性，从而实现perfect forwarding
    template<typename T>
    constexpr T&& forward(std::remove_reference_t<T>& t) noexcept{
        return static_cast<T&&>(t);
    }//overload 1

    //rvalue->lvalue，触发static_assert
    template<typename T>
    constexpr T&& forward(std::remove_reference_t<T>&& t) noexcept{
        static_assert(!std::is_lvalue_reference<T>::value,
                "template argument substituting T is an lvalue reference type");

        return static_cast<T&&>(t);
    }//overload 2
}
#endif //TINYSTL__MOVE_H
