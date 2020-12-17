#ifndef TINYSTL_STL_CONSTRUCT_H
#define TINYSTL_STL_CONSTRUCT_H

//该头文件主要包含两个函数：
//construct：负责对象的构造
//destroy：负责对象的析构
#include <new>
#include <type_traits.h>
#include <stl_iterator.h>
#include <_move.h>

namespace TinySTL {
    template<typename T1, typename T2>
    inline void construct(T1 *ptr1, T2 const &value) {
        //::new((void*)ptr1) T1(value);
        ::new(ptr1) T1(value);  //placement new
    }

    //实际上，由于通用引用重载过于“贪婪”，上面这个重载基本不会被选择
    template<typename T1,typename...Args>
    inline void construct(T1* ptr1,Args&&... args){
        ::new(ptr1) T1(TinySTL::forward<Args>(args)...);
    }

    template<class T>
    inline void destroy_single(T *ptr, _false_type) {
        if (ptr != nullptr)
            ptr->~T();
    }

//
    template<typename T>
    inline void destroy_single(T *ptr, _true_type) {
    }

    template<typename T>
    inline void destroy(T *ptr) {
        destroy_single(ptr, typename _type_traits<T>::has_trivially_destructor{});
    }

    template<class ForwardIterator>
    inline void destroy_cat(ForwardIterator first, ForwardIterator last, _false_type) {//cat:catenate
        for (; first != last; ++first) {
            destroy(&*first);
        }
    }

    template<typename ForwardIterator>
    inline void destroy_cat(ForwardIterator first, ForwardIterator last, _true_type) {
    }

    template<class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        using has_trivially_destructor = typename _type_traits<
                                            typename iterator_traits<
                                            ForwardIterator>::value_type>
                                            ::has_trivially_destructor;
        destroy_cat(first, last, has_trivially_destructor{});
    }

    /*
    template<>
    inline void destroy<char*>(char*,char*);

    template<>
    inline void destroy<wchar_t*>(wchar_t*,wchar_t*);
    */
}

#endif //TINYSTL_STL_CONSTRUCT_H
