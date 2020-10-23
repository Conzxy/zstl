#ifndef TINYSTL_ALLOCATOR_H
#define TINYSTL_ALLOCATOR_H

//空间配置器
//以变量为单元进行分配
#include "alloc.h"
#include "stl_construct.h"
#include "stl_utility.h"

namespace TinySTL{
    template<typename T>
    class allocator{
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef std::size_t size_type;
        typedef ptrdiff_t   difference_type;
    public:
        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T* ptr);
        static void deallocate(T* ptr,std::size_t);

        static void construct(T* ptr);

        /*template<typename T1,typename T2>
        static void construct(T1* ptr,T2 const& value);*/

        static void construct(T* ptr,T const& value);

        /*template<typename...Args>
        static void construct(T* ptr,Args&&... args);*/

        static void destroy(T* ptr);
        static void destroy(T* first,T* last);
    };

    template<typename T>
    T* allocator<T>::allocate(){
        return static_cast<T*>(alloc::allocate(sizeof(T)));
    }

    template<typename T>
    T* allocator<T>::allocate(size_t n){
        return static_cast<T*>(alloc::allocate(sizeof(T)*n));
    }

    template<typename T>
    void allocator<T>::deallocate(T *ptr) {
        alloc::deallocate(ptr,sizeof(T));
    }

    template<typename T>
    void allocator<T>::deallocate(T* ptr,size_t n){
        alloc::deallocate(ptr,sizeof(T)*n);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr){
        ::new(ptr) T();
    }

    template<typename T>
    void allocator<T>::construct(T* ptr,T const& value){
        TinySTL::construct(ptr,value);              //作用域限定以防递归
    }

    /*template<typename T>
    template<typename...Args>
    void allocator<T>::construct(T* ptr,Args&&... args){
        TinySTL::construct(ptr,TinySTL::forward<Args>(args)...);
    }*/

    template<typename T>
    void allocator<T>::destroy(T *ptr) {
        TinySTL::destroy(ptr);
    }

    template<typename T>
    void allocator<T>::destroy(T *first,T *last) {
        TinySTL::destroy(first,last);
    }

    template<typename T>
    struct allocator_traits{
        using value_type        =typename allocator<T>::value_type;
        using pointer           =typename allocator<T>::pointer;
        using reference         =typename allocator<T>::reference;
        using const_pointer     =typename allocator<T>::const_pointer;
        using const_reference   =typename allocator<T>::const_reference;
        using difference_type   =typename allocator<T>::difference_type;
        using size_type         =typename allocator<T>::size_type;
    };
}

#endif //TINYSTL_ALLOCATOR_H
