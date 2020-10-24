#ifndef TINYSTL_STL_TYPE_TRAITS_H
#define TINYSTL_STL_TYPE_TRAITS_H

//该文件实现了一部分标准库traits（×），templates练习（√）
#include <cstddef>
#include <type_traits>
#include "stl_type_traits_base.h"

namespace TinySTL{

    //以下模板用于去除引用
    template<typename T>
    struct Remove_reference{
        using type=T;
    };

    template<typename T>
    struct Remove_reference<T&>{
        using type=T;
    };

    template<typename T>
    struct Remove_reference<T&&>{
        using type=T;
    };

    template<typename T>
    using Remove_reference_t=typename Remove_reference<T>::type;

    //以下模板用于添加左值引用
    template<typename T>
    struct Add_lvalue_reference{
        using type=T&;
    };

    //void进行特化
    template<>
    struct Add_lvalue_reference<void>{
        using type=void;
    };

    template<>
    struct Add_lvalue_reference<void const>{
        using type=void const;
    };

    template<>
    struct Add_lvalue_reference<void volatile>{
        using type=void volatile;
    };

    template<>
    struct Add_lvalue_reference<void const volatile>{
        using type=void const volatile;
    };

    template<typename T>
    using Add_lvalue_reference_t=typename Add_lvalue_reference<T>::type;

    //以下模板用于添加右值引用
    template<typename T>
    struct Add_rvalue_reference{
        using type=T&&;
    };

    //对void进行特化
    template<>
    struct Add_rvalue_reference<void>{
        using type=void;
    };

    template<>
    struct Add_rvalue_reference<const void>{
        using type=void;
    };

    template<>
    struct Add_rvalue_reference<volatile void>{
        using type=void;
    };

    template<>
    struct Add_rvalue_reference<const volatile void>{
        using type=void;
    };

    template<typename T>
    using Add_rvalue_reference_t
    =typename Add_rvalue_reference<T>::type;


    //以下模板用于去除cv
    template<typename T>
    struct Remove_const{
        using type=T;
    };

    template<typename T>
    struct Remove_const<T const>{
        using type=T;
    };

    template<typename T>
    using Remove_const_t=typename Remove_const<T>::type;

    template<typename T>
    struct Remove_volatile{
        using type=T;
    };

    template<typename T>
    struct Remove_volatile<T volatile>{
        using type=T;
    };

    template<typename T>
    using Remove_volatile_t=typename Remove_volatile<T>::type;

    template<typename T>
    struct Remove_cv: Remove_const<Remove_volatile_t<T>>
    {};

    //template<typename T>
    //using Remove_cv_t=typename Remove_cv<T>::type;

    template<typename T>
    using Remove_cv_t=Remove_const_t<Remove_volatile_t<T>>;

    //以下模板用于退化（decay）
    //注意，这里必须先去引用再去cv，不然低层const去不掉（不针对指针）
    template<typename T>
    struct Decay:Remove_cv<Remove_reference_t<T>>{
    };

    template<typename T>
    struct Decay<T[]>{
        using type=T*;
    };

    template<typename T>
    struct Decay<T(&)[]>{
        using type=T*;
    };

    //数组特化
    template<typename T,std::size_t N>
    struct Decay<T[N]>{
        using type=T*;
    };

    template<typename T,std::size_t N>
    struct Decay<T(&)[N]>{
        using type=T*;
    };

    //函数特化
    template<typename R,typename...Args>
    struct Decay<R(Args...)>{
        using type=R(*)(Args...);
    };

    //c-style varargs
    template<typename R,typename...Args>
    struct Decay<R(Args...,...)>{
        using type=R(*)(Args...,...);
    };

    template<typename T>
    using Decay_t=typename Decay<T>::type;

    /*template<typename T1,typename T2>
    struct Is_same{
        static constexpr bool value=false;
    };

    template<typename T>
    struct Is_same<T,T>{
        static constexpr bool value=true;
    };

    template<typename T1,typename T2>
    constexpr bool Is_same_v=Is_same<T1,T2>::value;*/


    //dummy object
    template<typename T>
    Add_rvalue_reference_t<T> declval()noexcept;

    //predicate
    //以下用于判断类型是否相同
    template<typename T1,typename T2>
    struct Is_same:_false_type{
    };

    template<typename T>
    struct Is_same<T,T>:_true_type{
    };

    template<typename T1,typename T2>
    using Is_same_t=typename Is_same<T1,T2>::type;

