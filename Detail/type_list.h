#ifndef TYPE_LIST_H
#define TYPE_LIST_H

//typelist is a type container as a component of type metaprogramming
#include <cstdint>
#include <stl_type_traits_base.h>

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
// provide a typelist with sequences of compile-time values
///////////////////////////////////////////////////////////////
		template<typename T, T... Values>
		struct Valuelist {

		};


/////////////////////////////////////////////////
// class template CValue
// helper to wrap a compile-time value as a type(that is,type wrapper)
/////////////////////////////////////////////////
		template<typename T, T Value>
		struct CValueT {
			static constexpr T value=Value;
		};

/////////////////////////////////////////////		
// class template Nil
// represent a empty list
/////////////////////////////////////////////
		struct Nil {};


//////////////////////////////////////////////////
// class template Cons
// represent a Cons cell,contains two componet
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
//type list algorithm
		namespace TL {

///////////////////////////////////////////////////////////
// class template Select
// replace the TinySTL::Conditional,
// because TinySTL::Is_function has used the typelist
// Invocation(Cond is a pridicate ,T and F are types):
// Select<Cond,T,F>
///////////////////////////////////////////////////////////

			namespace detail {
				template<bool >
				struct Select_ {
					template<typename T,typename F>
					using type=T;
				};

				template<>
				struct Select_<false> {
					template<typename T,typename F>
					using type=F;
				};
			}

			template<bool Cond,typename T,typename F>
			using Select=typename detail::Select_<Cond>::template type<T, F>;

//////////////////////////////////////////////////////////
// class template Length
// obtains the number of elements of a typelist
// Invocation(List is a typelist):
// Length<List>
//////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Length_ {
					static constexpr int_ value=0;
				};

				template<typename ...Elems>
				struct Length_<Typelist<Elems...>> {
					static constexpr int_ value=sizeof...(Elems);
				};

				template<typename Head,typename Tail>
				struct Length_<Cons<Head, Tail>> {
					static constexpr int_ value=1+Length_<Tail>::value;
				};

				template<>
				struct Length_<Nil> {
					static constexpr int_ value=0;
				};

			}

			template<typename List>
			inline constexpr auto Length=detail::Length_<List>::value;

//////////////////////////////////////////////////////////
// class template Is_Empty
// get the Boolean value according to the typelist if empty
// Invocation(List is a typelist):
// Is_Empty<List>
//////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Is_Empty_ :_false_type{
				};

				template<>
				struct Is_Empty_<Typelist<>> :_true_type{
				};

				template<>
				struct Is_Empty_<Nil> :_true_type {};
			}

			template<typename T>
			inline constexpr bool Is_Empty=detail::Is_Empty_<T>::value;


/////////////////////////////////////////////////////////
// class template Front
// obtains the first element(type) of typelist
// Invocation(List is a typelist):
// Front<List> or FrontT<List>::type
//////////////////////////////////////////////////////////
			namespace detail{

//primary template for cons cell
				template<typename List>
				struct Front_ 
					:Type_identity<typename List::Head> {};

				template<typename Head, typename ...Tail>
				struct Front_<Typelist<Head, Tail...>> {
					using type=Head;
				};
			}

			template<typename List>
			using FrontT=detail::Front_<List>;

			template<typename List>
			using Front=typename detail::Front_<List>::type;

////////////////////////////////////////
// class template Pop_Front
// pop the first element of Typelist
// Invocation(List is a typelist):
// Pop_Front<List> or Pop_FrontT<List>::type
////////////////////////////////////////
			namespace detail {

//primary template for Cons cell				
				template<typename List>
				struct Pop_Front_ 
					:Type_identity<typename List::Tail>{};

				template<typename Head, typename ...Tail>
				struct Pop_Front_<Typelist<Head, Tail...>> {
					using type=Typelist<Tail...>;
				};
			}

			template<typename List>
			using Pop_FrontT=detail::Pop_Front_<List>;

			template<typename List>
			using Pop_Front=typename detail::Pop_Front_<List>::type;

////////////////////////////////////////////////////////////////////
// class template Push_Front
// appends the given type to the firts position of typelist
// Invocation(List is a typelist and NewElement is a type):
// Push_Front<List,NewElement> or Push_FrontT<List,NewElement>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
//primary template for Cons cell
				template<typename List, typename New>
				struct Push_Front_ 
					:Type_identity<Cons<New, List>>{};

				template<typename ...Elem, typename New>
				struct Push_Front_<Typelist<Elem...>, New> {
					using type=Typelist<New, Elem...>;
				};
			}     

			template<typename List,typename T>
			using Push_FrontT=detail::Push_Front_<List, T>;

			template<typename List, typename T>
			using Push_Front=typename detail::Push_Front_<List, T>::type;

