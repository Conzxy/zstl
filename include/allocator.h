#ifndef TINYSTL_ALLOCATOR_H
#define TINYSTL_ALLOCATOR_H

//空间配置器
//以变量为单元进行分配
#include "alloc.h"
#include "stl_construct.h"
#include "stl_utility.h"
#include "type_traits.h"
#include <new>

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
		
		template<typename U>
		struct Rebind{
			using type = allocator<U>;
		};

		template<typename U>
		using rebind = typename Rebind<U>::type;
    public:
        static T* allocate();
        static T* allocate(size_t n);
        static void deallocate(T* ptr);
        static void deallocate(T* ptr,std::size_t);

        static void construct(T* ptr);

        /*template<typename T1,typename T2>
        static void construct(T1* ptr,T2 const& value);*/
	
        static void construct(T* ptr,T const& value);

        template<typename...Args>
        static void construct(T* ptr,Args&&... args);

        static void destroy(T* ptr);
        static void destroy(T* first,T* last);
    };

    template<typename T>
    T* allocator<T>::allocate(){
        return static_cast<T*>(::operator new(sizeof(T)));
    }

    template<typename T>
    T* allocator<T>::allocate(size_t n){
        return static_cast<T*>(::operator new(sizeof(T)*n));
    }

    template<typename T>
    void allocator<T>::deallocate(T *ptr) {
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::deallocate(T* ptr,size_t n){
        ::operator delete(ptr);
    }

    template<typename T>
    void allocator<T>::construct(T *ptr){
        ::new(ptr) T();
    }

    template<typename T>
    void allocator<T>::construct(T* ptr,T const& value){
        TinySTL::construct(ptr,value);              //作用域限定以防递归
    }

    template<typename T>
    template<typename...Args>
    void allocator<T>::construct(T* ptr,Args&&... args){
        TinySTL::construct(ptr,TinySTL::forward<Args>(args)...);
    }

    template<typename T>
    void allocator<T>::destroy(T *ptr) {
        TinySTL::destroy(ptr);
    }

    template<typename T>
    void allocator<T>::destroy(T *first,T *last) {
        TinySTL::destroy(first,last);
    }

    template<typename allocator,typename =TinySTL::Void_t<>>
    struct allocator_traits {
    };

    template<typename allocator>
    struct allocator_traits<allocator, TinySTL::Void_t<
                                                        typename allocator::value_type,
                                                        typename allocator::pointer,
                                                        typename allocator::reference,
                                                        typename allocator::const_pointer,
                                                        typename allocator::const_reference,
                                                        typename allocator::difference_type,
                                                        typename allocator::size_type>>
    {
        using value_type        =typename allocator::value_type;
        using pointer           =typename allocator::pointer;
        using reference         =typename allocator::reference;
        using const_pointer     =typename allocator::const_pointer;
        using const_reference   =typename allocator::const_reference;
        using difference_type   =typename allocator::difference_type;
        using size_type         =typename allocator::size_type;
    };
}

#endif //TINYSTL_ALLOCATOR_H
