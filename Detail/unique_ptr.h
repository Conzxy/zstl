#ifndef TINYSTL_UNIQUE_PTR_H
#define TINYSTL_UNIQUE_PTR_H

#include <stl_utility.h>
#include <type_traits.h>

namespace TinySTL{
    template<typename T>
    struct default_delete{
        constexpr default_delete()noexcept=default;

        //allow U* is implicitly convertible to T*
        //but doesn't ensure this is safe
        template<typename U,typename =typename
                Enable_if<Is_convertible<U*,T*>::value>::type>
        default_delete(default_delete<U> const& other)noexcept{}

        void operator()(T* ptr){
            static_assert(!Is_void<T>::value,
                          "can't delete pointer to incomplete type!");
            static_assert(sizeof(T)>0,
                          "can't delete pointer to incomplete type!");
            if(ptr) delete ptr;
        }
    };

    template<typename T>
    struct default_delete<T[]>{
        constexpr default_delete()noexcept=default;

        void operator()(T* ptr){
            static_assert(sizeof(T)>0,
                          "can't delete pointer to incomplete type!");
            if(ptr) delete[]ptr;
        }

        //if U is derived from T,delete[](U*) is unsafe
        template<typename U> void operator()(U*)const=delete;
    };

    template<typename T,typename D>
    class unique_ptr_impl{
        //if D has pointer alias,pointer type=D(non-ref)::pointer
        // SFINAE by partial specialization
        template<typename U,typename E,typename =Void_t<>>
        struct _ptr{
            using type=U*;
        };

        template<typename U,typename E>
        struct _ptr<U,E,Void_t<typename
                                Remove_reference_t<E>::pointer>>{
            using type=typename Remove_reference_t<E>::pointer;
        };
    public:
        //D must be callable object,such as function object/function pointer/reference to function/lambda expression
        //D must be default-constructible
        using pointer=typename _ptr<T,D>::type;

        unique_ptr_impl()=default;
        unique_ptr_impl(pointer p):data(p){}

        //if d is rvalue,d must be moveconstructible
        //if d is lvalue,d must be copyconstructible
        template<typename Del>
        unique_ptr_impl(pointer p,Del&& d)
        :data(p),deleter(TinySTL::forward<Del>(d)){}

        //因为pointer可能会被用来迭代，故const版本不返回const pointer&
        //又由于bitwise constness，故返回pointer
        pointer  M_ptr() const { return data; }
        pointer& M_ptr() { return data; }
        D const& M_deleter() const { return deleter; }
        D&       M_deleter() { return deleter; }
    private:
        pointer data;
        D       deleter;
    };

    //unique_ptr of a single object
    template<typename T,typename D=default_delete<T>>
    class unique_ptr{
    public:
        //type:
        using pointer           =typename unique_ptr_impl<T,D>::pointer;
        using element_type      =T;
        using deleter_type      =D;
    public:
        //constructors:

        //deleter will be value initialized
        constexpr unique_ptr()noexcept
        :M_t(){}    //default

        explicit unique_ptr(pointer p)noexcept
        :M_t(p){}


        //custom deleter version:

        //if deleter_type=A&
        //signature:unique_ptr(pointer p,D&)
        //if deleter_type=A const&/A(non-ref)
        //signature:unique_ptr(pointer p,D const&)
        unique_ptr(pointer p,typename Conditional<
                    Is_reference<deleter_type>::value,
                    deleter_type,deleter_type const&>::type d)noexcept
        :M_t(p,d){}

        //delete_type is not a reference type
        unique_ptr(pointer p,Remove_reference_t<deleter_type>&& d)noexcept
        :M_t(TinySTL::move(p),TinySTL::move(d)){
            static_assert(!Is_reference<deleter_type>::value,
                          "rvalue deleter bound to reference");
        }

        //move constructor
        //if delete_type is reference type,this deleter is copy constructed from up's deleter
        //otherwise,this deleter is move constructed from up's deleter
        unique_ptr(unique_ptr&& up)noexcept
        :M_t(up.release(),TinySTL::forward<deleter_type>(up.get_deleter())){}

        //nullptr version
        constexpr unique_ptr(std::nullptr_t)noexcept
        :unique_ptr(){}