    template<typename T1,typename T2>
    constexpr bool Is_same_v=Is_same<T1,T2>::value;

    template<typename T>
    struct Is_lvalue_reference:_false_type{};

    template<typename T>
    struct Is_lvalue_reference<T&>:_true_type{};

    template<typename T>
    constexpr bool Is_lvalue_reference_v=Is_lvalue_reference<T>::value;

    //SFINAE
    /* overloads version
     * template<typename T>
    struct Is_default_constructible_Helper{
    private:
        template<typename U,typename =decltype(U())>
        static _true_type test(void*);

        template<typename>
        static _false_type test(...);
    public:
        using type=decltype(test<T>(nullptr));
    };

    template<typename T>
    struct Is_default_constructible:Is_default_constructible_Helper<T>::type{};

    template<typename T>
    constexpr bool Is_default_constructible_v=Is_default_constructible<T>::value;
    */
    template<bool B,typename T=void>
    struct Enable_if;

    template<typename T>
    struct Enable_if<true,T>{
        using type=T;
    };

    template<bool B,typename T=void>
    using Enable_if_t=typename Enable_if<B,T>::type;

    template<typename...>
    using Void_t=void;

    template<typename T>
    struct Is_destructible_Helper{
    private:
        //declval<U&>().~U()
        //express p->~U(),i.e. (*p).~U() requiring p is a lvalue
        template<typename U,typename=decltype(declval<U&>().~U())>
        static _true_type test(void*);
        template<typename>
        static _false_type test(...);
    public:
        using type=decltype(test<T>(nullptr));
    };

    template<typename T>
    struct Is_destructible:Is_destructible_Helper<T>::type{};

    template<typename T>
    constexpr bool Is_destructible_v=Is_destructible<T>::value;

    //generic lambda version
    //helper:check validity of f(args...) for F f and Args...args
    template<typename F,typename...Args,
            typename=decltype(declval<F>()(declval<Args&&>()...))>
    _true_type Is_valid_impl(void*);

    template<typename F,typename...Args>
    //template<typename ...args>
    _false_type Is_valid_impl(...);

    //inline是为了效率，这里constexpr有const语义，所以是internal linkage
    //其实除了第一个&&，declval的&&和decltype(Args)&&...的&&去了也没有影响，因为：
    //假如Args是左值，那么auto推断为T&，折叠为&，而Args是右值，那么auto推断为T，折叠为&&（T为实参的声明类型）
    //我想这里的&&可能起提示作用，全部设置为universal reference可能是个不错的习惯（编程者的习惯）
    inline constexpr
    auto Is_valid=[](auto f) {      //traits factory
                    return [](auto &&... Args) {
                            return decltype(Is_valid_impl<decltype(f),
                                                          decltype(Args)&& ...
                                                          >(nullptr)){};
        };
    };

    //helper template to present a type as a value
    template<typename T>
    struct typeT{
        using type=T;
    };

    //helper tp wrap a type as a value
    template<typename T>
    constexpr auto type=typeT<T>{};

    //helper to unwrap a wrapped type in unevaluated context
    template<typename T>
    T valueT(typeT<T>);  //no definition needed

    //callable object
    constexpr auto Is_default_constructible
     =Is_valid([](auto x)->decltype((void)decltype(valueT(x))()){
     });
    //substitution Is_valid's definition
    /*
     * constexpr auto Is_default_constructible
     *  =[](auto&&... Args){
     *      return decltype(
     *                Is_valid_impl<
     *                      decltype([](auto x)
     *                                  ->decltype((void)decltype(valueT(x))())),
     *                      decltype(Args)&&...
     *                      >(nullptr)){};
     *   };
     */

    template<typename T>
    constexpr bool Is_default_constructible_v
     =decltype(Is_default_constructible(type<T>))::value;

    template<typename from,typename to>
    struct Is_convertible_helper{
    private:
        static void aux(to);
        template<typename F,
                typename =decltype(aux(declval<F>()))>
        static _true_type test(void*);
        template<typename,typename>
        static _false_type test(...);
    public:
        using type=decltype(test<from>(nullptr));
    };

    template<typename from,typename to>
    struct Is_convertible: Is_convertible_helper<from,to>::type{};

    template<typename from,typename to>
    constexpr bool Is_convertible_v=Is_convertible<from,to>::value;

}

#endif //TINYSTL_STL_TYPE_TRAITS_H