//////////////////////////////////////////////////////////////////
// class template Push_Back
// appends the new element to the back of the typelist
// Invocation(List is a typelist and NewElem is a type):
// Push_Back<List,NewElem> or Push_BackT<List,NewElem>::type
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, typename New,
					bool=Is_Empty<List>>
				struct Push_Back_Rec;
				
				template<typename List,typename New>
				struct Push_Back_Rec<List, New, false> {
				private:
					using Head=Front<List>;
					using Tail=Pop_Front<List>;
					using NewTail=typename Push_Back_Rec<Tail, New>::type;
				public:
					using type=Push_Front<NewTail,Head>;
				};

				template<typename List,typename New>
				struct Push_Back_Rec<List, New, true> {
					using type=Push_Front<List, New>;
				};

				template<typename List, typename T>
				struct Push_Back_ 
					:Type_identity<Push_Back_Rec<List, T>>{};

				template<typename ...Elems, typename New>
				struct Push_Back_<Typelist<Elems...>, New> {
					using type=Typelist<Elems..., New>;
				};

			}

			template<typename List,typename T>
			using Push_BackT=detail::Push_Back_<List, T>;

			template<typename List, typename T>
			using Push_Back=typename detail::Push_Back_<List, T>::type;


////////////////////////////////////////////////////////////////////
// class template Reverse
// reverse the typelist
// Invocation(List is a typelist):
// Reverse<List> or ReverseT<List>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, bool=Is_Empty<List>>
				struct Reverse_;
				  
				/*template<typename List>
				struct Reverse_<List, false> {
					using Head=Front<List>;
					using type=Push_Back<typename Reverse_<Pop_Front<List>>::type, Head>;
				};*/

				template<typename List>
				struct Reverse_<List,false>
					:Push_BackT<typename Reverse_<Pop_Front<List>>::type,Front<List>>{};

				template<typename List>
				struct Reverse_<List, true> {
					using type=List;
				};
			}

			template<typename List>
			using ReverseT=detail::Reverse_<List>;

			template<typename List>
			using Reverse=typename detail::Reverse_<List>::type;

//////////////////////////////////////////////////////////////////
// class template Back
// obtains the last element of a typelist
// Invocation(List is a typelist):
// Back<List> or BackT<List>::type
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Back_ 
					:Type_identity<Front<Reverse<List>>>{};
			}

			template<typename List>
			using BackT=detail::Back_<List>;

			template<typename List>
			using Back=typename detail::Back_<List>::type;

//////////////////////////////////////////////////////////////////////
// class template Pop_Back
// removes the last element of a typelist
// Invocation(List is a typelist):
// Pop_Back<List> or Pop_BackT<List>::type
//////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List>
				struct Pop_Back_
					:Type_identity<Reverse<Pop_Front<Reverse<List>>>>{};

			}

			template<typename List>
			using Pop_BackT=detail::Pop_Back_<List>;

			template<typename List>
			using Pop_Back=typename detail::Pop_Back_<List>::type;

/////////////////////////////////////////////////////////////////////////////
// class template Type_At
// obtains the type at a given index in a typelist
// Invocation(List is a typelist and N is a compile-time integral value)
// Type_At<List,N> or Type_AtT<List,N>::type
/////////////////////////////////////////////////////////////////////////////
			namespace detail {
				
				template<typename List, int_ N>
				struct Type_At_ {
					static_assert(!Is_Empty<List>, "Typelist is empty!");
					
					using type=typename Type_At_<Pop_Front<List>, N-1>::type;
				};

				template<typename List>
				struct Type_At_<List, 0>
					:Type_identity<Front<List>> {};

				/*template<typename Head, typename ...Tail>
				struct Type_At_ <Typelist<Head, Tail...>, 0> {
					using type=Head;
				};

				template<typename Head, typename ...Tail, int_ N>
				struct Type_At_<Typelist<Head, Tail...>, N> {
					using type=typename Type_At_<Typelist<Tail...>, N-1>::type;
				};*/


			}

			template<typename List,int_ N>
			using Type_AtT=detail::Type_At_<List, N>;

			template<typename List, int_ N>
			using Type_At=typename detail::Type_At_<List, N>::type;

