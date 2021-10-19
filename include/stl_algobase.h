#ifndef STL_ALGOBASE_H
#define STL_ALGOBASE_H

#include <cstring>
#include "stl_move.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "utility.h" // swap()

namespace TinySTL{
	// FIXME: In C++14, use auto is better
    template<typename T1,typename T2>
	TINYSTL_CONSTEXPR
#ifdef CXX_STANDARD_14
	auto
#else
	Common_type_t<T1, T2> 
#endif
	max(T1 const& x,T2 const& y) {
        return x<y?y:x;
    }

    template<typename T1,typename T2>
	TINYSTL_CONSTEXPR
#ifdef CXX_STANDARD_14
	auto
#else
    Common_type_t<T1, T2> 
#endif
	min(T1 const& x, T2 const& y) {
        return x<y?x:y;
    }

	/**
	 * @fn fill
     * @brief fill range in a given value
	 */
    template<typename ForwardIterator,typename T>
    TINYSTL_CONSTEXPR void fill(ForwardIterator first,ForwardIterator last,T const& value){
        for(;first!=last;++first)
            *first=value;
    }

    // char* and wchar_t* as byte stream, we can use memset to set specified value
    TINYSTL_CONSTEXPR void fill(char* first,char* last,char const& value){
        std::memset(first,static_cast<unsigned char>(value),last-first);
    }

    inline void fill(wchar_t* first,wchar_t* last,wchar_t const& value){
        std::memset(first,static_cast<unsigned char>(value),(last-first)*sizeof(wchar_t));
    }
	
	/**
	 * @fn fill_n
	 * @brief fill number of n after first in given value	
	 */
    template<typename OutputIterator,typename Size,typename T>
    TINYSTL_CONSTEXPR OutputIterator fill_n(OutputIterator first, Size n, T const& value){
        for(;n>0;--n,++first){
            *first=value;
        }
        return first;
    }

    template<typename Size>
    TINYSTL_CONSTEXPR char* fill_n(char* first,Size n,char const& value){
        std::memset(first,static_cast<unsigned char>(value),n);
        return first+n;
    }

    template<typename Size>
    wchar_t* fill_n(wchar_t* first,Size n,wchar_t const& value){
        std::memset(first,static_cast<unsigned char>(value),n*sizeof(wchar_t));
        return first+n;
    }
	
	/**
	 * @fn copy
	 * @brief write contents in [first, last) into [result, result + (last - first))
	 */
    template<class InputIterator,class OutputIterator>
    OutputIterator copy_iterator(InputIterator first,InputIterator last,
                                 OutputIterator result,Input_iterator_tag)
    {
        for(;first!=last;++result,++first)
            *result=*first;
        return result;
    }

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

    //tag dispatching
    template<class InputIterator,class OutputIterator>
    struct copy_dispatch{
        OutputIterator operator()(InputIterator first,InputIterator last,
                                  OutputIterator result){
            return copy_iterator(first,last,result,iterator_category(first));
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
    BI_2 copy_b_distance(BI_1 first,BI_1 last,
                         BI_2 result,Distance*){
        for(Distance n=last-first;n>0;--n){
            *--result=*--last;
        }
        return result;
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
            return copy_b_trivial(first,last,result,is_assign());
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
    OI move_iterator_(II first,II last,OI result,
            Input_iterator_tag){
        for(;first!=last;++result,++first){
            *result=TinySTL::move(*first);
        }
        return result;
    }

    template<typename II,typename OI>
    OI move_iterator_(II first,II last,OI result,
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
        return move_iterator_(first,last,result,iterator_category(first));
    }

    template<typename IT,typename OT>
    TinySTL::Enable_if_t<TinySTL::Is_same<TinySTL::Remove_reference_t<IT>,
                                          OT>::value &&_type_traits<OT>
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
    TinySTL::Enable_if_t<TinySTL::Is_same<TinySTL::Remove_reference_t<T1>,
                                            T2>::value&&
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
    /*****************************lexicographical_compare*********************************/
    template<typename II_1,typename II_2>
    bool lexicographical_compare(II_1 first1, II_1 last1,
                                 II_2 first2, II_2 last2){
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
    bool lexicographical_compare(II_1 first1, II_2 last1,
                                 II_2 first2, II_2 last2, BP binary_pred){
        for(;first1!=last1&&first2!=last2;++first1,++first2){
            if(binary_pred(*first1,*first2))
                return true;
            if(binary_pred(*first2,*first1))
                return false;
        }

        return first1==last1&&first2!=last2;
    }
    /*****************************swap********************************************/

    template<typename FwdIter,typename =TinySTL::Enable_if_t<is_forward_iterator<FwdIter>::value>>
    void iter_swap(FwdIter iter1,FwdIter iter2){
    	using value_type=Iter_value_type<FwdIter>;
    	value_type tmp=TinySTL::move(*iter2);
    	*iter2=TinySTL::move(*iter1);
    	*iter1=TinySTL::move(tmp);
    }

}


#endif
