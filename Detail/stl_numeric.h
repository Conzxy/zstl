#ifndef STL_NUMERIC_H
#define STL_NUMERIC_H

namespace TinySTL {
	/************************accumulate************************/
	template<typename II>
	constexpr auto accumulate(II first, II last) {
		return accumulate(first, last, TinySTL::plus);
	}

	template<typename II, typename BinaryOperation>
	constexpr auto accumulate(II first, II last, BinaryOperation binary_op) {
		using T=typename TinySTL::iterator_traits<II>::value_type;
		T init{};
		for (; first!=last; ++first)
			init=binary_op(init, *first);
		return init;
	}

	/***************************adjacent_difference************************/
	//compute the difference adjacent elements in [first,last)
	template<typename II,typename OI>
	OI adjacent_difference(II first, II last,OI result) {
		return adjacent_difference(first, last, result, TinySTL::minus<>{});
	}

	template<typename II,typename OI,typename BinaryOperation>
	OI adjacent_difference(II first, II last, OI result,
						   BinaryOperation binary_op) {
		if (first==last) return result;

		TinySTL::iterator_traits<II>::value_type val=*first;

		while (++first!=last) {
			auto tmp=*first;
			*result=binary_op(tmp, val);
			val=tmp;
		}
		return ++result;
	}

	/******************inner_product*******************************/
	template<typename II_1,typename II_2>
	constexpr auto inner_product(II_1 first1, II_1 last1,
					   II_2 first2) {
		return inner_product(first1, last1, first2, TinySTL::multiplies<>{});
	}

	template<typename II_1,typename II_2,typename BinaryOperation>
	constexpr auto inner_product(II_1 first1, II_1 last1,
					   II_2 first2, BinaryOperation binary_op) {
		using T1=TinySTL::iterator_traits<II_1>::value_type;
		using T2=TinySTL::iterator_traits<II_2>::value_type;
		using T=Common_type_t<T1, T2>;
		if (first1==last1) return T{};
		T res{};
		for (; first1!=last1; ++first1, ++first2) {
			res+=binary_op(*first1,*first2);
		}
		return res;
	}
}

#endif