////////////////////////////////////////////////////
// class template Largest_Type
// find the largest type within a typelist
// Invocation(List is a type list):
// Largest_Type<List> or Largest_TypeT<List>::type
////////////////////////////////////////////////////
			namespace detail {
				template<typename List,bool =Is_Empty<List>>
				struct Largest_Type_ {
				};

				template<typename List>
				struct Largest_Type_<List, false> {
					using Head=Front<List>;
					using Rest=typename Largest_Type_<Pop_Front<List>>::type;
					
					using type=Select<(sizeof(Head)<sizeof(Rest)), Rest, Head>;
				};

				template<typename List>
				struct Largest_Type_<List, true> {
					using type=char;
				};
			}

			template<typename List>
			using Largest_TypeT=detail::Largest_Type_<List>;

			template<typename List>
			using Largest_Type=typename detail::Largest_Type_<List>::type;

////////////////////////////////////////////////////////////////////
// class template Transform
// takes a typelist and a metafunction and 
// produces another typelist containing the result
// of applying the metafunction to each type
// Invocation(List is a typelist and MetaFun is a metafunction):
// Transform<List,MetaFun> or TransformT<List,MetaFun>::type
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, template<typename > class MetaFun,
						bool=Is_Empty<List>>
				struct Transform_;

//generic version:fit for Cons cell	
				/*template<typename List,template<typename > class MetaFun>
				struct Transform_<List, MetaFun, false> {
					using Head=Front<List>;
					using Rest=typename Transform_<Pop_Front<List>,MetaFun>::type;
					using type=Push_Front<Rest, typename MetaFun<Head>::type>;

				};*/

				template<typename List,template<typename > class MetaFun>
				struct Transform_<List,MetaFun,false>
					:Push_Front<typename Transform_<Pop_Front<List>,MetaFun>::type,
								MetaFun<Front<List>>>{};

//rely on the Pack Expansions
				template<typename ...Elems,template<typename > class MetaFun>
				struct Transform_<Typelist<Elems...>, MetaFun, false> {
					using type=Typelist<typename MetaFun<Elems>::type...>;
				};

				template<typename List,template<typename > class MetaFun>
				struct Transform_<List, MetaFun, true> {
					using type=List;
				};
			}

			template<typename List,template<typename > class MetaFun>
			using TrasformT=detail::Transform_<List,MetaFun>;

			template<typename List, template<typename > class MetaFun>
			using Transform=typename detail::Transform_<List,MetaFun>::type;

/////////////////////////////////////////////////////////////////////////////
// class template Accumulate
// produces a single result by using metafunction to compute two types in turn 
// Invocation(List is a typelist,MetaFun is a metafunction,
//			  and Init is a initial type):
// Accumulate<List,MetaFun,Init> or AccumulateT<List,MetaFun,Init>::type
/////////////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List,template<typename ,typename > class MetaFun,typename I,
						bool =Is_Empty<List>>
				struct Accumulate_;


//if EN=null type,return the result of F(F(...F(T1,I),T2),...,TN-1),TN)
//Note:the former ellipsis indicates F(,the latter indicates T)

				/*template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I,false> {
					using Head=Front<List>;
					using Rest=Pop_Front<List>;
					using type=typename Accumulate_<Rest,MetaFun,typename MetaFun<Head, I>::type>::type;
				};*/

				//metafunction forwarding
				template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I, false> 
					:Accumulate_<Pop_Front<List>,MetaFun,
										typename MetaFun<Front<List>,I>::type>{};

				template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I,true> 
					:Type_identity<I>{};
			}

			template<typename List,template<typename ,typename > class MetaFun,typename I>
			using AccumulateT=detail::Accumulate_<List,MetaFun,I>;

			template<typename List,template<typename ,typename > class MetaFun,typename I>
			using Accumulate=typename detail::Accumulate_<List, MetaFun, I>::type;

