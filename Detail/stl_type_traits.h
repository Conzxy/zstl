#ifndef TINYSTL_STL_TYPE_TRAITS_H
#define TINYSTL_STL_TYPE_TRAITS_H

//该文件实现了一部分标准库traits（×），templates练习（√）
#include <cstddef>
#include <cstdint>
#include <stl_type_traits_base.h>

#define DEFINE_HAS_TYPE(Memtype)						        \
template<typename,typename =Void_t<>>				            \
struct HasTypeT_##Memtype							            \
:_false_type{};										            \
template<typename T>								            \
struct HasTypeT_##Memtype<T,Void_t<typename T::Memtype>>		\
:_true_type{};                                                  \

	
#define DEFINE_HAS_MEMBER(Member)                               \
template<typename,typename =Void_t<>>                           \
struct HasMemberT_##Member                                      \
:_false_type{};                                                 \
template<typename T>                                            \
struct HasMemberT_##Member<T,Void_t<decltype(&T::Member)>>      \
:_true_type{};                                                  \

#define DEFINE_INTEGRAL(INT_TYPE)							\
template<>													\
struct Is_integral_<INT_TYPE>								\
:_true_type {												\
};															\

#define DEFINE_FLOATING_POINT(TYPE)									\
template<>															\
struct Is_floating_point_<TYPE>:_true_type{};						\

#define DEFINE_IS_FUNCTION(TYPE)									\
template<typename R, typename ...Paras>								\
struct Is_function_<R(Paras...) TYPE> :_true_type {					\
	using Return=R;													\
	static constexpr bool variadic=false;							\
};																	\

#define DEFINE_IS_FUNCTION_VARG(TYPE)								\
template<typename R, typename ...Paras>								\
struct Is_function_<R(Paras..., ...) TYPE> :_true_type {			\
	using Return=R;													\
	static constexpr bool variadic=true;							\
};																	\

namespace TinySTL {

	namespace detail {
		using int_=std::intmax_t;

		template<typename T>
		struct Remove_const_;

		template<typename T>
		struct Remove_volatile_;

		template<typename T>
		struct Remove_reference_;

		template<typename T>
		struct Add_lvalue_reference_;

		template<typename T>
		struct Add_rvalue_reference_;
	}

	using int_=detail::int_;

	template<typename T>
	using Remove_reference_t=typename detail::Remove_reference_<T>::type;

	template<typename T>
	using Remove_const_t=typename detail::Remove_const_<T>::type;

	template<typename T>
	using Remove_volatile_t=typename detail::Remove_volatile_<T>::type;

	template<typename T>
	using Remove_cv_t=Remove_const_t<Remove_volatile_t<T>>;

	template<typename T>
	using Add_lvalue_reference_t=typename detail::Add_lvalue_reference_<T>::type;

	template<typename T>
	using Add_rvalue_reference_t=typename detail::Add_rvalue_reference_<T>::type;

	//dummy object
	template<typename T>
	Add_rvalue_reference_t<T> declval()noexcept;

	template<typename...>
	using Void_t=void;

	namespace detail {
		template<bool>
		struct Conditional_;
	}

	template<bool Cond, typename T, typename F>
	using Conditional_t=typename detail::Conditional_<Cond>::template type<T, F>;

	//_or_,_and_可用于组合谓词
	template<typename ...>
	struct _or_ {};

	template<>
	struct _or_<> :_false_type {};

	template<typename T>
	struct _or_ <T> :T {};

	template<typename T1, typename T2>
	struct _or_<T1, T2> :Conditional_t<T1::value, T1, T2> {};

	template<typename T1, typename T2, typename T3, typename...Tn>
	struct _or_<T1, T2, T3, Tn...> :Conditional_t<T1::value, T1, _or_<T2, T3, Tn...>> {};

	template<typename...>
	struct _and_ {};

	template<>
	struct _and_<> :_true_type {};

	template<typename T>
	struct _and_<T> :T {};

	template<typename T1, typename T2>
	struct _and_<T1, T2> :Conditional_t<T1::value, T2, T1> {};

	template<typename T1, typename T2, typename T3, typename ...Tn>
	struct _and_<T1, T2, T3, Tn...> :Conditional_t<T1::value, _and_<T2, T3, Tn...>, T1> {};

	template<typename T>
	struct _not_ :Bool_constant<!bool(T::value)> {};

	//conjunction,disjunction,negation
	template<typename...Tn>
	struct Conjunction :_and_<Tn...> {};

	template<typename...Tn>
	struct Disjunction :_or_<Tn...> {};

	template<typename T>
	struct Negation :_not_<T> {};

	template<typename...Tn>
	inline constexpr bool Conjunction_v=Conjunction<Tn...>::value;

	template<typename...Tn>
	inline constexpr bool Disjunction_v=Disjunction<Tn...>::value;

