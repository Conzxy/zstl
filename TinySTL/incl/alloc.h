#ifndef TINYSTL_ALLOC_H
#define TINYSTL_ALLOC_H

#include <cstddef>
#include <cstdlib>
//空间配置器，以字节为单位分配
namespace TinySTL{
    class alloc{
    private:
        static std::size_t const ALIGN=8;
        static std::size_t const MAX_BYTES=128;
        static std::size_t const NFREELISTS=MAX_BYTES/ALIGN;
    private:
        union obj{
            union obj* next;
            char client[1]; //起提示作用：表示指向一个实际区块，无实际用途
            //不过还有一种说法是为了避免丑陋的强制转换，有道理，但实际没用
        };

        static obj* free_list[NFREELISTS];

        static char* start_free;
        static char* end_free;
        static std::size_t heap_size;
    private:
        //将bytes上调至8的倍数
        static std::size_t ROUND_UP(std::size_t bytes){
            return (bytes+ALIGN-1)&~(ALIGN-1);
        }

        static std::size_t FREELIST_INDEX(std::size_t bytes){
            return (bytes+ALIGN-1)/ALIGN-1;
        }

        //返回一个大小为n的对象，可能加入大小为n的新结点
        //假如n已经上调至8的倍数
        static void* refill(std::size_t n);

        //配置一大块空间，可容纳nobjs个大小为size的区块
        //如果配置nobjs个区块有所不便，nobjs会减少
        static char* chunk_alloc(std::size_t size,int& nobjs);
    public:
        static void* allocate(std::size_t bytes);
        static void deallocate(void *ptr,std::size_t bytes);
        static void* reallocate(void* ptr,std::size_t old_sz,std::size_t new_sz);
    };
}
#endif //TINYSTL_ALLOC_H