////////////////////////////////////////////////////////////////////////////////////
// class template Sort
// sort a typelist through insertion Based on a Comparison
// Invocation(List is a typelist and Comp is a predicate for comparing elements):
// Sort<List,Comp> or SortT<List,Comp>::type
////////////////////////////////////////////////////////////////////////////////////

			namespace detail{

				template<typename List, template<typename ,typename > class Comp,
						bool =Is_Empty<List>>
				struct Sort_;

				template<typename List,template<typename ,typename > class Comp>
				using Sort_t=typename Sort_<List, Comp>::type;


//insert a type into an already-sorted list at the first point that will keep the list sorted				
				template<typename List,typename Elem,template<typename ,typename > class Comp,
						bool =Is_Empty<List>>
				struct Sorted_;

				template<typename List,template<typename ,typename > class Comp>
				struct Sort_<List, Comp, false> 
					: Sorted_<Sort_t<Pop_Front<List>,   Comp>,
							  Front<List>,Comp>{};

				template<typename List,template<typename ,typename > class Comp>
				struct Sort_<List, Comp, true> {
					using type=List;
				};

				template<typename List, typename Elem, template<typename, typename > class Comp,
					     bool=Is_Empty<List>>
				struct Sorted_;

				template<typename List,typename Elem,template<typename ,typename > class Comp>
				struct Sorted_<List, Elem, Comp,false> {
				private:	
					using NewTail=typename Select<
													Comp<
														 Elem,
														 Front<List>
													>::value,
												    Type_identity<List>,
												    Sorted_<Pop_Front<List>, Elem, Comp>
					>::type;

					using NewHead=Select<
										 Comp<
											  Elem,
											  Front<List>
										 >::value,
										 Elem,
										 Front<List>
					>;

				public:
					using type=Push_Front<NewTail, NewHead>;
				};

				/*template<typename List,typename Elem,template<typename ,typename> class Comp>
				struct Sorted_<List, Elem,Comp, false> {
					using type=Select<Comp<Elem, Front<List>>::value,
						Push_Front<List, Elem>,
						Push_Front<typename Sorted_<Pop_Front<List>, Elem, Comp>::type, Front<List>>>;
				};*/

				template<typename List,typename Elem,template<typename ,typename > class Comp>
				struct Sorted_<List, Elem, Comp,true> 
					:Push_FrontT<List,Elem>{};
				
			}
			template<typename List,template<typename ,typename > class Comp>
			using SortT=detail::Sort_<List,Comp>;

			template<typename List,template<typename ,typename > class Comp>
			using Sort=typename detail::Sort_<List, Comp>::type;

			
		}	//namespace TL
		
//For valuelist,provide some algorithms differ from previous defined
		namespace VL {
			using namespace TL;

			namespace detail { using namespace TL::detail; }

			
/////////////////////////////////////
// class template Length
// obtains the length of a valuelist
// Invocation(VL is a valuelist):
// Length<VL>
/////////////////////////////////////
			
			namespace detail {

				template<typename T,T ...Values>
				struct Length_<Valuelist<T,Values...>> {
					static constexpr int_ value=sizeof...(Values);
				};
			}

/////////////////////////////////////////////////////////
// class template Is_Empty
// obtains a Boolean value know if valuelist is empty
// Invocation(VL is a valuelist):
// Is_Empty<VL>
/////////////////////////////////////////////////////////

			namespace detail {

				template<typename T>
				struct Is_Empty_<Valuelist<T>>:_true_type {
				};
			}

//////////////////////////////////////////////
// class template Front
// obtains the first element of the valuelist
// Invocation(VL is a valuelist):
// Front<VL>::value
//////////////////////////////////////////////

			namespace detail {

				template<typename T, T Head, T ...Values>
				struct Front_<Valuelist<T, Head, Values...>> {
					using type=CValueT<T, Head>;
				};
			}

///////////////////////////////////////////////////////////
// class template Push_Front
// appends the new value to a valuelist as the first element
// Invocation(VL is a valuelist type ,T is a type of the valuelist element 
//			  and New is a value):
// Push_FrontT<VL,CValueT<T,New>>::type or Push_Front<VL,CValueT<T,New>>
///////////////////////////////////////////////////////////

			namespace detail {

				template<typename T,T... Values,T value>
				struct Push_Front_<Valuelist<T, Values...>, CValueT<T,value>> {
					using type=Valuelist<T, value, Values...>;
				};
			}


////////////////////////////////////////////////////////////
// class template Pop_Front
// erase the first element of a valuelist
// Invocation(VL is a valuelist):
// Pop_FrontT<VL>::type or Pop_Front<VL>
////////////////////////////////////////////////////////////

			namespace detail {

				template<typename T,T value,T ...Values>
				struct Pop_Front_<Valuelist<T, value, Values...>> {
					using type=Valuelist<T, Values...>;
				};
			}

////////////////////////////////////////////////////////////
// class template Push_Back
// appends the new value to the last of the valuelist
// Invocation(VL is a valuelist and New is a value):
// Push_BackT<VL>::type or Push_Back<VL>
////////////////////////////////////////////////////////////

			namespace detail {

				template<typename T,T value,T... Values>
				struct Push_Back_<Valuelist<T, Values...>, CValueT<T,value>> {
					using type=Valuelist<T, Values..., value>;
				};
			}

		}	//namespace VL
	}	//namespace mpl
}	//namespace TinySTL


#endif //TYPE_LIST_H