	template<typename T>
	inline constexpr bool Negation_v=Negation<T>::value;

	//////////////////////////////////////
	///Check the Primary Type Category////
	//////////////////////////////////////
	namespace detail {
		template<typename T>
		struct Is_void_helper
			:public _false_type {};

		template<>
		struct Is_void_helper<void>
			:public _true_type {};

	}

	template<typename T>
	using Is_void=typename detail::Is_void_helper<Remove_cv_t<T>>::type;

	namespace detail {
		template<typename T>
		struct Is_integral_ :_false_type {};

		DEFINE_INTEGRAL(bool);
		DEFINE_INTEGRAL(char);
		DEFINE_INTEGRAL(unsigned char);
		DEFINE_INTEGRAL(signed char);
		DEFINE_INTEGRAL(wchar_t);
		DEFINE_INTEGRAL(char16_t);
		DEFINE_INTEGRAL(char32_t);

		DEFINE_INTEGRAL(signed short);
		DEFINE_INTEGRAL(unsigned short);
		DEFINE_INTEGRAL(signed int);
		DEFINE_INTEGRAL(unsigned int);
		DEFINE_INTEGRAL(signed long);
		DEFINE_INTEGRAL(unsigned long);
		DEFINE_INTEGRAL(signed long long);
		DEFINE_INTEGRAL(unsigned long long);
	}

	template<typename T>
	using Is_integral=typename detail::Is_integral_<Remove_cv_t<T>>::type;

	namespace detail {
		template<typename T>
		struct Is_floating_point_ :_false_type {};

		DEFINE_FLOATING_POINT(double);
		DEFINE_FLOATING_POINT(double long);
		DEFINE_FLOATING_POINT(float);
	}

	template<typename T>
	using Is_floating_point=typename detail::Is_floating_point_<Remove_cv_t<T>>::type;


	namespace detail {
		template<typename T>
		struct Is_array_ :_false_type {};

		template<typename T>
		struct Is_array_<T[]> :_true_type {};

		template<typename T, int_ sz>
		struct Is_array_<T[sz]> :_true_type {};
	}

	template<typename T>
	using Is_array=typename detail::Is_array_<T>::type;


	namespace detail {
		template<typename T>
		struct Is_pointer_ :_false_type {
		};

		template<typename T>
		struct Is_pointer_ <T*> :_true_type {
		};

	}

	template<typename T>
	using Is_pointer=typename detail::Is_pointer_<Remove_cv_t<T>>::type;

	namespace detail {
		template<typename T>
		struct Is_null_pointer_ :_false_type {
		};

		template<>
		struct Is_null_pointer_<std::nullptr_t> :_true_type {};
	}

	template<typename T>
	using Is_null_pointer=typename detail::Is_null_pointer_<Remove_cv_t<T>>::type;

	namespace detail {
		template<typename T>
		struct Is_member_object_pointer_ :_false_type {
		};

		template<typename C, typename T>
		struct Is_member_object_pointer_<T C::*> :_true_type {
			using Member=T;
			using Class=C;
		};
	}

	template<typename T>
	using Is_member_object_pointer=typename detail::Is_member_object_pointer_<Remove_cv_t<T>>::type;

	namespace detail {

		template<typename T>
		struct Is_function_;

		template<typename T>
		struct Is_member_function_pointer_ :_false_type {};

		template<typename F, typename C>
		struct Is_member_function_pointer_<F C::*> :Is_function_<F> {
			using Member=F;
			using Return=typename Is_function_<F>::Return;
			using Class=C;
			static constexpr bool variadic=Is_function_<F>::variadic;
		};
	}

	template<typename T>
	using Is_member_function_pointer=typename detail::Is_member_function_pointer_<Remove_cv_t<T>>::type;

	template<typename T, bool=Is_member_function_pointer<T>::value>
	struct member_function_pointer_traits {
		using Member=typename detail::Is_member_function_pointer_<T>::Member;
		using Return=typename detail::Is_member_function_pointer_<T>::Return;
		using Class=typename detail::Is_member_function_pointer_<T>::Class;
		static constexpr bool variadic=detail::Is_member_function_pointer_<T>::variadic;
	};

	template<typename T>
	struct member_function_pointer_traits<T, false> {
	};

	namespace detail {
		template<typename T>
		struct Is_lvalue_reference_ :_false_type {};

		template<typename T>
		struct Is_lvalue_reference_<T&> :_true_type {};

		template<typename T>
		struct Is_rvalue_reference_ :_false_type {};

		template<typename T>
		struct Is_rvalue_reference_<T&&> :_true_type {};
	}

	template<typename T>
	using Is_lvalue_reference=typename detail::Is_lvalue_reference_<T>::type;

	template<typename T>
	using Is_rvalue_reference=typename detail::Is_rvalue_reference_<T>::type;

	namespace detail {
		template<typename T, typename=Void_t<>>
		struct Is_class_ :_false_type {};

