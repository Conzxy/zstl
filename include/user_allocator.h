#ifndef _USER_ALLOCATOR_H
#define _USER_ALLOCATOR_H

#include <alloc.h>

namespace TinySTL{
    template<typename T,typename Alloc=alloc>
    class UserAllocator{
    public:
        template<typename ...Args>
        static void construct(T* ptr,Args&&... args){
            TinySTL::construct(ptr,TinySTL::forward<Args>(args)...);
        }

        static void construct(T* ptr){
            ::new(ptr) T();
        }

        static void destroy(T* ptr){
            TinySTL::destroy(ptr);
        }

        static void destroy(T* first,T* last){
            TinySTL::destroy(first,last);
        }

        static T* allocate(size_t n=1){
            return n == 0 ? 0 : (T*)Alloc::allocate(sizeof(T)*n);
        }

        static void deallocate(T* ptr,size_t n=1){
            if(n != 0){
                Alloc::deallocate(ptr,n*sizeof(T));
            }
        }
    };
}


#endif