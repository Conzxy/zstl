////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//typelist is a type container as a component of type metaprogramming
//Construction:
//1. Typelist</Types.../>							(general)
//2. Cons<T1,Cons<T2,Cons<T3,...,Cons<TN>...>>		(Cons cell)
//3. Valuelist<Type,Values...>						(valuelist,the first parameter is a type,and then there are some values)

//The provided algorithm as following:
//1. Push_Front
//2. Pop_Front
//3. Push_Back
//4. Pop_Back
//5. Front
//6. Back
//7. Length
//8. Is_Empty
//9. Type_At
//10.Reverse
//11.Transform
//12.Accumulate
//13.Sort
//14.Erase
//15.Erase_All
//16.Replace
//17.Replace_All
//18.Index_Of
//19.Unique
//20.Largest_Type(also can be implemented by the Accumulate algorithm)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include <cstdint>
#include <type_traits.h>

namespace TinySTL {
	namespace mpl {

		namespace detail {
			using int_=std::intmax_t;
		}

		using int_=detail::int_;

///////////////////////////////////////////////////////////
// class template Typelist
// provide typelist container
///////////////////////////////////////////////////////////
		template<typename ...Elements>
		struct Typelist {};

///////////////////////////////////////////////////////////////
// class template Valuelist
// provide a typelist with sequence of compile-time values
///////////////////////////////////////////////////////////////
		template<typename T, T... Values>
		struct Valuelist {};


//////////////////////////////////////////////////////////////////////////////
// class template CValue
// helper to wrap a compile-time value as a type(that is,type wrapper)
//////////////////////////////////////////////////////////////////////////////
		template<typename T, T Value>
		struct CValueT {
			static constexpr T value=Value;
		};

/////////////////////////////////////////////		
// class template Nil
// represents a empty list
/////////////////////////////////////////////
		struct Nil {};


//////////////////////////////////////////////////
// class template Cons
// represents a Cons cell,contains two componet
// which hold two types
//////////////////////////////////////////////////
		template<typename HeadT, typename TailT=Nil>
		struct Cons {
			using Head=HeadT;
			using Tail=TailT;
		};

		template<typename Type>
		struct Type_identity {
			using type=Type;
		};
	}	//namespace mpl
}	//namespace TinySTL

#endif //TYPE_LIST_H