		template<typename T>
		struct Is_class_<T, Void_t<int T::*>>
			:_true_type {};
	}

	template<typename T>
	using Is_class=typename detail::Is_class_<T>::type;

	namespace detail {
		template<typename T>
		struct Is_function_ :_false_type {};

		//total 48 partial specializations
		//regular 
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...)> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...)> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		//cv
		DEFINE_IS_FUNCTION(const);
		DEFINE_IS_FUNCTION(volatile);
		DEFINE_IS_FUNCTION(const volatile);

		//&/&&
		DEFINE_IS_FUNCTION(&);
		DEFINE_IS_FUNCTION(const&);
		DEFINE_IS_FUNCTION(volatile&);
		DEFINE_IS_FUNCTION(const volatile&);
		DEFINE_IS_FUNCTION(&&);
		DEFINE_IS_FUNCTION(const&&);
		DEFINE_IS_FUNCTION(volatile&&);
		DEFINE_IS_FUNCTION(const volatile&&);

		//noexcept
		DEFINE_IS_FUNCTION(noexcept);
		DEFINE_IS_FUNCTION(const noexcept);
		DEFINE_IS_FUNCTION(volatile noexcept);
		DEFINE_IS_FUNCTION(const volatile noexcept);
		DEFINE_IS_FUNCTION(&noexcept);
		DEFINE_IS_FUNCTION(const& noexcept);
		DEFINE_IS_FUNCTION(volatile& noexcept);
		DEFINE_IS_FUNCTION(const volatile& noexcept);
		DEFINE_IS_FUNCTION(&& noexcept);
		DEFINE_IS_FUNCTION(const&& noexcept);
		DEFINE_IS_FUNCTION(volatile&& noexcept);
		DEFINE_IS_FUNCTION(const volatile&& noexcept);

		//variadic
		DEFINE_IS_FUNCTION_VARG(volatile);
		DEFINE_IS_FUNCTION_VARG(const volatile);
		DEFINE_IS_FUNCTION_VARG(&);
		DEFINE_IS_FUNCTION_VARG(const&);
		DEFINE_IS_FUNCTION_VARG(volatile&);
		DEFINE_IS_FUNCTION_VARG(const volatile&);
		DEFINE_IS_FUNCTION_VARG(&&);
		DEFINE_IS_FUNCTION_VARG(const&&);
		DEFINE_IS_FUNCTION_VARG(volatile&&);
		DEFINE_IS_FUNCTION_VARG(const volatile&&);
		DEFINE_IS_FUNCTION_VARG(noexcept);
		DEFINE_IS_FUNCTION_VARG(const noexcept);
		DEFINE_IS_FUNCTION_VARG(volatile noexcept);
		DEFINE_IS_FUNCTION_VARG(const volatile noexcept);
		DEFINE_IS_FUNCTION_VARG(&noexcept);
		DEFINE_IS_FUNCTION_VARG(const& noexcept);
		DEFINE_IS_FUNCTION_VARG(volatile& noexcept);
		DEFINE_IS_FUNCTION_VARG(const volatile& noexcept);
		DEFINE_IS_FUNCTION_VARG(&& noexcept);
		DEFINE_IS_FUNCTION_VARG(const&& noexcept);
		DEFINE_IS_FUNCTION_VARG(volatile&& noexcept);
		DEFINE_IS_FUNCTION_VARG(const volatile&& noexcept);
		DEFINE_IS_FUNCTION_VARG(const);
	}

	template<typename T>
	using Is_function=typename detail::Is_function_<T>::type;

	template<typename T, bool=Is_function<T>::value>
	struct function_traits {
		using Return=typename detail::Is_function_<T>::Return;
		static constexpr bool variadic=detail::Is_function_<T>::variadic;
	};

	template<typename T>
	struct function_traits<T, false> {
	};

	namespace detail {
		template<typename T>
		using Is_enum_=Disjunction<Is_void<T>, Is_null_pointer<T>, Is_integral<T>, Is_floating_point<T>,
			Is_array<T>, Is_pointer<T>, Is_class<T>,
			Is_lvalue_reference<T>, Is_rvalue_reference<T>,
			Is_member_object_pointer<T>, Is_member_function_pointer<T>,
			Is_function<T>>;
	}

	template<typename T>
	using Is_enum=typename Negation<detail::Is_enum_<T>>::type;

	/////////////////////////////////////
	///Composite type categories/////////
	/////////////////////////////////////
	template<typename T>
	using Is_arithmetic=typename Disjunction<Is_integral<T>, Is_floating_point<T>>::type;

	template<typename T>
	using Is_fundamental=typename Disjunction<Is_arithmetic<T>, Is_void<T>, Is_null_pointer<T>>::type;

	template<typename T>
	using Is_member_pointer=typename Disjunction<Is_member_object_pointer<T>, Is_member_function_pointer<T>>::type;

	template<typename T>
	using Is_scalar=typename Disjunction<Is_arithmetic<T>, Is_pointer<T>, Is_member_pointer<T>,
		Is_enum<T>, Is_null_pointer<T>>::type;

	template<typename T>
	using Is_reference=typename Disjunction<Is_lvalue_reference<T>, Is_rvalue_reference<T>>::type;


	//template<typename T>
	//using Is_object=typename Disjunction<Is_scalar<T>, Is_array<T>, Is_class<T>>::type;

	//why I use this version as following?
	//just because I can't write the "Is_union" though there are instantiations of Is_function...
	namespace detail {
		template<typename T>
		using Is_object_=typename Conjunction<Is_void<T>, Is_reference<T>, Is_function<T>>::type;
	}

	template<typename T>
	using Is_object=typename Negation<detail::Is_object_<T>>::type;

	//fundamemtal include arithmetic,null_pointer,void
	//others are compound types
	//this is,array,function,union,reference,pointer,pointer to member,class,union,enumeration
	template<typename T>
	using Is_compound=typename Negation<Is_fundamental<T>>::type;

	////////////////////////
	///Type properties//////
	////////////////////////
	namespace detail {
		template<typename T>
		struct Is_const_ :_false_type {
		};

		template<typename T>
		struct Is_const_ <T const> :_true_type {
		};

		template<typename T>
		struct Is_const_<T const*> :_true_type {
		};
	}

	template<typename T>
	using Is_const=typename detail::Is_const_<T>::type;

	namespace detail {
		template<typename T>
		struct Is_volatile_ :_false_type {};

		template<typename T>
		struct Is_volatile_<T volatile> :_true_type {};

		template<typename T>
		struct Is_volatile_<T volatile*> :_true_type {};
	}

	template<typename T>
	using Is_volatile=typename detail::Is_volatile_<T>::type;

	//Is_trivial
	//Is_trivially_copyable
	//Is_standard_layout
	//Is_literal_type

	template<typename T>
	using Is_pod=typename _type_traits<T>::is_POD_type;

	template<typename T>
	using Is_trivially_default_constructible=typename _type_traits<T>::has_trivially_default_constructor;;

	template<typename T>
	using Is_trivially_copy_constructible=typename _type_traits<T>::has_trivially_copy_constructble;

	template<typename T>
	using Is_trivially_assigment=typename _type_traits<T>::has_trivially_assigment_operator;

	template<typename T>
	using Is_trivially_destructible=typename _type_traits<T>::has_trivially_destructor;

	namespace detail {
		template<typename T, typename=Void_t<>>
		struct Is_empty_ :_false_type {};

		template<typename T>
		struct Is_empty_<T, Void_t<decltype(sizeof(T)==1)>> :_true_type {};
	}

	template<typename T>
	using Is_empty=typename detail::Is_empty_<T>::type;

	namespace detail {
		//use the property of dynamic_cast
		template<typename T>
		_true_type Is_polymorphic_(
			decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr)))
		);

		template<typename T>
		_false_type Is_polymorphic_(...);
	}

	template<typename T>
	using Is_polymorphic=decltype(detail::Is_polymorphic_<T>(nullptr));

	namespace detail {
		template<typename T, typename=Conjunction<Is_class<T>, Is_polymorphic<T>>>
		struct Is_abstract_ :_true_type {};

		template<typename T>
		struct Is_abstract_<T, _false_type> :_false_type {};
	}

	template<typename T>
	using Is_abstract=typename detail::Is_abstract_<T>::type;

	//Is_aggregate
	//Is_fianl

	namespace detail {
		template<typename T, bool=Is_arithmetic<T>::value>
		struct Is_signed_ :Bool_constant<(bool)(T{ -1 }<T{ 0 })>{};

		template<typename T>
		struct Is_signed_<T, false> :_false_type {};
	}

	template<typename T>
	using Is_signed=typename detail::Is_signed_<T>::type;

	namespace detail {
		template<typename T, bool=Is_arithmetic<T>::value>
		struct Is_unsigned_ :Bool_constant<(bool)(T{ 0 }<T{ -1 })>{};

		template<typename T>
		struct Is_unsigned_<T, false> :_false_type {};
	}

	template<typename T>
	using Is_unsigned=typename detail::Is_unsigned_<T>::type;

	namespace detail {
		template<typename T>
		struct Is_bounded_array_ :_false_type {};

		template<typename T, int_ sz>
		struct Is_bounded_array_<T[sz]> :_true_type {
		};

		template<typename T>
		struct Is_unbounded_array_ :_false_type {};

		template<typename T>
		struct Is_unbounded_array_<T[]> :_true_type {};
	}

	template<typename T>
	using Is_bounded_array=typename detail::Is_bounded_array_<T>::type;

	template<typename T>
	using Is_unbounded_array=typename detail::Is_unbounded_array_<T>::type;

	/*namespace detail {
		template<typename T,bool =Is_enum<T>::value>
		struct Is_scoped_enum:Bool_constant<Is_convertible<T,std::underling_type_t<T>>::value>{};

		template<typename T>
		struct Is_scoped_enum<T, false> :_false_type {};
	}*/

	////////////////////////
	///Property quries//////
	////////////////////////
	namespace detail {
		template<typename T>
		struct Rank_ {
			static constexpr int_ value=0;
		};

		template<typename T>
		struct Rank_<T[]> {
			static constexpr int_ value=Rank<T>::value+1;
		};

		template<typename T, int_ sz>
		struct Rank_<T[sz]> {
			static constexpr int_ value=Rank<T>::value+1;
		};
	}

	template<typename T>
	using Rank=detail::Rank_<T>;

	template<typename T>
	inline constexpr auto Rank_v=detail::Rank_<T>::value;

	namespace detail {
		template<typename T, int_ N>
		struct Extent_ {
			static constexpr int_ value=0;
		};

		template<typename T>
		struct Extent_<T[], 0> {
			static constexpr int_ value=0;
		};

		template<typename T, int_ N>
		struct Extent_<T[], N> {
			static constexpr int_ value=extent<T, N-1>::value;
		};
		template<typename T, int_ sz>
		struct Extent_<T[sz], 0> {
			static constexpr int_ value=sz;
		};

		template<typename T, int_ N, int_ sz>
		struct Extent_<T[sz], N> {
			static constexpr int_ value=extent<T, N-1>::value;
		};
	}

	template<typename T, int_ N>
	using Extent=detail::Extent_<T, N>;

	template<typename T, int_ N>
	inline constexpr auto Extent_v=detail::Extent_<T, N>::value;



	////////////////////////
	///Supported operations/
	////////////////////////

	namespace detail {
		//generic lambda version
		//helper:check validity of f(args...) for F f and Args...args
		template<typename F, typename...Args, typename=
			decltype(declval<F>()(declval<Args&&>()...))>
			_true_type Is_valid_impl(void*);

		template<typename F, typename...Args>
		//template<typename ...args>
		_false_type Is_valid_impl(...);

		//traits factory
		inline constexpr
			auto Is_valid=[](auto f) {
			return [](auto&&... Args) {
				return decltype(Is_valid_impl<decltype(f),
					decltype(Args)&& ...
				>(nullptr)){};
			};
		};

		//helper template to present a type as a value
		template<typename T>
		struct typeT {
			using type=T;
		};

		//helper tp wrap a type as a value
		template<typename T>
		constexpr auto type=typeT<T>{};

		//helper to unwrap a wrapped type in unevaluated context
		template<typename T>
		T valueT(typeT<T>);  //no definition needed

		//typeT--->[valueT]--->T

		
		constexpr auto Is_default_constructible_
			=Is_valid([](auto x)->decltype((void)decltype(
				valueT(x))()) {
				});


		constexpr auto Is_destructible_
			=Is_valid([](auto x)->decltype((void)(declval<decltype(
				valueT(x))>().~decltype(valueT(x))())) {
				});

		constexpr auto Is_constructible_
			=Is_valid([](auto x,auto ...args)->decltype((void)(decltype(
				valueT(x))(declval<decltype(valueT(args))>()...))) {
				});

		constexpr auto Is_assignable_
			=Is_valid([](auto x, auto y)->decltype((void)(
				declval<decltype(valueT(x))>() = declval<decltype(valueT(y))>())) {
				});

	}
	//type
	template<typename T>
	using Is_default_constructible
		=decltype(detail::Is_default_constructible_(detail::type<T>));

	template<typename T,typename ...Types>
	using Is_constructible
		=decltype(detail::Is_constructible_(detail::type<T>,detail::type<Types>...));

	template<typename T>
	using Is_copy_constructible
		=Is_constructible<T, T const&>;

	template<typename T>
	using Is_move_constructible
		=Is_constructible<T, T&&>;


	template<typename T,typename U>
	using Is_assignable
		=decltype(detail::Is_assignable_(detail::type<T>, detail::type<U>));

	template<typename T>
	using Is_copy_assignable
		=Is_assignable<T,const T&>;

	template<typename T>
	using Is_move_assignable
		=Is_assignable<T,T&&>;

	template<typename T>
	using Is_destructible
		=decltype(detail::Is_destructible_(detail::type<T>));

	//type-nothrow
	template<typename T,typename =Is_default_constructible<T>>
	struct Is_nothrow_default_constructible
		:Bool_constant<noexcept(T{})>{};

	template<typename T>
	struct Is_nothrow_default_constructible <T, _false_type>
		:_false_type {};

	template<typename T,typename =Is_destructible<T>>
	struct Is_nothrow_destructible
		:Bool_constant<noexcept(declval<T>().~T())> {};

	template<typename T>
	struct Is_nothrow_destructible<T,_false_type>
		:_false_type {};

	namespace detail {
		template<typename T, typename ...Types>
		struct Is_nothrow_constructible_helper {
		private:
			template<typename =Is_constructible<T>>
			struct test :Bool_constant<noexcept(T(declval<Types>()...))> {};

			template<>
			struct test <_false_type> :_false_type {};
		public:
			using type=test<>;

		};
	}

	template<typename T,typename ...Types>
	struct Is_nothrow_constructible 
		: detail::Is_nothrow_constructible_helper<T, Types...>::type {};

	template<typename T,typename =Is_copy_constructible<T>>
	struct Is_nothrow_copy_constructible
		: Bool_constant<noexcept(T(declval<T const&>()))>{};

	template<typename T>
	struct Is_nothrow_copy_constructible<T, _false_type> 
		:_false_type{};

	template<typename T,typename =Is_move_constructible<T>>
	struct Is_nothrow_move_constructible
		:Bool_constant<noexcept(T(declval<T&&>()))>{};

	template<typename T>
	struct Is_nothrow_move_constructible<T, _false_type> 
		:_false_type{};

	
	template<typename T, typename U,typename =Is_assignable<T, U>>
	struct Is_nothrow_assignable
		:Bool_constant<noexcept(declval<T>()=declval<U>())> {};

	template<typename T,typename U>
	struct Is_nothrow_assignable<T, U,_false_type>
		:_false_type {};

	template<typename T,typename =Is_copy_assignable<T>>
	struct Is_nothrow_copy_assignable
		:Bool_constant<noexcept(declval<T>()=declval<T const&>())> {};

	template<typename T>
	struct Is_nothrow_copy_assignable<T, _false_type>
		:_false_type {};

	template<typename T,typename =Is_move_assignable<T>>
	struct Is_nothrow_move_assignable
		:Bool_constant<noexcept(declval<T>()=declval<T&&>())> {};

	template<typename T>
	struct Is_nothrow_move_assignable<T, _false_type>
		:_false_type {};

	//variable
	template<typename T>
	inline constexpr bool Is_default_constructible_v
		=Is_default_constructible<T>::value;


	template<typename ...Types>
	inline constexpr bool Is_constructible_v
		=Is_constructible<Types...>::value;

	template<typename T>
	inline constexpr bool Is_copy_constructible_v
		=Is_copy_constructible<T>::value;

	template<typename T>
	inline constexpr bool Is_move_constructible_v
		=Is_move_constructible<T>::value;

	template<typename T,typename U>
	inline constexpr bool Is_assignable_v
		=Is_assignable<T,U>::value;

	template<typename T>
	inline constexpr bool Is_copy_assignable_v
		=Is_copy_assignable<T>::value;

	template<typename T>
	inline constexpr bool Is_move_assignable_v
		=Is_move_assignable<T>::value;

	template<typename T>
	inline constexpr bool Is_destructible_v
		=Is_destructible<T>::value;

	//nothrow-variable
	template<typename T>
	inline constexpr bool Is_nothrow_default_constructible_v
		=Is_nothrow_default_constructible<T>::value;

	template<typename T,typename ...Types>
	inline constexpr bool Is_nothrow_constructible_v
		=Is_nothrow_constructible<T,Types...>::value;

	template<typename T>
	inline constexpr bool Is_nothrow_copy_constructible_v
		=Is_nothrow_copy_constructible<T>::value;

	template<typename T>
	inline constexpr bool Is_nothrow_move_constructible_v
		=Is_nothrow_move_constructible<T>::value;

	template<typename T,typename U>
	inline constexpr bool Is_nothrow_assignable_v
		=Is_nothrow_assignable<T,U>::value;

	template<typename T>
	inline constexpr bool Is_nothrow_copy_assignable_v
		=Is_nothrow_copy_assignable<T>::value;

	template<typename T>
	inline constexpr bool Is_nothrow_move_assignable_v
		=Is_nothrow_move_assignable<T>::value;

	template<typename T>
	inline constexpr bool Is_nothrow_destructible_v
		=Is_nothrow_destructible<T>::value;

	/*namespace detail {
		template<typename T>
		struct Is_destructible_Helper {
		private:
			//declval<U&>().~U()
			//express p->~U(),i.e. (*p).~U() requiring p is a lvalue
			template<typename U, typename=decltype(declval<U&>().~U())>
			static _true_type test(void*);
			template<typename>
			static _false_type test(...);
		public:
			using type=decltype(test<T>(nullptr));
		};
	}

	template<typename T>
	struct Is_destructible : detail::Is_destructible_Helper<T>::type {};

	template<typename T>
	constexpr bool Is_destructible_v=Is_destructible<T>::value;*/





	////////////////////////
	///Type relationships///
	////////////////////////
	namespace detail {
		template<typename from, typename to, bool =
			Disjunction_v<Is_void<to>, Is_array<to>, Is_function<to>>>
			struct Is_convertible_ :Bool_constant<Is_void<to>::value||Is_void<from>::value> {
		};

		template<typename from, typename to>
		struct Is_convertible_<from, to, false> {
		private:
			static void aux(to);
			template<typename F,
				typename=decltype(aux(declval<F>()))>
				static _true_type test(void*);
			template<typename>
			static _false_type test(...);
		public:
			using type=decltype(test<from>(nullptr));
		};
	}

	template<typename from, typename to>
	using Is_convertible =typename detail::Is_convertible_<from, to>::type;

	template<typename from, typename to>
	inline constexpr bool Is_convertible_v=Is_convertible<from, to>::value;

	namespace detail {
		/*template<typename T1, typename T2>
		struct Is_same :_false_type {
		};

		template<typename T>
		struct Is_same<T, T> :_true_type {
		};
		
		template<typename T1,typename T2>
		inline constexpr bool Is_same_v=Is_same<T1,T2>::value;*/
	
		//Based on the rule of chiel
		template<typename T1,typename T2>
		inline constexpr bool Is_same_=false;

		template<typename T>
		inline constexpr bool Is_same_<T,T> =true;
	}

	template<typename T1,typename T2>
	inline constexpr bool Is_same_v=detail::Is_same_<T1, T2>;
	
	namespace detail {
		template<typename B, typename D, bool =Conjunction_v<
			Is_class<B>, Is_class<D>, Is_convertible<D*, B*>>>
		struct Is_base_of_ :_true_type {};

		template<typename B,typename D>
		struct Is_base_of_<B, D, false> :_false_type {};
	}
	
	template<typename B,typename D>
	using Is_base_of=typename detail::Is_base_of_<B, D>::type;
	
	template<typename B,typename D>
	inline constexpr bool Is_base_of_v=detail::Is_base_of_<B, D>::value;
	
	
	
	////////////////////////
	///Type modifications///
	////////////////////////
	//remove cv specifiers:
	namespace detail {
		template<typename T>
		struct Remove_const_ {
			using type=T;
		};

		template<typename T>
		struct Remove_const_ <T const> {
			using type=T;
		};

		template<typename T>
		struct Remove_volatile_ {
			using type=T;
		};

		template<typename T>
		struct Remove_volatile_<T volatile> {
			using type=T;
		};
	}
	
	template<typename T>
	using Remove_const=detail::Remove_const_<T>;

	template<typename T>
	using Remove_volatile =detail::Remove_volatile_<T>;

	//add cv-specifiers
	namespace detail {
		template<typename T>
		struct Add_const_ {
			using type=T const;
		};

		template<>
		struct Add_const_<void> {
			using type=void;
		};

		template<typename T>
		struct Add_volatile_ {
			using type=T volatile;
		};

		template<>
		struct Add_volatile_<void> {
			using type=void;
		};

		template<typename T>
		struct Add_cv_ {
			using type=T const volatile;
		};

		template<>
		struct Add_cv_ <void>{
			using type=void;
		};
	}

	template<typename T>
	using Add_const=detail::Add_const_<T>;

	template<typename T>
	using Add_volatile=detail::Add_volatile_<T>;

	template<typename T>
	using Add_cv=detail::Add_cv_<T>;

	template<typename T>
	using Add_const_t=typename detail::Add_const_<T>::type;

	template<typename T>
	using Add_volatile_t=typename detail::Add_volatile_<T>::type;

	template<typename T>
	using Add_cv_t=typename detail::Add_cv_<T>::type;

	//pointers:
	namespace detail {

		template<typename T>
		struct Remove_pointer_ {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T*> {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T* const> {
			using type=T;
		};

		template<typename T>
		struct Remove_pointer_<T* volatile> {
			using type=T;
		};
	}

	template<typename T>
	using Remove_pointer=detail::Remove_pointer_<T>;
	
	template<typename T>
	using Remove_pointer_t=typename detail::Remove_pointer_<T>::type;

	namespace detail {
		template<typename T>
		struct Add_pointer_ {
			using type=T*;
		};

	}

	template<typename T>
	using Add_pointer=detail::Add_pointer_<T>;

	template<typename T>
	using Add_pointer_t=typename detail::Add_pointer_<T>::type;

	namespace detail {
		template<typename T>
		struct Remove_reference_ {
			using type=T;
		};

		template<typename T>
		struct Remove_reference_<T&> {
			using type=T;
		};

		template<typename T>
		struct Remove_reference_<T&&> {
			using type=T;
		};
	}

	template<typename T>
	using Remove_reference=detail::Remove_reference_<T>;

	namespace detail {
		template<typename T>
		struct Add_lvalue_reference_ {
			using type=T&;
		};

		//void
		template<>
		struct Add_lvalue_reference_<void> {
			using type=void;
		};

		
		template<>
		struct Add_lvalue_reference_<void const> {
			using type=void const;
		};

		template<>
		struct Add_lvalue_reference_<void volatile> {
			using type=void volatile;
		};

		template<>
		struct Add_lvalue_reference_<void const volatile> {
			using type=void const volatile;
		};

	
		template<typename T>
		struct Add_rvalue_reference_ {
			using type=T&&;
		};

		//void
		template<>
		struct Add_rvalue_reference_<void> {
			using type=void;
		};

		template<>
		struct Add_rvalue_reference_<const void> {
			using type=void;
		};

		template<>
		struct Add_rvalue_reference_<volatile void> {
			using type=void;
		};

		template<>
		struct Add_rvalue_reference_<const volatile void> {
			using type=void;
		};
	}

	template<typename T>
	using Add_lvalue_reference=detail::Add_lvalue_reference_<T>;

	template<typename T>
	using Add_rvalue_reference=detail::Add_rvalue_reference_<T>;
	
	namespace detail {
		//Remove the reference at first,because low-level const of reference can be removed
		template<typename T>
		struct Decay_ :Remove_cv_t<Remove_reference_t<T>> {
		};

		//array->pointer to first element
		template<typename T>
		struct Decay_ <T[]> {
			using type=T*;
		};

		template<typename T>
		struct Decay_ <T(&)[]> {
			using type=T*;
		};
		
		template<typename T, int_ N>
		struct Decay_ <T[N]> {
			using type=T*;
		};

		template<typename T, int_ N>
		struct Decay_ <T(&)[N]> {
			using type=T*;
		};

		//function->pointer-to-function
		template<typename R, typename...Args>
		struct Decay_ <R(Args...)> {
			using type=R(*)(Args...);
		};

		//c-style varargs
		template<typename R, typename...Args>
		struct Decay_ <R(Args..., ...)> {
			using type=R(*)(Args..., ...);
		};
	}
	
	template<typename T>
	using Decay=detail::Decay_<T>;

	template<typename T>
	using Decay_t=typename detail::Decay_<T>::type;

	//array:
	namespace detail {
		template<typename T>
		struct Remove_extent_ {
			using type=T;
		};

		template<typename T>
		struct Remove_extent_<T[]> {
			using type=T;
		};

		template<typename T, int_ sz>
		struct Remove_extent_<T[sz]> {
			using type=T;
		};
		template<typename T>
		struct Remove_all_extents_ {
			using type=T;
		};

		template<typename T>
		struct Remove_all_extents_<T[]> {
			using type=typename Remove_all_extents_<T>::type;
		};

		template<typename T, int_ sz>
		struct Remove_all_extents_<T[sz]> {
			using type=typename Remove_all_extents_<T>::type;
		};
	}


	template<typename T>
	using Remove_extent_t=typename detail::Remove_extent_<T>::type;

	template<typename T>
	using Remove_all_extents_t=typename detail::Remove_all_extents_<T>::type;

	
	namespace detail {
		template<bool B, typename T=void>
		struct Enable_if_;

		template<typename T>
		struct Enable_if_ <true, T> {
			using type=T;
		};
	}

	template<bool Cond,typename T=void>
	using Enable_if_t=typename detail::Enable_if_<Cond,T>::type;

	namespace detail {
		//if condition bool value is true,select the second template parameter
		//otherwise,select the third template parameter
		/*template<bool ,typename T,typename F>
		struct Conditional {
			using type=T;
		};

		template<typename T,typename F>
		struct Conditional<false, T, F> {
			using type=F;
		};

		template<bool Cond,typename T,typename F>
		using Conditional_t=typename Conditional<Cond, T, F>::type;*/

		//according to the rule of chiel,use alias template to optimize conditional<>
		template<bool>
		struct Conditional_ {
			template<typename T, typename F>
			using type=T;
		};

		template<>
		struct Conditional_<false> {
			template<typename T, typename F>
			using type=F;
		};
	}
	
	namespace detail {
		//common_type
		template<typename... Types>
		struct Common_type_;

		
		template<typename T>
		struct Common_type_<T> {
			using type=Decay_t<T>;
		};

		template<typename T1, typename T2>
		struct Common_type_<T1, T2> {
			using type=Decay_t<decltype(true?TinySTL::declval<T1>():
				TinySTL::declval<T2>())>;
		};

		template<typename T1, typename T2, typename... Ts>
		struct Common_type_<T1, T2, Ts...> {
			using type=typename Common_type_<typename Common_type_<T1, T2>::type, Ts...>::type;
		};
	}

	template<typename... Ts>
	using Common_type_t=typename detail::Common_type_<Ts...>::type;

}

#endif //TINYSTL_STL_TYPE_TRAITS_H
