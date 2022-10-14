// allocator interface
// requires:
// allocator need provide non-static member:
// -- T* allocate(size_t n = 1)
// -- void deallocate(T* ptr, size_t)
// if you have a static class(i.e. allocate and deallocate are static)
// you should inherit from this class in proteced(EBCO)
//
// FIXME remove others interfaces except for allocate and deallocate
#ifndef ZSTL_ALLOCATOR_H
#define ZSTL_ALLOCATOR_H

#include "stl_construct.h"
#include "stl_utility.h"
#include "type_traits.h"
#include <new>

namespace zstl{
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
        T* allocate(size_t n=1) const {
            return static_cast<T*>(::operator new(sizeof(T)*n));
        }

        void deallocate(T* ptr,std::size_t=1) const {
            ::operator delete(ptr);
        }

        template<typename...Args, typename U>
        void construct(U* ptr,Args&&... args) const {
            zstl::construct(ptr,zstl::forward<Args>(args)...);
        }

        template<typename U>
        void destroy(U* ptr) const {
            zstl::destroy(ptr);
        }

        template<typename U>
        void destroy(U* first,U* last) const {
            zstl::destroy(first,last);
        }
    };

    template<typename Alloc>
    struct allocator_traits {
        using allocator_type = Alloc;
        using pointer = typename Alloc::pointer;
        using reference = typename Alloc::reference;
        using const_pointer = typename Alloc::const_pointer;
        using const_reference = typename Alloc::const_reference;
        using size_type = typename Alloc::size_type;
        using difference_type = typename Alloc::difference_type;
        using value_type = typename Alloc::value_type;

        template<typename U>
        using rebind = typename Alloc::template rebind<U>;
	
        inline static pointer allocate(allocator_type& alloc, size_type n=1){
            return alloc.allocate(n);
        }

        inline static void deallocate(allocator_type& alloc, pointer ptr
                                    , size_type n=1){
            alloc.deallocate(ptr, n);
        }
 
        template<typename... Args, typename U>
        inline static void construct(allocator_type& alloc, U* ptr, Args&&... args){
            alloc.construct(ptr, zstl::forward<Args>(args)...);
        }

        template<typename U>
        inline static void destroy(allocator_type& alloc, U* ptr){
            alloc.destroy(ptr);
        }

        template<typename U>
        inline static void destroy(allocator_type& alloc, U* first, U* last){
            alloc.destroy(first, last);
        }
    };

}//namespace zstl

#endif //ZSTL_ALLOCATOR_H
