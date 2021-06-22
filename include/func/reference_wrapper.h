#pragma once


#include "../type_traits.h"
#include <memory>

namespace TinySTL{
    namespace detail{
        template<typename T>
        constexpr T& try_lref(T& t) noexcept { return t; }
        template<typename T>
        void try_lref(T&& t)=delete;
    }

    template<typename T>
    class reference_wrapper
    {
    public:
        typedef T                   type;
        typedef reference_wrapper   self;

        template<typename U,
                typename =decltype(
                    detail::try_lref<T>(TinySTL::declval<U>())),
                TinySTL::Enable_if_t<!Is_same_v<self,Remove_cv_t<Remove_reference_t<U>>>,int> =0
                >             
        reference_wrapper(U&& val)
            :ref_{std::addressof(val)}
        {}

        reference_wrapper(self const&)=default;
        reference_wrapper& operator=(self const&)=default;

        constexpr operator T& () const noexcept { return *ref_; }
        constexpr T& get() const noexcept { return *ref_; }
        
    private:
        T* ref_;
    };
}