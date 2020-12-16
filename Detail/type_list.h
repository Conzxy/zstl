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
			}

			template<typename T>
			inline constexpr bool Is_Empty=detail::Is_Empty_<T>::value;


/////////////////////////////////////////////////////////
// class template Front
// obtains the first element(type) of typelist
// Invocation(List is a typelist):
// Front<List>
//////////////////////////////////////////////////////////
				namespace detail{
					template<typename List>
					struct Front_ {
					};

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
// Pop_Front<List>
////////////////////////////////////////
			namespace detail {
				template<typename List>
				struct Pop_Front_ {
					static_assert(!Is_Empty<List>,"Typelist is empty!");
				};

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
// Push_Front<List,T>
////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List, typename T>
				struct Push_Front_ {};

				template<typename ...Elem, typename T>
				struct Push_Front_<Typelist<Elem...>, T> {
					using type=Typelist<T, Elem...>;
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
// Push_Back<List,NewElem>
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, typename T>
				struct Push_Back_ {};

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
// Reverse<List>
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List, bool=Is_Empty<List>>
				struct Reverse_;

				template<typename List>
				struct Reverse_<List, false> {
					using Head=Front<List>;
					using type=Push_Back<typename Reverse_<Pop_Front<List>>::type, Head>;
				};

				template<typename List>
				struct Reverse_<List, true> {
					using type=List;
				};
			}

			template<typename List>
			using Reverse_=detail::Reverse_<List>;

			template<typename List>
			using Reverse=typename detail::Reverse_<List>::type;

//////////////////////////////////////////////////////////////////
// class template Back
// obtains the last element of a typelist
// Invocation(List is a typelist):
// Back<List>
//////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List>
				struct Back_ {
					static_assert(!Is_Empty<List>, "Typelist is empty!");
				};

				template<typename ...Elems>
				struct Back_<Typelist<Elems...>> {
					using type=Front<Reverse<Typelist<Elems...>>>;
				};
			}

			template<typename List>
			using BackT=detail::Back_<List>;

			template<typename List>
			using Back=typename detail::Back_<List>::type;

//////////////////////////////////////////////////////////////////////
// class template Pop_Back
// removes the last element of a typelist
// Invocation(List is a typelist):
// Pop_Back<List>
//////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List>
				struct Pop_Back_ {
					static_assert(!Is_Empty<List>, "Typelist is empty!");
				};

				template<typename ...Elems>
				struct Pop_Back_<Typelist<Elems...>> {
					using type=Reverse<Pop_Front<Reverse<Typelist<Elems...>>>>;
				};

			}

			template<typename List>
			using Pop_BackT=detail::Pop_Back_<List>;

			template<typename List>
			using Pop_Back=typename detail::Pop_Back_<List>::type;

/////////////////////////////////////////////////////////////////////////////
// class template Type_At
// obtains the type at a given index in a typelist
// Invocation(List is a typelist and N is a compile-time integral value)
// Type_At<List,N>
/////////////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List, int_ N>
				struct Type_At_ {
					static_assert(!Is_Empty<List>, "Typelist is empty!");
				};

				template<typename Head, typename ...Tail>
				struct Type_At_ <Typelist<Head, Tail...>, 0> {
					using type=Head;
				};

				template<typename Head, typename ...Tail, int_ N>
				struct Type_At_<Typelist<Head, Tail...>, N> {
					using type=typename Type_At_<Typelist<Tail...>, N-1>::type;
				};
			}

			template<typename List,int_ N>
			using Type_AtT=detail::Type_At_<List, N>;

			template<typename List, int_ N>
			using Type_At=typename detail::Type_At_<List, N>::type;

////////////////////////////////////////////////////
// class template Largest_Type
// find the largest type within a typelist
// Invocation(List is a type list):
// Largest_Type<List>
////////////////////////////////////////////////////
			namespace detail {
				template<typename List,bool =Is_Empty<List>>
				struct Largest_Type_ {
					static_assert(!Is_Empty<List>, "Typelist is empty!");
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
// Transform<List,MetaFun>
////////////////////////////////////////////////////////////////////
			namespace detail {
				template<typename List, template<typename > class MetaFun,
						bool=Is_Empty<List>>
				struct Transform_;

				/*template<typename List,template<typename > class MetaFun>
				struct Transform_<List, MetaFun, false> {
					using Head=Front<List>;
					using Rest=typename Transform_<Pop_Front<List>,MetaFun>::type;
					using type=Push_Front<Rest, typename MetaFun<Head>::type>;

				};*/

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

////////////////////////////////////////////////////////////////////
// class template Accumulate
// Combines all the elements of a typelist into a single result
// Invocation(List is a typelist,MetaFun is a metafunction,
//			  and Init is a initial type):
// Accumulate<List,MetaFun,Init>
////////////////////////////////////////////////////////////////////

			namespace detail {
				template<typename List,template<typename ,typename > class MetaFun,typename I,
						bool =Is_Empty<List>>
				struct Accumulate_;

				//F(EN,F(...,F(E3,F(E2,F(E1,I)))))
				//if EN=null type,return the result of F(...,F(E3,F(E2,F(E1,I)))) 
				/*template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I,false> {
					using Head=Front<List>;
					using Rest=Pop_Front<List>;
					using type=typename Accumulate_<Rest,MetaFun,typename MetaFun<Head, I>::type>::type;
				};*/

				template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I, false> 
					:Accumulate_<Pop_Front<List>,MetaFun,
										typename MetaFun<Front<List>,I>::type>{};

				template<typename List,template<typename ,typename > class MetaFun,typename I>
				struct Accumulate_<List, MetaFun, I,true> {
					using type=I;
				};
			}

			template<typename List,template<typename ,typename > class MetaFun,typename I>
			using AccumulateT=detail::Accumulate_<List,MetaFun,I>;

			template<typename List,template<typename ,typename > class MetaFun,typename I>
			using Accumulate=typename detail::Accumulate_<List, MetaFun, I>::type;

////////////////////////////////////////////////////////////////////////////////////
// class template Sort
// sort a typelist through insertion Based on a Comparison
// Invocation(List is a typelist and Comp is a predicate for comparing elements):
// Sort<List,Comp>
////////////////////////////////////////////////////////////////////////////////////

			namespace detail{
				template<typename Type>
				struct Type_identity {
					using type=Type;
				};

				template<typename List, template<typename ,typename > class Comp,
						bool =Is_Empty<List>>
				struct Sort_;

				template<typename List,template<typename ,typename > class Comp>
				using Sort_t=typename Sort_<List, Comp>::type;

				template<typename List,typename Elem,template<typename ,typename > class Comp,
						bool =Is_Empty<List>>
				struct Sorted_;

				template<typename List,template<typename ,typename > class Comp>
				struct Sort_<List, Comp, false> 
					: Sorted_<Sort_t<Pop_Front<List>,Comp>,
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

				template<typename List,typename Elem,template<typename ,typename > class Comp>
				struct Sorted_<List, Elem, Comp,true> 
					:Push_FrontT<List,Elem>{};
				
			}
			template<typename List,template<typename ,typename > class Comp>
			using SortT=detail::Sort_<List,Comp>;

			template<typename List,template<typename ,typename > class Comp>
			using Sort=typename detail::Sort_<List, Comp>::type;

		}	//namespace TL

///////////////////////////////////////////////////////////////
// class template Valuelist
// provide a typelist with sequences of compile-time values
///////////////////////////////////////////////////////////////
		template<typename T,T... Values>
		struct Valuelist{};


	}	//namespace mpl
}	//namespace TinySTL


#endif //TYPE_LIST_H