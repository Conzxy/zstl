#ifndef TINYSTL_STL_ALGORITHM_H
#define TINYSTL_STL_ALGORITHM_H

#include <cstring>
#include "_move.h"
#include "stl_iterator.h"
#include "type_traits.h"

namespace TinySTL{
    template<typename T1,typename T2>
    auto max(T1 const& x,T2 const& y){
        return x<y?y:x;
    }
    /**************************************************************************/
    //[first,last)内的所有元素覆写（overwrite）新值
    template<typename ForwardIterator,typename T>
    void fill(ForwardIterator first,ForwardIterator last,T const& value){
        for(;first!=last;++first)
            *first=value;
    }

    //对char，wchar_t进行特化
    template<>
    void fill<char*,char>(char* first,char* last,char const& value){
        std::memset(first,static_cast<unsigned char>(value),last-first);
    }

    template<>
    void fill<wchar_t*,wchar_t>(wchar_t *first,wchar_t *last,wchar_t const& value){
        std::memset(first,static_cast<unsigned char>(value),(last-first)*sizeof(wchar_t));
    }

    /**********************************************************************************/
    //[first,last)内的前n个元素覆写（overwrite）新值
    template<typename OutputIterator,typename Size,typename T>
    OutputIterator fill_n(OutputIterator first, Size n, T const& value){
        //异常处理待补充
        for(;n>0;--n,++first){
            *first=value;
        }
        return first;
    }

    template<typename Size>
    char* fill_n(char* first,Size n,char const& value){
        std::memset(first,static_cast<unsigned char>(value),n);
        return first+n;
    }

    template<typename Size>
    wchar_t* fill_n(wchar_t* first,Size n,wchar_t const& value){
        std::memset(first,static_cast<unsigned char>(value),n*sizeof(wchar_t));
        return first+n;
    }

    /***************************************************************************/
    //拷贝[first,last)内的元素到[result,result+(last-first))
    //对于n<(last-first),(*result+n)=*(first+n)
    //require：result不在[first,last)中
    //如果result在[first,last)则可能在被复制前该值就被覆写了，
    //但是如果是memmove操作，则不会出现这种情况，因为memmove会先拷贝好原来区间的值

    //InputIterator版本
    //由迭代器是否等同来决定循环次数，速度相对较慢
    template<class InputIterator,class OutputIterator>
    OutputIterator copy_iterator(InputIterator first,InputIterator last,
                                 OutputIterator result,Input_iterator_tag)
    {
        for(;first!=last;++result,++first)
            *result=*first;
        return result;
    }

    //Random access Iterator的辅助函数
    //由n来决定循环次数，速度快
    template<class RandomAccessIterator,class OutputIterator,
            typename Distance_type>
    OutputIterator copy_distance(RandomAccessIterator first,RandomAccessIterator last,
                                 OutputIterator result,Distance_type*)
    {
        for(Distance_type n=last-first;n>0;--n,++result,++first){
            *result=*first;
        }
        return result;
    }

    //Random access Iterator版本
    template<class RandomAccessIterator,class OutputIterator>
    OutputIterator copy_iterator(RandomAccessIterator first,RandomAccessIterator last,
                                 OutputIterator result,Random_access_iterator_tag)
    {
        return copy_distance(first,last,result,distance_type(first));
    }


    //trivial assignmengt
    template<class T>
    T* copy_trivial(T const* first,T const* last,
                    T* result,_true_type)
    {
        std::memmove(result,first,sizeof(T)*(last-first));
        return result+(last-first);
    }

    template<class T>
    T* copy_trivial(T const* first,T const* last,
                    T* result,_false_type)
    {
        //内置指针本身就是一种Random access iterator
        return copy_distance(first,last,result,(ptrdiff_t*)nullptr);
    }
    //tag dispatch
    template<class InputIterator,class OutputIterator>
    struct copy_dispatch{
        OutputIterator operator()(InputIterator first,InputIterator last,
                                  OutputIterator result){
            return copy_iterator(result,first,iterator_category(first));
        }
    };

    template<typename T>
    struct copy_dispatch<T*,T*>{
        T* operator()(T* first,T* last,T* result){
            using is_assign=typename _type_traits<T>::has_trivially_assignment_operator;
            return copy_trivial(first,last,result,is_assign());
        }
    };

