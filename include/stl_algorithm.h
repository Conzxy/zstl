#ifndef TINYSTL_STL_ALGORITHM_H
#define TINYSTL_STL_ALGORITHM_H

#include "config.h"
#include "stl_move.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_algobase.h"
#include "functional.h"

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


    template<typename Iter, STL_ENABLE_IF((is_bidirectional_iterator<Iter>::value), int)>
    void reverse(Iter first,Iter last){
		if(first == last || first == --last)
			return ;
		else{
			iter_swap(first++,last--);
		}
    }

	template<typename Iter, STL_ENABLE_IF((is_random_access_iterator<Iter>::value), char)>
	void reverse(Iter first, Iter last){
		while(first < last)
			iter_swap(first++,last--);
	}
	
	template<typename Iter, STL_ENABLE_IF((is_forward_iterator<Iter>::value), char)>
	void rotate_impl(Iter first, Iter mid, Iter last){
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
	
	template<typename Iter, STL_ENABLE_IF((is_bidirectional_iterator<Iter>::value), int)>
	void rotate_impl(Iter first, Iter mid, Iter last){
		reverse(first,mid);
		reverse(mid,last);
		reverse(first,last);
	}

    template<typename Iter>
    void rotate(Iter first,Iter mid,Iter last){
        if(first == mid || mid == last) return ;

		rotate_impl(first, mid, last);
    }

    //lower_bound
    //返回第一个不小于给定值的位置（iterator）
    //P.s.:这个适用于有序序列
    template<typename FI,typename T>
    FI lower_bound(FI first,FI last,T const& val){
        using diff=typename iterator_traits<FI>::difference_type;

        diff half;
        diff len=distance(first,last);
        FI middle;

        //IDEA:binary search
        //if middle value less than given value,go to (first,last)
        //else go to [first,middle]
        while(len > 0){
            half=len/2;
            middle=first;
            advance(middle,half);

            if(*middle < val){
                first=middle;
                ++first;
                len=len-half-1;
            }else{
                len=half;
            }
        }
        return first;
    }

    //FUNCITON TEMPLATE for_each
    template<typename II,typename Func,typename ...Args>
    Func for_each(II first,II last,Func func,Args... args)
    {
        while(first != last)
        {
            TinySTL::invoke(func,TinySTL::forward<Args>(args)...,*first);
            ++first;
        }
        return func;
    }

	/*
	 * Removing Algorithm
	 * remove		| remove_if
	 * remove_copy  | remove_copy_if
	 * unique		| unique_if
	 * unique_copy  | unique_copy_if
	 */
	
	//FUNCTION TEMPLATE remove
	//remove does not really remove some elements, just let these unremoved elements move forward,return new logical end
	//you should use erase provided by correspending container to delete these elements after the logical end 
	template<typename FI, typename T>
	FI remove(FI , FI , T const& ){
		static_assert(TinySTL::Is_same<typename iterator_traits<FI>::value_type, T>::value, 
					  "remove assert: the value_type of the iterator differ from given value type");
		
	}
}

#endif //TINYSTL_STL_ALGORITHM_H
