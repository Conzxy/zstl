#ifndef TINYSTL_STL_ALGORITHM_H
#define TINYSTL_STL_ALGORITHM_H

#include <cstring>
#include "_move.h"
#include <stl_iterator.h>
#include "type_traits.h"
#include "stl_algobase.h"

namespace TinySTL {
    /*****************************find********************************************/
    template<typename II,typename T>
    II find(II first, II last, T const& val) {
        /*for (; first!=last; ++first)
            if (*first==val)
                return first;*/
        while (first!=last&&*first!=val) ++first;
        return first;
    }

    /***************************find_if*******************************************/
    template<typename II,typename Pred>
    II find_if(II first, II last, Pred pred) {
        while (first!=last&&!pred(*first)) ++first;
        return first;
    }

    /*****************************find_end****************************************/


    template<typename Iter>
    void reverse(Iter first,Iter last){
        if constexpr (is_bidirectional_iterator<Iter>){
            if(first == last || first == --last)
                return ;
            else{
                iter_swap(first++,last--);
            }
        }
        else if constexpr (is_random_access_iterator<Iter>){
            while(first < last)
                iter_swap(first++,last--);
        }
    }

    template<typename Iter>
    void rotate(Iter first,Iter mid,Iter last){
        if(first == mid || mid == last) return ;
        if constexpr (is_forward_iterator<Iter>){
            for(Iter _mid=mid;;){
                iter_swap(first++,_mid++);

                if(first == mid){
                    if(_mid == last) return ;
                    mid=_mid;
                }
                else if(_mid == last){
                    _mid=mid;
                }
            }
        }
        else if constexpr (is_bidirectional_iterator<Iter>){
            reverse(first,mid);
            reverse(mid,last);
            reverse(first,last);
        }
    }
}
#endif //TINYSTL_STL_ALGORITHM_H
