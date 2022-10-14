#ifndef ZSTL_STL_UNINITIALIZED_H
#define ZSTL_STL_UNINITIALIZED_H

#include "stl_construct.h"
#include "stl_algorithm.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_exception.h"

namespace zstl{
    // gcc use class template<IsTrivialType>
    // but according to "the rule of chiel", class template also have cost
    // (just better than function template and SFINAE)
    template<class ForwardIterator,typename T>
    void uninitialized_fill_dispatch(ForwardIterator first,ForwardIterator last,
                                     T const& x,_true_type)
    {
        zstl::fill(first,last,x);
    }

    template<class ForwardIterator,typename T>
    void uninitialized_fill_dispatch(ForwardIterator first,ForwardIterator last,
                                     T const& x,_false_type)
    {
        ForwardIterator cur;
        TRY_BEGIN
            //FIXME std::__addressof() instead of &*
            for(cur=first;cur!=last;++cur){
                zstl::construct(addressof(cur),x);
            }
        TRY_END
        CATCH_ALL_BEGIN
            zstl::destroy(first, cur);
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
        using ValueType = typename iterator_traits<ForwardIterator>::value_type;

        static_assert(Is_convertible<T, ValueType>::value,
            "uninitialized_fill: Given value type can not convertible to value type of"
            "iterator");

        using TrivialType = Conjunction_t<Is_trivial<ValueType>, Is_copy_constructible<ValueType>>;
        uninitialized_fill_dispatch(first,last,x, TrivialType{});
    }
    /*********************************************************************/
    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x, _true_type){
        return zstl::fill_n(first,n,x);
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n_dispatch(ForwardIterator first,Size n,
                                     T const& x, _false_type){
        auto cur=first;
        TRY_BEGIN
			for(;n>0;--n,++cur){
				zstl::construct(&*cur,x);
			}
		TRY_END
        CATCH_ALL_BEGIN 
            destroy(first, cur);
			RETHROW
        CATCH_END
        return cur;
    }

    template<class ForwardIterator,typename Size,typename T>
    ForwardIterator uninitialized_fill_n(ForwardIterator first,Size n,
                                         T const& x){
        using ValueType = typename iterator_traits<ForwardIterator>::value_type;

        static_assert(Is_convertible<T, ValueType>::value,
            "uninitialized_fill_n: Given value type can not convertible to value type of"
            "iterator");

        using TrivialType = Conjunction_t<Is_trivial<ValueType>, Is_copy_constructible<ValueType>>;
        return uninitialized_fill_n_dispatch(first,n,x, TrivialType{});
    }

    /**********************************************************************/
    template<class InputIterator,class ForwardIterator>
    ForwardIterator uninitialized_copy_dispatch(InputIterator first,InputIterator last,
                                                ForwardIterator result,_true_type){
        return zstl::copy(first,last,result);
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
		using T = typename iterator_traits<InputIterator>::value_type;
        using ValueType = typename iterator_traits<ForwardIterator>::value_type;

        static_assert(Is_convertible<T, ValueType>::value,
            "uninitialized_fill_n: Given value type can not convertible to value type of"
            "iterator");

        using TrivialType = Conjunction_t<Is_trivial<ValueType>, Is_copy_constructible<ValueType>>;
        return uninitialized_copy_dispatch(first,last,result, TrivialType{});
    }

    /**************************************************************************************************/
    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_true_type){
        return zstl::move(first,last,result);
    }

    template<typename II,typename FI>
    FI uninitialized_move_dispatch(II first,II last,
                          FI result,_false_type){
        auto res=result;

        TRY_BEGIN
            for(;first!=last;++first,++result){
                construct(&*result,zstl::move(*first));
            }
        TRY_END
        CATCH_ALL_BEGIN
            destroy(res,result);
        RETHROW
        CATCH_END

        return result;
    }

    template<typename II,typename FI>
    FI uninitialized_move(II first,II last,FI result){
        using ValueType = typename iterator_traits<FI>::value_type;
		using T = typename iterator_traits<II>::value_type;

        static_assert(Is_convertible<T, ValueType>::value,
            "uninitialized_fill_n: Given value type can not convertible to value type of"
            "iterator");

        using TrivialType = Conjunction_t<Is_trivial<ValueType>, Is_copy_constructible<ValueType>>;
        return uninitialized_move_dispatch(first,last,result, TrivialType{});
    }

	template<typename II, typename FI>
	inline FI uninitialized_move_if_noexcept(II first, II last, FI result) {
		return zstl::uninitialized_copy(
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(first), 
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(last), 
				result);
	}

}
#endif //ZSTL_STL_UNINITIALIZED_H
