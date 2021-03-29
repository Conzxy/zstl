#ifndef _USER_ALLOCATOR_H
#define _USER_ALLOCATOR_H

#include <alloc.h>

namespace TinySTL{
    template<typename T,typename Alloc=alloc>
    class UserAllocator{
    public:
        static T* allocate(size_t n=1){
            return n == 0 ? 0 : Alloc::allocate(sizeof(T)*n);
        }

        static void deallocate(T* ptr,size_t n=1){
            if(n != 0){
                Alloc::deallocate(ptr,n*sizeof(T));
            }
        }
    };
}


#endif