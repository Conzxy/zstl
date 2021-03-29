#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "stl_algorithm.h"
#include <stl_numeric.h>

namespace TinySTL{
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

}
#endif //!ALGORITHM_H
