#ifndef TINYSTL_STL_UNINITIALIZED_H
#define TINYSTL_STL_UNINITIALIZED_H

#include "stl_construct.h"
#include "stl_algorithm.h"
#include "type_traits.h"
#include <stl_exception.h>

namespace TinySTL{
    /**********************************************************************/
    //FUNCTION TEMPALATE uninitialized_fill
    //requires:first and last
    //TO DO:在未初始化内存上构造对象
    //return:void
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
        ForwardIterator cur;
        TRY_BEGIN
        for(cur=first;cur!=last;++cur){
            construct(&*cur,x);
        }
        TRY_END
        CATCH_ALL_BEGIN
        for(auto _cur=first;_cur!=cur;++cur)
            (&*_cur)->~T();  //&*是为了解开iterator
        RETHROW
        CATCH_END
    }

    //对于POD类型直接调用fill，最有效率（内置类型保证绝对不会出现异常）
    //而对于non_POD类型采用相对安全的方法
    //当构造失败时，roll-back销毁所有已构造对象
    template<class ForwardIterator,typename T>
    void uninitialized_fill(ForwardIterator first,ForwardIterator last,
                            T const& x)
    {
        uninitialized_fill_dispatch(first,last,x,typename Is_pod<Decay_t<decltype(*first)>>::type{});
    }
    /*********************************************************************/
    //FUNCTION TEMPLATE uninitialzed_fill_n
    //requires:first and n
    //TO DO:在以first开始的n个未初始化内存上构造对象
    //return:first+n
    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x,_true_type){
        return fill_n(first,n,x);
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x,_false_type){
        auto cur=first;
        TRY_BEGIN
        for(;n>0;--n,++cur){
            construct(&*cur,x);
        }
        TRY_END
        CATCH_ALL_BEGIN 
        for(auto _cur=first;_cur!=cur;++_cur)
            (&*_cur)->~T();
        RETHROW
        CATCH_END
        return cur;
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first,Size n,
                                         T const& x){
        return uninitialized_fill_n_dispatch(first,n,x,typename Is_pod<Decay_t<decltype(*first)>>::type{});
    }

    /**********************************************************************/
    //FUNCTION TEMPLATE
    //requires:first,last,resuly
    //TO DO:[first,last)->[result,result+(last-first))
    //return:result+(last-first)
    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy_dispatch(InputIterator first,InputIterator last,
                                                ForwardIterator result,_true_type){
        return copy(first,last,result);
    }

    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy_dispatch(InputIterator first, InputIterator last,
                                                ForwardIterator result, _false_type){
        auto res=result;

        TRY_BEGIN
        for(;first!=last;++result,++first)
            construct(&*result,*first);
        TRY_END
        CATCH_ALL_BEGIN
        destroy(res,result);
        RETHROW
        CATCH_END
        return result;
    }

    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                       ForwardIterator result){
        return uninitialized_copy_dispatch(first,last,result,typename Is_pod<Decay_t<decltype(*first)>>::type{});
    }

    /**************************************************************************************************/
    //FUNCTION TEMPLATE uninitialized_move
    //requires:first,last,result
    //TO DO:same as uninitialized_copy,but move construct object
    //return:result+(last-first)
    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_true_type){
        return TinySTL::move(first,last,result);
    }

    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_false_type){
        auto res=result;

        TRY_BEGIN
        for(;first!=last;++first,++result){
            construct(&*result,TinySTL::move(*first));
        }
        TRY_END
        CATCH_ALL_BEGIN
        destroy(res,result);
        RETHROW
        CATCH_END

        destroy(first,last);
        return result;
    }

    template<typename II,typename FI>
    FI uninitialized_move(II first,II last,FI result){
        return uninitialized_move_dispatch(first,last,result,typename Is_pod<Decay_t<decltype(*first)>>::type{});
    }

}
#endif //TINYSTL_STL_UNINITIALIZED_H
