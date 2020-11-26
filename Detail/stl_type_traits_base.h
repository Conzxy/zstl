#ifndef TINYSTL_STL_TYPE_TRAITS_BASE_H
#define TINYSTL_STL_TYPE_TRAITS_BASE_H

namespace TinySTL{
    //以下是供predicate用的基类
    template<typename T,T val>
    struct Integral_constant{
        static constexpr T value=val;
        using value_type=T;
        using type=Integral_constant;
        constexpr operator value_type()const noexcept{
            return value;
        }
        constexpr value_type operator()()const noexcept{
            return value;
        }
    };

    template<bool val>
    using Bool_constant=Integral_constant<bool,val>;

    using _true_type=Bool_constant<true>;
    using _false_type=Bool_constant<false>;

    //以下用于类型特性过滤
    template<typename T>
    struct _type_traits{
        typedef _false_type has_trivially_default_constructor;
        typedef _false_type has_trivially_copy_constructor;
        typedef _false_type has_trivially_assignment_operator;
        typedef _false_type has_trivially_destructor;
        typedef _false_type is_POD_type;
    };

    template<>
    struct _type_traits<int>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned int>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<short>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned short>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<double>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<float>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<char>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned char>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<unsigned long>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long long>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<long double>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<wchar_t>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<typename T>
    struct _type_traits<T*>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<typename T>
    struct _type_traits<T const*>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<char*>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };

    template<>
    struct _type_traits<char const*>{
        typedef _true_type has_trivially_default_constructor;
        typedef _true_type has_trivially_copy_constructor;
        typedef _true_type has_trivially_assignment_operator;
        typedef _true_type has_trivially_destructor;
        typedef _true_type is_POD_type;
    };
}
#endif //TINYSTL_STL_TYPE_TRAITS_BASE_H
