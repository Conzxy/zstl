#include "type_list.h"
#include "type_list_impl.h"
#include <iostream>
#include "tuple.h"

using namespace zstl::mpl;
using namespace zstl;
	
template<typename T, T left, T right, 
		typename Indices = Valuelist<T>>
struct MakeIndices {
	using type 
		= typename MakeIndices<T, left + 1, right, TL::Push_Back<
			Indices, CValueT<T, left>
			>>::type;
};

template<typename T, T left, typename Indices>
struct MakeIndices <T, left, left, Indices>{
	using type = Indices;
};

template<typename Ostream, typename Indices>
struct PrintValuelist_ {
	static Ostream& Impl(Ostream& os) 
	{ return os; }
};

template<typename T, typename Ostream, T... values>
struct PrintValuelist_<Ostream, Valuelist<T, values...>>{
	using Cur = Valuelist<T, values...>;

	static Ostream& Impl(Ostream& os, bool is_first = true) {
		os << ((is_first) ? "[" : ", ");
		os << TL::Front<Cur>::value;
		return PrintValuelist_<Ostream, TL::Pop_Front<Cur>>::Impl(os, false);
	}
};

template<typename T, typename Ostream>
struct PrintValuelist_<Ostream, Valuelist<T>>{
	using Cur = Valuelist<T>;

	static Ostream& Impl(Ostream& os, bool is_first=true){
		os << (is_first ? "[]" : "]");
		return os;
	}
};

template<typename Ostream, typename Indices>
Ostream& operator<<(Ostream& os, Indices){
	return PrintValuelist_<Ostream, Indices>::Impl(os);
}

template<typename T, T left, T right>
using IndexList = typename MakeIndices<T, left, right>::type;

template<typename ...Args>
void test1(Tuple<Args...> tuple, Args&&... args){

}

template<typename ...Args>
void test1(Tuple<Args...> tuple){
}

int main(){
	using vl1 = Valuelist<int, 1, -1>;
	std::cout << IndexList<int, -100, 100>{};
	std::cout << '\n' << IndexList<int, 100, 100>{};
}