    template<typename T>
    struct copy_dispatch<T const*,T*>{
        T* operator()(T const* first,T const* last,T* result){
            using is_assign=typename _type_traits<T>::has_trivially_assignment_operator;
            return copy_trivial(first,last,result,is_assign());
        }
    };

    template<class InputIterator,class OutputIterator>
    OutputIterator copy(InputIterator first,InputIterator last,
         OutputIterator result)
    {
        return copy_dispatch<InputIterator,OutputIterator>()
                (first,last,result);
    }

    //对char*，wchar_t*进行特化
    //memmove底层内存操作，速度极快
    template<>
    inline char*
    copy<char const*,char*>(char const* first,char const* last,
                            char* result)
    {
        std::memmove(result,first,last-first);
        return result+(last-first);
    }

    template<>
    inline wchar_t*
    copy<wchar_t const*,wchar_t*>(wchar_t const* first,wchar_t const* last,
                                  wchar_t *result)
    {
        std::memmove(result,first,sizeof(wchar_t)*(last-first));
        return result+(result-first);
    }

    /********************************************************************************/
    //反向copy
    /*                 ←result
     *                   ↓
     * _ _ _ _ _ _ _ _ _ _ _
     *     ↑   ←  ↑
     *   first   last
     *   注意！：若result落在[first,last)内，则可能会在原件被拷走之前被覆写
     *   不过POD类型用的memmove没有这个问题
     */
    template<typename BI_1,typename BI_2>
    BI_2 copy_b_iterator(BI_1 first,BI_1 last,
                         BI_2 result,Bidirectional_iterator_tag){
        while(last!=first){
            *--result=*--last;
        }
        return result;
    }

    template<typename BI_1,typename BI_2,typename Distance>
    BI_2 copy_b_distance(BI_1 first,BI_2 last,
                         BI_2 result,Distance*){
        for(Distance n=last-first;n>0;--n){
            *--result=*--last;
        }
    }

    template<typename BI_1,typename BI_2>
    BI_2 copy_b_iterator(BI_1 first,BI_1 last,
                         BI_2 result,Random_access_iterator_tag){
        return copy_b_distance(first,last,result,distance_type(first));
    }

    template<typename T>
    T* copy_b_trivial(T* first,T* last,
                      T* result,_true_type){
        auto n=last-first;
        std::memmove(result-n,first,sizeof(*first)*n);
        return result-n;
    }

    template<typename T>
    T* copy_b_trivial(T* first,T* last,
                      T* result,_false_type){
        return copy_b_distance(first,last,result,(std::ptrdiff_t*)(nullptr));
    }

    template<class BI_1,class BI_2>
    struct copy_b_dispatch{
        BI_2 operator()(BI_1 first,BI_1 last,BI_2 result){
            return copy_b_iterator(first,last,result,iterator_category(first));
        }
    };

    template<typename T>
    struct copy_b_dispatch<T*,T*>
    {
        T* operator()(T* first,T* last,T* result){
            using is_assign=typename _type_traits<T>::has_trivially_assignment_operator;
            return copy_b_trivial(first,last,result,is_assign());
        }
    };

    template<typename T>
    struct copy_b_dispatch<const T*,T*>
    {
        T* operator()(const T* first,const T* last,T* result){
            using is_assign=typename _type_traits<T>::has_trivially_assignment_operator;
            return copy_b_trivial(first,last,is_assign());
        }
    };

    template<class BI_1,class BI_2>
    inline BI_2 copy_backward(BI_1 first,BI_1 last,BI_2 result){
        return copy_b_dispatch<BI_1,BI_2>()
                (first,last,result);
    }

    template<>
    inline char*
    copy_backward<char*,char*>(char* first,char* last,char* result){
        auto n=last-first;
        std::memmove(result-n,first,n);
        return result-n;
    }

    template<>
    inline wchar_t*
    copy_backward<wchar_t*,wchar_t*>(wchar_t* first,wchar_t* last,wchar_t* result){
        auto n=last-first;
        std::memmove(result-n,first,n*sizeof(wchar_t));
        return result-n;
    }

    /*********************************move*****************************************************/
    template<typename II,typename OI>
    OI move_iterator(II first,II last,OI result,
            Input_iterator_tag){
        for(;first!=last;++result,++first){
            *result=TinySTL::move(*first);
        }
        return result;
    }

