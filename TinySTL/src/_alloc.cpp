#include "../incl/alloc.h"

//alloc.h的实现细节

namespace TinySTL{
    alloc::obj* alloc::free_list[alloc::NFREELISTS]=
            {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
             nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};

    char* alloc::start_free=nullptr;
    char* alloc::end_free=nullptr;
    std::size_t alloc::heap_size=0;

    void* alloc::refill(std::size_t n){
        int nobjs=20;
        char* chunk=chunk_alloc(n,nobjs);

        obj* *my_free_list;
        obj* current_obj,*next_obj;
        obj* result;

        //如果只获得一个区块，这个区块直接分配给调用者，freelist无新结点
        if(1==nobjs) return chunk;

        //否则纳入新结点
        my_free_list=free_list+FREELIST_INDEX(n);

        //第1个区块返给客户端
        result=(obj*)chunk;

        //从下一个对象的位置开始将新结点串起来
        *my_free_list=next_obj=(obj*)(chunk+n);

        for(int i=1;;++i){
            current_obj=next_obj;
            next_obj=(obj*)((char*)(next_obj)+n);
            if(nobjs-1==i){
                current_obj->next=nullptr;
                break;
            }
            else
                current_obj->next=next_obj;
        }
        return result;
    }

    char* alloc::chunk_alloc(std::size_t size, int &nobjs){
        char* result;
        std::size_t total_bytes= size * nobjs;
        std::size_t bytes_left=end_free-start_free;//内存池剩余量

        if(bytes_left>=total_bytes){    //内存池可以容下所有结点
            result=start_free;
            start_free+=total_bytes;
            return result;
        }else if(bytes_left>=size){     //内存池至少有一个区块
            nobjs=bytes_left/size;
            total_bytes= size * nobjs;
            result=start_free;
            start_free+=total_bytes;
            return result;
        }else{      //如果一个区块的大小都无法提供
            std::size_t bytes_to_get=2*total_bytes+ROUND_UP(heap_size>>4);
            //ROUND_UP(heap_size)>>4为附加值

            if(bytes_left>0){//利用内存池中的残余零头
                obj* *my_free_list=free_list+FREELIST_INDEX(bytes_left);

                //让内存池的残余空间编入free list
                ((obj*)(start_free))->next=*my_free_list;
                *my_free_list=(obj*)start_free;
            }

            //申请内存
            start_free=(char*)malloc(bytes_to_get);

            if(!start_free){    //heap空间不够，malloc失败
                obj* *my_free_list=nullptr,* p=nullptr;
                //试着在free list中找可以利用的
                for(int i=size;i<=alloc::MAX_BYTES;i+=ALIGN){
                    my_free_list=free_list+FREELIST_INDEX(i);
                    p=*my_free_list;
                    if(p){
                        //抽去
                        *my_free_list=p->next;
                        start_free=(char*)p;
                        end_free=start_free+i;
                        //调整nobjs并递归调用chunk_alloc
                        return chunk_alloc(size,nobjs);
                    }
                }
                end_free=nullptr;
            }
            heap_size+=bytes_to_get;
            end_free=start_free+bytes_to_get;
            //调整nobjs
            return chunk_alloc(size,nobjs);
        }
    }

    void* alloc::allocate(std::size_t bytes) {
        //当bytes大于最大字节数时直接malloc返回
        if(bytes>alloc::MAX_BYTES){
            return malloc(bytes);
        }

        //找到当前free list
        obj* *my_free_list=free_list+FREELIST_INDEX(bytes);
        obj* list=*my_free_list;
        if(list){
            *my_free_list=list->next;   //list=list->next （×）  list只是*my_free_list的拷贝
            return list;
        }
        else    //如果没有free list可以用，要求填充内存池
            return refill(ROUND_UP(bytes));
    }

    void alloc::deallocate(void *ptr,std::size_t bytes){
        if(bytes>alloc::MAX_BYTES){
            free(ptr);
            return ;
        }

        obj* *my_free_list=free_list+FREELIST_INDEX(bytes);

        obj* q=static_cast<obj*>(ptr);

        //把区块回收
        q->next=*my_free_list;
        *my_free_list=q;
    }

    void* alloc::reallocate(void* ptr,std::size_t old_sz,std::size_t new_sz){
        deallocate(ptr,old_sz);
        return allocate(new_sz);
    }
}
