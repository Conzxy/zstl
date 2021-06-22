#ifndef TINYSTL_STL_HEAP_H
#define TINYSTL_STL_HEAP_H

//该文件提供heap相关算法
#include "config.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "Functional.h"

namespace TinySTL {
    sz_t left(sz_t i){ return 2*i+1; }
    sz_t right(sz_t i){ return 2*i+2; }
    sz_t parent(sz_t i){ return (i-1)/2; }

    //push_heap_aux
    //O(lgn)
    namespace detail {
        template<typename T, typename RI, typename Distance, typename Compare>
        void push_heap_aux(RI first,
                           Distance topIndex, Distance holeIndex, T key,
                           Compare cmp) {
            //percolate up
            while (holeIndex > topIndex && cmp(*(first + parent(holeIndex)), key)) {
                *(first + holeIndex) = *(first + parent(holeIndex));
                holeIndex = parent(holeIndex);
            }
            *(first + holeIndex) = key;
        }
    }
    //push_heap:将元素压入heap，其范围为[first,last-1)，该操作过后，范围变为[first,last)
    //O(lgn)
    template<typename RI,typename Compare>
    constexpr void push_heap(RI first,RI last,Compare cmp){
        using Value_type=typename iterator_traits<RI>::value_type;
        using Distance_type=typename iterator_traits<RI>::difference_type;

        Value_type val=STL_MOVE(*(last-1));
        detail::push_heap_aux(first, Distance_type(0), Distance_type(last - first - 1), STL_MOVE(val),cmp);
    }

    template<typename RI>
    constexpr void push_heap(RI first,RI last){
        using Value_type=typename iterator_traits<RI>::value_type;
        using Distance_type=typename iterator_traits<RI>::difference_type;

        Value_type val=STL_MOVE(*(last-1));
        push_heap_aux(first, Distance_type(0), Distance_type(last - first - 1), STL_MOVE(val),STL_MOVE(STL_ less<Value_type>()));
    }


    //adjust_heap:维护heap-property，注意调用该函数前保证子树满足heap-property（不然性质仍被破坏）
    //T(n)<=T(2n/3)+Θ(1),用substitution method易证T(n)=O(lgn)
    template<typename RI,typename Distance,typename Compare>
    constexpr void adjust_heap(RI first,Distance holeIndex,Distance heap_size,Compare cmp){
        //percolate down
            /*Distance l,r;
        while(true) {
            l = left(holeIndex);
            r = right(holeIndex);
            Distance largest=holeIndex;
            if (l < heap_size && *(first + holeIndex) < *(first + l))
                largest = l;
            if (r < heap_size && *(first + largest) < *(first + r))
                largest = r;
            if(largest == holeIndex)
                return;
            else{
                STL_SWAP(*(first+largest),*(first+holeIndex));
                holeIndex=largest;
            }
        }*/

            Distance child;
            auto tmp=STL_MOVE(*(first+holeIndex));

            for(;left(holeIndex)<heap_size;holeIndex=child){
                child=left(holeIndex);
                if(child+1!=heap_size && cmp(*(first+child),*(first+child+1)))
                    ++child;
                if(cmp(tmp,*(first+child)))
                    *(first+holeIndex)=STL_MOVE(*(first+child));
                else break;
            }
            *(first+holeIndex)=STL_MOVE(tmp);
    }

    //pop_heap:将top弹出heap，并交换top与tail，此时[first,last-1)满足heap-property
    //O(lgn)
    template<typename RI,typename Compare>
    constexpr void pop_heap_aux(RI first,RI last,RI result,Compare cmp){
        using Distance_type=typename iterator_traits<RI>::difference_type;

        STL_SWAP(*first,*result);
        adjust_heap(first,Distance_type(0),Distance_type(last-first),cmp);
    }

    template<typename RI,typename Compare>
    constexpr void pop_heap(RI first,RI last,RI result,Compare cmp){
        if(last-first>1)
            pop_heap_aux(first,last-1,last-1,cmp);
    }

    template<typename RI>
    constexpr void pop_heap(RI first,RI last){
        using Value_type=typename iterator_traits<RI>::value_type;
        if(last-first>1)
            pop_heap_aux(first,last-1,last-1,STL_MOVE(STL_ less<Value_type>()));
    }

    //从第一个非叶子结点节点进行调整
    //通过循环不变式易证算法正确性
    //O(n)
    template<typename RI>
    void make_heap(RI first,RI last){
        using Distance_type=typename iterator_traits<RI>::difference_type;
        using Value_type=typename iterator_traits<RI>::value_type;

        if(last-first<2) return;
        Distance_type heap_size=last-first;
        Distance_type noLeaf=heap_size/2-1;
        while(noLeaf>=0){
            adjust_heap(first,noLeaf,heap_size,STL_MOVE(STL_ less<Value_type>()));
            --noLeaf;
        }
    }

    template<typename RI,typename Compare>
    void make_heap(RI first,RI last,Compare cmp){
        using Distance_type=typename iterator_traits<RI>::difference_type;

        if(last-first<2) return;
        Distance_type heap_size=last-first;
        Distance_type noLeaf=heap_size/2-1;
        while(noLeaf>=0){
            adjust_heap(first,noLeaf,heap_size,cmp);
            --noLeaf;
        }
    }

    //sort_heap:堆排序
    //O(nlgn)
    template<typename RI>
    void sort_heap(RI first,RI last){
        make_heap(first,last);
        auto heap_size=last-first;
        for(auto i=heap_size-1;i>=1;--i){
            STL_SWAP(*(first),*(first+i));
            --heap_size;
            adjust_heap(first,0,heap_size,STL_MOVE(STL_ less<typename iterator_traits<RI>::value_type>()));
        }

        //while(last-first>1)
        //    pop_heap(first, last--);
    }

    template<typename RI,typename Compare>
    void sort_heap(RI first,RI last,Compare cmp){
        make_heap(first,last);
        auto heap_size=last-first;
        for(auto i=heap_size-1;i>=1;--i){
            STL_SWAP(*(first),*(first+i));
            --heap_size;
            adjust_heap(first,0,heap_size,cmp);
        }
    }
}
#endif //TINYSTL_STL_HEAP_H
