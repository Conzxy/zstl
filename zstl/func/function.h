//CLASS TEMPLATE function
//use type erasure to make function to accept all callables
#ifndef ZSTL_FUNCTION_H
#define ZSTL_FUNCTION_H

#include <exception>

#include "invoke.h"

namespace zstl{
    // SFINAE predicate
    template<typename T>
    class IsEqualComparable {
    private:
        static void* conv(bool);
        template<typename U>
        static _true_type test(decltype(conv(zstl::declval<U const&>()==zstl::declval<U const&>())),
            decltype(conv(!(zstl::declval<U const&>()==zstl::declval<U const&>()))));
        template<typename >
        static _false_type test(...);
    public:
        static constexpr bool value=decltype(test<T>(nullptr, nullptr))::value;
    };
    
    // Because class template support partial specilization
    // and it is also a SFINAE context, we can just use bool
    // non-type template parameter instead of Enable_if_t to
    // remove non-disired specilization
    template<typename T, bool=IsEqualComparable<T>::value>
    class TryEquals {
    public:
        static bool equals(T const& x, T const& y) {
            return x==y;
        }
    };

    struct NotEqualityComparable :public std::exception {};

    template<typename T>
    class TryEquals<T, false> {
    public:
        static bool equals(T const& , T const& ) {
            throw NotEqualityComparable();
        }
    };
  
    // User desire to use zstl::function as following:
    // zstl::function<void()> thread_func = ...;
    //
    // But we must write this template as following since we
    // want accept all callable including lambda expression:
    // template<typename Functor, typename R, typename... Args>
    //
    // Here, we use a technique called "Type Erasure": Depend
    // on the virtual function to make derived class store the
    // Functor and implemetation specific function.
    //
    // template<typename R, typename... Args> this is base class.
    // template<typename Functor, typename R, typename... Args> 
    // this is derived class.
    //
    // Then, used can just provide a signature type, we split it
    // to R and Args, and construct base class object, then call
    // according virtual function.
    //
    // Same technique is also applied in boost::any and std::shared_ptr
    // (I guess since I don't see the source code of shared_ptr)
    template<typename R, typename... Args>
    class FunctorBridge {
    public:
        virtual ~FunctorBridge() {}
        virtual FunctorBridge* clone()const=0;
        virtual R invoke(Args... args)const=0;
        virtual bool equals(FunctorBridge const* fb)const=0;
    };

    template<typename Functor, typename R, typename... Args>
    class SpecificFunctorBridge :public FunctorBridge<R, Args...> {
        Functor functor;
    public:
        template<typename FunctorFwd>
        SpecificFunctorBridge(FunctorFwd&& functor)
            : functor(zstl::forward<FunctorFwd>(functor)) {
        }

        virtual SpecificFunctorBridge* clone()const override {
            return new SpecificFunctorBridge(functor);
        }

        virtual R invoke(Args... args)const override {
            return zstl::invoke(Functor(functor),zstl::forward<Args>(args)...);
        }

        virtual bool equals(FunctorBridge<R, Args...> const* fb)const override {
            if (auto specFb=dynamic_cast<SpecificFunctorBridge const*>(fb))
                return TryEquals<Functor>::equals(functor, specFb->functor);
            else
                return false;
        }
    };

    template<typename Signature>
    class function;

    template<typename R, typename... Args>
    class function<R(Args...)> {
    private:
        FunctorBridge<R, Args...>* bridge;
    public:
        function() :bridge(nullptr) {}
        function(function const& other) :bridge(nullptr) {
            if (other.bridge) {
                bridge=other.bridge->clone();
            }
        }

        function(function& other)
            : function(static_cast<function const&>(other)) {
        }

        function(function&& other) :bridge(other.bridge) {
            other.bridge=nullptr;
        }

        template<typename F>
        function(F&& f):bridge(nullptr) {
            using Functor=Decay_t<F>;
            using Bridge=SpecificFunctorBridge<Functor, R, Args...>;
            bridge=new Bridge(zstl::forward<F>(f));
        }

        function& operator=(function const& other) {
            function tmp(other);
            swap(*this, tmp);
            return *this;
        }

        function& operator=(function&& other) {
            delete bridge;
            bridge=other.bridge;
            other.bridge=nullptr;
            return *this;
        }

        template<typename F>
        function& operator=(F&& f) {
            function tmp(std::forward<F>(f));
            swap(*this, tmp);
            return *this;
        }

        ~function() {
            delete bridge;
        }

        friend void swap(function& f1, function& f2) {
            std::swap(f1.bridge, f2.bridge);
        }

        explicit operator bool()const {
            return bridge!=nullptr;
        }

        //invocation
        R operator()(Args... args)const {
            return bridge->invoke(std::forward<Args>(args)...);
        }

        friend bool operator==(function const& f1, function const& f2) {
            if (!f1||!f2) {
                return !f1&&!f2;
            }
            else
                return f1.bridge->equals(f2.bridge);
        }

        friend bool operator!=(function const& f1, function const& f2) {
            return !(f1==f2);
        }
    };
} // namespace zstl

#endif // ZSTL_FUNCTION_H
