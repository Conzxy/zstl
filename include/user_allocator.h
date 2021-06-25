#ifndef _USER_ALLOCATOR_H
#define _USER_ALLOCATOR_H

#include "alloc.h"

namespace TinySTL{
	/*
	 * @class ByteAllocator
	 * @brief a simple allocator in bytes
	 */
	class ByteAllocator{
	public:
		static void* allocate(size_t bytes){
			return ::operator new(bytes);
		}

		static void deallocate(void* ptr){
			::operator delete(ptr);
		}
	};
	
	/*
	 * @class UserAllocator
	 * @brief low level adapter of ByteAllocator
	 *
	 * @code
	 * template<typename T, typename Alloc=<ByteAllocator>>
	 * class deque{
	 * //...
	 * typedef UserAllocator<T, Alloc> node_allocator;
	 * typedef UserAllocator<T*, Alloc> map_allocator;
	 * //...
	 * };
	 * @endcode
	 *
	 * now, TinySTL::allocator have a member type alias template named "rebind"
	 * @code
	 * template<typename T, typename Alloc=STL_ allocator>
	 * class deque{
	 * //...
	 * typedef Alloc data_allocator;
	 * typedef typename Alloc::template rebind<T*> map_allocator;
	 * //...
	 * };
	 * @endcode
	 */
    template<typename T, typename Alloc=ByteAllocator>
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
