#ifndef TINYSTL_STL_UNINITIALIZED_H
#define TINYSTL_STL_UNINITIALIZED_H

#include "stl_construct.h"
#include "stl_algorithm.h"
#include "type_traits.h"

namespace TinySTL{
    /**********************************************************************/
    //[first,last)内每个迭代器指向未初始化内存，在该范围内提供一个蓝本供拷贝

    template<class ForwardIterator,typename T>
    void uninitialized_fill_dispatch(ForwardIterator first,ForwardIterator last,
                                   T const& x,TinySTL::_true_type)
    {
        fill(first,last,x);
    }

    template<class ForwardIterator,typename T>
    void uninitialized_fill_dispatch(ForwardIterator first,ForwardIterator last,
                                   T const& x,TinySTL::_false_type)
    {
        auto cur=first;
        for(;cur!=last;++cur){
            construct(&*cur,x);
        }
    }

    //对于POD类型直接调用fill，最有效率（内置类型保证绝对不会出现异常）
    //而对于non_POD类型采用相对安全的方法
    template<typename ForwardIterator,typename T,typename Tv>
    void uninitialized_fill_aux(ForwardIterator first,ForwardIterator last,
                                T const& x,Tv*)
    {
        using is_pod=typename _type_traits<Tv>::is_POD_type;
        unitialized_fill_dispatch(first,last,x,is_pod());//tag dispatch
    }

    template<class ForwardIterator,typename T>
    void uninitialized_fill(ForwardIterator first,ForwardIterator last,
                            T const& x)
    {
        //Q：为什么不直接_type_traits<T>::is_POD_type直接dispatch？
        //A：接口兼容性的完备。即使是指向int的迭代器也可以用来初始化double容器
        unitialized_fill_aux(first,last,x,value_type(first));
    }
    /*********************************************************************/
    //[first,first+n)内的每一个迭代器都指向未初始化的内存
    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x,_true_type){
        return fill_n(first,n,x);
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x,_false_type){
        auto cur=first;
        //exception
        for(;n>0;--n,++cur){
            construct(&*cur,x);
        }
        return cur;
    }

    template<class ForwardIterator,typename Size,typename T,typename Tv>
    ForwardIterator uninitialized_fill_n_aux(ForwardIterator first,Size n,
                                             T const& x,Tv*){
        using is_pod=typename _type_traits<Tv>::is_POD_type;
        return uninitialized_fill_n_dispatch(first,n,x,is_pod());
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first,Size n,
                                         T const& x){
        return uninitialized_fill_n_aux(first,n,x,value_type(first));
    }

    /**********************************************************************/
    //[first,last)->[result,result+(last-first))

    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy_dispatch(InputIterator first,InputIterator last,
                                              ForwardIterator result,_true_type){
        return copy(first,last,result);
    }

    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy_dispatch(InputIterator first, InputIterator last,
                                   ForwardIterator result, _false_type){
        for(;first!=last;++result,++first)
            construct(&*result,*first);
        return result;
    }

    template<class InputIterator,class ForwardIterator,typename Tv>
    ForwardIterator uninitialized_copy_aux(InputIterator first, InputIterator last,
                                           ForwardIterator result, Tv*){
        using is_pod=typename _type_traits<Tv>::is_POD_type;
        return uninitialized_copy_dispatch(first,last,result,is_pod());
    }

    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                       ForwardIterator result){
        return uninitialized_copy_aux(first,last,result,value_type(first));
    }

    //uninitialized_move
    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_true_type){
        return TinySTL::move(first,last,result);
    }

    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_false_type){
        FI cur=result;
        for(;first!=last;++first,++cur){
            construct(&*cur,TinySTL::move(*first));
        }
        return cur;
    }

    template<typename II,typename FI,typename T>
    FI uninitialized_move_aux(II first,II last,FI result,T*){
        using is_pod=typename _type_traits<T>::is_POD_type;
        return uninitialized_move_dispatch(first,last,result,is_pod());
    }

    template<typename II,typename FI>
    FI uninitialized_move(II first,II last,FI result){
        return uninitialized_move_aux(first,last,result,value_type(first));
    }

    //
}
#endif //TINYSTL_STL_UNINITIALIZED_H