        //converting constructor
        template<typename U,typename E>
        unique_ptr(unique_ptr<U,E>&& up)noexcept
        :M_t(up.release(),TinySTL::forward<E>(up.get_deleter())){}

        //destructor:
        ~unique_ptr(){
            auto& ptr=M_t.M_ptr();
            if(ptr!=nullptr)
                deleter_type()(ptr);
            ptr=pointer();
        }

        //assignment:
        unique_ptr& operator=(unique_ptr&& up)noexcept{
            reset(up.release());
            get_deleter()=TinySTL::forward<deleter_type>(up.get_deleter());
            return *this;
        }

        //allow  transfer ownership from other convertible pointer
        template<typename U,typename E>
        unique_ptr& operator=(unique_ptr<U,E>&& up)noexcept{
            reset(up.release());
            get_deleter()=TinySTL::forward<E>(up.get_deleter());
            return *this;
        }

        unique_ptr& operator=(std::nullptr_t)noexcept{
            reset();
            return *this;
        }
        //observers:
        Add_lvalue_reference_t<T> operator*()const{
            if(get()!=nullptr)
                return *get();
        }

        pointer operator->()const noexcept{
            if(get()!=nullptr)
                return get();
        }

        pointer get()const noexcept{
            return M_t.M_ptr();
        }

        deleter_type& get_deleter()noexcept{
            return M_t.M_deleter();
        }

        deleter_type const& get_deleter()const noexcept{
            return M_t.M_deleter();
        }

        explicit operator bool()const noexcept{
            return get()!=nullptr;
        }

        //modifiers:
        pointer release()noexcept{
            pointer tp=get();
            M_t.M_ptr()=pointer();
            return tp;
        }

        void reset(pointer p=pointer())noexcept{
            //swap and destroy
            using std::swap;
            swap(M_t.M_ptr(),p);

            if(p!=nullptr)
                get_deleter()(p);
        }

        void swap(unique_ptr const& rhs)noexcept{
            std::swap(M_t,rhs.M_t);
        }


        //unique_ptr is a only move pointer,prohibiting copy
        unique_ptr(unique_ptr const&)=delete;
        unique_ptr& operator=(unique_ptr const&)=delete;
    private:
        unique_ptr_impl<T,D> M_t;
    };

    template<typename T,typename D>
    void swap(unique_ptr<T,D>& lhs,unique_ptr<T,D>& rhs){
        lhs.swap(rhs);
    }

    //make function:
    template<typename T,typename... paras>
    unique_ptr<T> make_unique(paras&&... args){
        return unique_ptr<T>(new T(TinySTL::forward<paras>(args)...));
    }

    //compare function:
    template<typename T1,typename D1,
             typename T2,typename D2>
    inline bool operator==(unique_ptr<T1,D1> const& x,unique_ptr<T2,D2> const& y){
        return x.get()==y.get();
    }

    template<typename T1,typename D1,
             typename T2,typename D2>
    inline bool operator!=(unique_ptr<T1,D1> const& x,unique_ptr<T1,D1> const& y){
        return !(x==y);
    }

    template<typename T,typename D>
    inline bool operator==(unique_ptr<T,D> const& x,std::nullptr_t)noexcept{
        return !x;
    }

    template<typename T,typename D>
    inline bool operator==(std::nullptr_t,unique_ptr<T,D> const& x)noexcept{
        return !x;
    }

    template<typename T,typename D>
    inline bool operator!=(unique_ptr<T,D> const& x,std::nullptr_t)noexcept{
        return (bool)x;
    }

    template<typename T,typename D>
    inline bool operator!=(std::nullptr_t,unique_ptr<T,D> const& x)noexcept{
        return (bool)x;
    }

    //common_type  less<>
    /*template<typename T1,typename D1,
             typename T2,typename D2>
    inline bool operator<(unique_ptr<T1,D1> const& x,unique_ptr<T2,D2> const& y){
        using CT=typename common_type<typename unique_ptr<T1,D1>::pointer,
                                      typename unique_ptr<T2,D2>::pointer>::type;
        return less<CT>()(x.get(),y.get());
    }*/
}
#endif //TINYSTL_UNIQUE_PTR_H