    template<typename II,typename OI>
    OI move_iterator(II first,II last,OI result,
            Random_access_iterator_tag){
        for(auto n=last-first;n>0;--n,++result,++first){
            *result=TinySTL::move(*first);
        }
        return result;
    }

    //如果是同类型内置指针，则选择第二重载
    //因为IT*->int*比II->int*更接近，重载决议偏向于第二重载
    //当然不同类型选择第一重载
    template<typename II,typename OI>
    OI move_dispatch(II first,II last,OI result){
        return move_iterator(first,last,result,iterator_category(first));
    }

    template<typename IT,typename OT>
    TinySTL::Enable_if_t<TinySTL::Is_same_v<TinySTL::Remove_reference_t<IT>,
                                          OT>&&_type_traits<OT>
                                          ::has_trivially_assignment_operator::value,OT*>
    move_dispatch(IT* first,IT* last,OT* result){
        auto n=last-first;
        std::memmove(result,first,n*sizeof(OT));
        return result+n;
    }

    template<typename II,typename OI>
    OI move(II first,II last,OI result){
        return move_dispatch(first,last,result);
    }

    /*****************************move_backward***********************************/
    template<typename BI_1,typename BI_2>
    BI_2 move_backward_iterator(BI_1 first,BI_1 last,
                                BI_2 result,Bidirectional_iterator_tag){
        while(first!=last)
            *--result=TinySTL::move(*--first);
        return result;
    }

    template<typename BI_1,typename BI_2>
    BI_2 move_backward_iterator(BI_1 first,BI_1 last,
                                BI_2 result,Random_access_iterator_tag){
        for(auto n=last-first;n>0;--n,++first,++result){
            *--result=TinySTL::move(*--first);
        }
        return result;
    }

    template<typename BI_1,typename BI_2>
    BI_2 move_b_dispatch(BI_1 first,BI_1 last,
                         BI_2 result){
        return move_backward_iterator(first,last,result,iterator_category(first));
    }

    template<typename T1,typename T2>
    TinySTL::Enable_if_t<TinySTL::Is_same_v<TinySTL::Remove_reference_t<T1>,
                                            T2>&&
                                            _type_traits<T2>
                                            ::has_trivially_assignment_operator::value,T2*>
    move_b_dispatch(T1* first,T1* last,T2* result){
        auto n=last-first;
        std::memmove(result-n,first,n*sizeof(T2));
        return result-n;
    }

    template<typename II,typename OI>
    OI move_backward(II first,II last,OI result){
       return move_b_dispatch(first,last,result);
    }
    /*****************************equal*******************************************/
    template<typename II_1,typename II_2>
    inline bool equal(II_1 first1,II_1 last1,
                      II_2 first2){
        for(;first1!=last1;++first1,++first2){
            if(*first1!=*first2)
                return false;
        }
        return true;
    }

    template<typename II_1,typename II_2,
            typename BP>
    inline bool equal(II_1 first1,II_2 last1,
                      II_2 first2,BP binary_pred){
        for(;first1!=last1;++first1,++first2){
            if(!binary_pred(*first1,*first2))
                return false;
        }
        return true;
    }
    /*****************************lexicographical*********************************/
    template<typename II_1,typename II_2>
    bool lexicographical(II_1 first1,II_1 last1,
                         II_2 first2,II_2 last2){
        for(;first1!=last1&&first2!=last2;++first1,++first2){
            if(*first1<*first2)
                return true;
            if(*first2<*first1)
                return false;
        }

        return first1==last1&&first2!=last2;
    }

    template<typename II_1,typename II_2,
            typename BP>
    bool lexicographical(II_1 first1,II_2 last1,
                         II_2 first2,II_2 last2,BP binary_pred){
        for(;first1!=last1&&first2!=last2;++first1,++first2){
            if(binary_pred(*first1,*first2))
                return true;
            if(binary_pred(*first2,*first1))
                return false;
        }

        return first1==last1&&first2!=last2;
    }
    /*****************************swap********************************************/
    template<typename T>
    void swap(T& x,T& y){
        auto tmp=TinySTL::move(y);
        y=TinySTL::move(x);
        x=TinySTL::move(tmp);
    }
}

#endif //TINYSTL_STL_ALGORITHM_H
