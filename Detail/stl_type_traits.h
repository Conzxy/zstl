#ifndef TINYSTL_STL_TYPE_TRAITS_H
#define TINYSTL_STL_TYPE_TRAITS_H

//该文件实现了一部分标准库traits（×），templates练习（√）
#include <cstddef>
#include <type_traits>
#include <stl_type_traits_base.h>

#define DEFINE_HAS_TYPE(Memtype)						            \
	template<typename,typename =Void_t<>>				            \
	struct HasTypeT_##Memtype							            \
	:_false_type{};										            \
	template<typename T>								            \
	struct HasTypeT_##Memtype<T,Void_t<typename T::Memtype>>		\
	:_true_type{};                                                  \

	//检查成员和single成员函数
#define DEFINE_HAS_MEMBER(Member)                                   \
	template<typename,typename =Void_t<>>                           \
	struct HasMemberT_##Member                                      \
	:_false_type{};                                                 \
	template<typename T>                                            \
	struct HasMemberT_##Member<T,Void_t<decltype(&T::Member)>>      \
	:_true_type{};                                                  \

#define DEFINE_INTEGRAL(INT_TYPE)									\
		template<>													\
		struct Is_integral_<INT_TYPE>								\
		:_true_type {												\
		};															\

#define DEFINE_FLOATING_POINT(TYPE)									\
template<>															\
struct Is_floating_point_<TYPE>:_true_type{};						\
	

namespace TinySTL{
	template<typename T>
	struct Remove_reference;

	template<typename T>
	struct Remove_cv;

	template<typename T>
	struct Add_rvalue_reference;

	//dummy object
	template<typename T>
	typename Add_rvalue_reference<T>::type declval()noexcept;

	template<typename...>
	using Void_t=void;
	
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
	struct Conditional {
		template<typename T,typename F>
		using type=T;
	};

	template<>
	struct Conditional<false> {
		template<typename T,typename F>
		using type=F;
	};

	template<bool Cond,typename T,typename F>
	using Conditional_t= typename Conditional<Cond>::template type<T,F>;

	//_or_,_and_可用于组合谓词
	template<typename ...>
	struct _or_{};

	template<>
	struct _or_<> :_false_type{};

	template<typename T>
	struct _or_ <T>:T{};

	template<typename T1,typename T2>
	struct _or_<T1,T2>:Conditional_t<T1::value,T1,T2>{};

	template<typename T1,typename T2,typename T3,typename...Tn>
	struct _or_<T1,T2,T3,Tn...>:Conditional_t<T1::value,T1,_or_<T2,T3,Tn...>>{};

	template<typename...>
	struct _and_{};

	template<>
	struct _and_<> :_true_type{};

	template<typename T>
	struct _and_<T> :T{};

	template<typename T1,typename T2>
	struct _and_<T1,T2>:Conditional_t<T1::value,T2,T1>{};

	template<typename T1,typename T2,typename T3,typename ...Tn>
	struct _and_<T1,T2,T3,Tn...> :Conditional_t<T1::value,_and_<T2,T3,Tn...>,T1>{};

	template<typename T>
	struct _not_:Bool_constant<!bool(T::value)>{};

	//conjunction,disjunction,negation
	template<typename...Tn>
	struct Conjunction:_and_<Tn...>{};

	template<typename...Tn>
	struct Disjunction:_or_<Tn...>{};

	template<typename T>
	struct Negation:_not_<T>{};

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
	using Is_void=typename detail::Is_void_helper<typename Remove_cv<T>::type>::type;

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
	using Is_integral=typename detail::Is_integral_<typename Remove_cv<T>::type>::type;

	namespace detail {
		template<typename T>
		struct Is_floating_point_ :_false_type {};

		DEFINE_FLOATING_POINT(double);
		DEFINE_FLOATING_POINT(double long);
		DEFINE_FLOATING_POINT(float);
	}

	template<typename T>
	using Is_floating_point=typename detail::Is_floating_point_<typename Remove_cv<T>::type>::type;

	
	namespace detail {
		template<typename T>
		struct Is_array_ :_false_type {};

		template<typename T>
		struct Is_array_<T[]> :_true_type {};

		template<typename T, std::size_t sz>
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
	using Is_pointer=typename detail::Is_pointer_<typename Remove_cv<T>::type>::type;

	namespace detail {
		template<typename T>
		struct Is_null_pointer_ :_false_type {
		};

		template<>
		struct Is_null_pointer_<std::nullptr_t> :_true_type {};
	}
		
	template<typename T>
	using Is_null_pointer=typename detail::Is_null_pointer_<typename Remove_cv<T>::type>::type;

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
	using Is_member_object_pointer=typename detail::Is_member_object_pointer_<typename Remove_cv<T>::type>::type;
	
	namespace detail {

		template<typename T>
		struct Is_function_;

		template<typename T>
		struct Is_member_function_pointer_ :_false_type {};

		template<typename F,typename C>
		struct Is_member_function_pointer_<F C::*> :Is_function_<F> {
			using Member=F;
			using Return=typename Is_function_<F>::Return;
			using Class=C;
			static constexpr bool variadic=Is_function_<F>::variadic;
		};
	}

	template<typename T>
	using Is_member_function_pointer=typename detail::Is_member_function_pointer_<
																		typename Remove_cv<T
																							>::type
																						>::type;

	template<typename T,bool =Is_member_function_pointer<T>::value>
	struct member_function_pointer_traits {
		using Member=typename detail::Is_member_function_pointer_<T>::Member;
		using Return=typename detail::Is_member_function_pointer_<T>::Return;
		using Class =typename detail::Is_member_function_pointer_<T>::Class;
		static constexpr bool variadic=detail::Is_member_function_pointer_<T>::variadic;
	};

	template<typename T>
	struct member_function_pointer_traits<T,false> {
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

	}

	template<typename T>
	using Is_function=typename detail::Is_function_<T>::type;

	template<typename T,bool =Is_function<T>::value>
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
								  Is_lvalue_reference<T>,Is_rvalue_reference<T>,
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
	template<typename T>
	struct Is_const;

	////////////////////////
	///Type construction////
	////////////////////////
	template<typename T>
	struct Remove_pointer{
		using type=T;
	};

	template<typename T>
	struct Remove_pointer<T*>{
		using type=T;
	};

	template<typename T>
	struct Remove_pointer<T *const>{
		using type=T;
	};

	template<typename T>
	struct Remove_pointer<T *volatile>{
		using type=T;
	};

	template<typename T>
	using Remove_pointer_t=typename Remove_pointer<T>::type;

	//以下模板用于去除引用
	template<typename T>
	struct Remove_reference{
		using type=T;
	};

	template<typename T>
	struct Remove_reference<T&>{
		using type=T;
	};

	template<typename T>
	struct Remove_reference<T&&>{
		using type=T;
	};

	template<typename T>
	using Remove_reference_t=typename Remove_reference<T>::type;

	//以下模板用于添加左值引用
	template<typename T>
	struct Add_lvalue_reference{
		using type=T&;
	};

	//void进行特化
	template<>
	struct Add_lvalue_reference<void>{
		using type=void;
	};

	template<>
	struct Add_lvalue_reference<void const>{
		using type=void const;
	};

	template<>
	struct Add_lvalue_reference<void volatile>{
		using type=void volatile;
	};

	template<>
	struct Add_lvalue_reference<void const volatile>{
		using type=void const volatile;
	};

	template<typename T>
	using Add_lvalue_reference_t=typename Add_lvalue_reference<T>::type;

	//以下模板用于添加右值引用
	template<typename T>
	struct Add_rvalue_reference{
		using type=T&&;
	};

	//对void进行特化
	template<>
	struct Add_rvalue_reference<void>{
		using type=void;
	};

	template<>
	struct Add_rvalue_reference<const void>{
		using type=void;
	};

	template<>
	struct Add_rvalue_reference<volatile void>{
		using type=void;
	};

	template<>
	struct Add_rvalue_reference<const volatile void>{
		using type=void;
	};

	template<typename T>
	using Add_rvalue_reference_t
	=typename Add_rvalue_reference<T>::type;


	//以下模板用于去除cv
	//t
	template<typename T>
	struct Remove_const{
		using type=T;
	};

	template<typename T>
	struct Remove_const<T const>{
		using type=T;
	};

	template<typename T>
	using Remove_const_t=typename Remove_const<T>::type;

	template<typename T>
	struct Remove_volatile{
		using type=T;
	};

	template<typename T>
	struct Remove_volatile<T volatile>{
		using type=T;
	};

	template<typename T>
	using Remove_volatile_t=typename Remove_volatile<T>::type;

	template<typename T>
	struct Remove_cv: Remove_const<Remove_volatile_t<T>>
	{};

	//template<typename T>
	//using Remove_cv_t=typename Remove_cv<T>::type;

	template<typename T>
	using Remove_cv_t=Remove_const_t<Remove_volatile_t<T>>;

	//以下模板用于退化（decay）
	//注意，这里必须先去引用再去cv，不然低层const去不掉（不针对指针）
	template<typename T>
	struct Decay:Remove_cv<Remove_reference_t<T>>{
	};

	template<typename T>
	struct Decay<T[]>{
		using type=T*;
	};

	template<typename T>
	struct Decay<T(&)[]>{
		using type=T*;
	};

	//数组特化
	template<typename T,std::size_t N>
	struct Decay<T[N]>{
		using type=T*;
	};

	template<typename T,std::size_t N>
	struct Decay<T(&)[N]>{
		using type=T*;
	};

	//函数特化
	template<typename R,typename...Args>
	struct Decay<R(Args...)>{
		using type=R(*)(Args...);
	};

	//c-style varargs
	template<typename R,typename...Args>
	struct Decay<R(Args...,...)>{
		using type=R(*)(Args...,...);
	};

	template<typename T>
	using Decay_t=typename Decay<T>::type;

	/*template<typename T1,typename T2>
	struct Is_same{
		static constexpr bool value=false;
	};

	template<typename T>
	struct Is_same<T,T>{
		static constexpr bool value=true;
	};

	template<typename T1,typename T2>
	constexpr bool Is_same_v=Is_same<T1,T2>::value;*/

	template<typename T>
	struct Remove_all_extents :_false_type {
		using type=T;
	};

	template<typename T>
	struct Remove_all_extents<T[]> :_true_type {
		using type=typename Remove_all_extents<T>::type;
	};

	template<typename T, std::size_t sz>
	struct Remove_all_extents<T[sz]> :_true_type {
		using type=typename Remove_all_extents<T>::type;
	};


	//predicate
	//以下用于判断类型是否相同
	template<typename T1,typename T2>
	struct Is_same:_false_type{
	};

	template<typename T>
	struct Is_same<T,T>:_true_type{
	};

	template<typename T1,typename T2>
	using Is_same_t=typename Is_same<T1,T2>::type;

	template<typename T1,typename T2>
	constexpr bool Is_same_v=Is_same<T1,T2>::value;

	

	//SFINAE
	/* overloads version
	 * template<typename T>
	struct Is_default_constructible_Helper{
	private:
		template<typename U,typename =decltype(U())>
		static _true_type test(void*);

		template<typename>
		static _false_type test(...);
	public:
		using type=decltype(test<T>(nullptr));
	};

	template<typename T>
	struct Is_default_constructible:Is_default_constructible_Helper<T>::type{};

	template<typename T>
	constexpr bool Is_default_constructible_v=Is_default_constructible<T>::value;
	*/
	template<bool B,typename T=void>
	struct Enable_if;

	template<typename T>
	struct Enable_if<true,T>{
		using type=T;
	};

	template<bool Cond,typename T=void>
	using Enable_if_t=typename Enable_if<Cond,T>::type;

	

	template<typename T>
	struct Is_destructible_Helper{
	private:
		//declval<U&>().~U()
		//express p->~U(),i.e. (*p).~U() requiring p is a lvalue
		template<typename U,typename=decltype(declval<U&>().~U())>
		static _true_type test(void*);
		template<typename>
		static _false_type test(...);
	public:
		using type=decltype(test<T>(nullptr));
	};

	template<typename T>
	struct Is_destructible:Is_destructible_Helper<T>::type{};

	template<typename T>
	constexpr bool Is_destructible_v=Is_destructible<T>::value;

	//generic lambda version
	//helper:check validity of f(args...) for F f and Args...args
	template<typename F,typename...Args,
			typename =decltype(declval<F>()(declval<Args&&>()...))>
	_true_type Is_valid_impl(void*);

	template<typename F,typename...Args>
	//template<typename ...args>
	_false_type Is_valid_impl(...);

	//inline是为了效率，这里constexpr有const语义，所以是internal linkage
	inline constexpr
	auto Is_valid=[](auto f) {      //traits factory
					return [](auto &&... Args) {
							return decltype(Is_valid_impl<decltype(f),
														  decltype(Args)&& ...
														  >(nullptr)){};
		};
	};

	//helper template to present a type as a value
	template<typename T>
	struct typeT{
		using type=T;
	};

	//helper tp wrap a type as a value
	template<typename T>
	constexpr auto type=typeT<T>{};

	//helper to unwrap a wrapped type in unevaluated context
	template<typename T>
	T valueT(typeT<T>);  //no definition needed

	//callable object
	constexpr auto Is_default_constructible
	 =Is_valid([](auto x)->decltype((void)decltype(valueT(x))()){
	 });
	//substitution Is_valid's definition
	/*
	 * constexpr auto Is_default_constructible
	 *  =[](auto&&... Args){
	 *      return decltype(
	 *                Is_valid_impl<
	 *                      decltype([](auto x)
	 *                                  ->decltype((void)decltype(valueT(x))())),
	 *                      decltype(Args)&&...
	 *                      >(nullptr)){};
	 *   };
	 */

	template<typename T>
	constexpr bool Is_default_constructible_v
	 =decltype(Is_default_constructible(type<T>))::value;

	template<typename from,typename to>
	struct Is_convertible_helper{
	private:
		static void aux(to);
		template<typename F,
				typename =decltype(aux(declval<F>()))>
		static _true_type test(void*);
		template<typename>
		static _false_type test(...);
	public:
		using type=decltype(test<from>(nullptr));
	};

	template<typename from,typename to>
	struct Is_convertible: Is_convertible_helper<from,to>::type{};

	template<typename from,typename to>
	constexpr bool Is_convertible_v=Is_convertible<from,to>::value;

	//is_convertible continue

	//common_type
	template<typename... Types>
	struct Common_type;

	template<typename... Ts>
	using Common_type_t=typename Common_type<Ts...>::type;

	template<typename T>
	struct Common_type<T> {
		using type=Decay_t<T>;
	};

	template<typename T1,typename T2>
	struct Common_type<T1, T2> {
		using type=Decay_t<decltype(true?TinySTL::declval<T1>():
										 TinySTL::declval<T2>())>;
	};

	template<typename T1,typename T2,typename... Ts>
	struct Common_type<T1, T2, Ts...> {
		using type=Common_type_t<Common_type_t<T1,T2>,Ts...>;
	};

	

	template<typename ,typename= Void_t<>>
	class has_SizeType:public _false_type{
	};

	template<typename T>
	class has_SizeType<T,Void_t<typename Remove_reference_t<T>::size_type>>: public _true_type {};

	template<typename T>
	using has_SizeType_t=typename has_SizeType<T>::type;

	template<typename T>
	constexpr bool has_SizeType_v=has_SizeType<T>::value;

	struct size_type{};
	struct sizeable:size_type{};
	static_assert(has_SizeType_v<sizeable>,
					"Compiler bug:Injected class name missing");

	
	namespace detail {
		/*template<typename T>
		struct remove_function_const{
			using type=T;
		};

		//total 24 partial specialization
		//regular
		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const>{
			using type=R(Paras...);
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile> {
			using type=R(Paras...) volatile;
		};

		//&
		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const &> {
			using type=R(Paras...) &;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile&> {
			using type=R(Paras...) volatile &;
		};

		//&&
		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const &&> {
			using type=R(Paras...) &&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile&&> {
			using type=R(Paras...) volatile &&;
		};

		//noexcept:
		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const noexcept> {
			using type=R(Paras...) noexcept;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile noexcept> {
			using type=R(Paras...) volatile noexcept;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const & noexcept> {
			using type=R(Paras...) & noexcept;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile & noexcept> {
			using type=R(Paras...) volatile & noexcept;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const && noexcept> {
			using type=R(Paras...) && noexcept;
		};

		template<typename R,typename ...Paras>
		struct remove_function_const<R(Paras...) const volatile&& noexcept> {
			using type=R(Paras...) volatile && noexcept;
		};

		//c-style variadic
		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const> {
			using type=R(Paras...,...);
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile> {
			using type=R(Paras...,...) volatile;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const &> {
			using type=R(Paras...,...) &;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile &> {
			using type=R(Paras...,...) volatile&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const &&> {
			using type=R(Paras...,...) &&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile &&> {
			using type=R(Paras...,...) volatile &&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const noexcept> {
			using type=R(Paras...,...) noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile noexcept> {
			using type=R(Paras...,...) volatile noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const & noexcept> {
			using type=R(Paras...,...) & noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile & noexcept> {
			using type=R(Paras..., ...) volatile & noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const && noexcept> {
			using type=R(Paras...,...) && noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_const<R(Paras...,...) const volatile && noexcept> {
			using type=R(Paras...,...) volatile && noexcept;
		};



		template<typename T>
		struct remove_function_volatile {
			using type=T;
		};

		//total 24 partial specialization
		//regular
		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile> {
			using type=R(Paras...);
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile> {
			using type=R(Paras...) const;
		};

		//&
		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile&> {
			using type=R(Paras...)&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile&> {
			using type=R(Paras...) const &;
		};

		//&&
		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile&&> {
			using type=R(Paras...)&&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile&&> {
			using type=R(Paras...) const&&;
		};

		//noexcept:
		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile noexcept> {
			using type=R(Paras...) noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile noexcept> {
			using type=R(Paras...) const noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile& noexcept> {
			using type=R(Paras...) & noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile& noexcept> {
			using type=R(Paras...) const & noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) volatile&& noexcept> {
			using type=R(Paras...)&&noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras...) const volatile&& noexcept> {
			using type=R(Paras...) const&& noexcept;
		};

		//c-style variadic
		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile> {
			using type=R(Paras..., ...);
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile> {
			using type=R(Paras..., ...) const;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile&> {
			using type=R(Paras..., ...)&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile&> {
			using type=R(Paras..., ...) const&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile&&> {
			using type=R(Paras..., ...)&&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile&&> {
			using type=R(Paras..., ...) const&&;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile noexcept> {
			using type=R(Paras..., ...) noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile noexcept> {
			using type=R(Paras..., ...) const noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile& noexcept> {
			using type=R(Paras..., ...) & noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile& noexcept> {
			using type=R(Paras..., ...) const& noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) volatile&& noexcept> {
			using type=R(Paras..., ...)&&noexcept;
		};

		template<typename R, typename ...Paras>
		struct remove_function_volatile<R(Paras..., ...) const volatile&& noexcept> {
			using type=R(Paras..., ...) const&& noexcept;
		};

		template<typename T>
		using remove_function_cv=typename remove_function_const<
									typename remove_function_volatile<T
												>::type
											>::type;

		template<typename T>
		struct remove_noexcept {
			using type=T;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept <R(Paras...) noexcept> {
			using type=R(Paras...);
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const noexcept>{
			using type=R(Paras...) const;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept <R(Paras...) volatile noexcept> {
			using type=R(Paras...) volatile;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const volatile noexcept> {
			using type=R(Paras...) const volatile;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) & noexcept> {
			using type=R(Paras...)&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const & noexcept> {
			using type=R(Paras...)&&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) volatile& noexcept> {
			using type=R(Paras...) volatile&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const volatile& noexcept> {
			using type=R(Paras...) const volatile&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...)&&noexcept> {
			using type=R(Paras...)&&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const&& noexcept> {
			using type=R(Paras...) const&&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) volatile&& noexcept> {
			using type=R(Paras...) volatile&&;
		};

		template<typename R,typename ...Paras>
		struct remove_noexcept<R(Paras...) const volatile&& noexcept> {
			using type=R(Paras...) const volatile&&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept <R(Paras...,...) noexcept> {
			using type=R(Paras...,...);
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const noexcept> {
			using type=R(Paras...,...) const;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept <R(Paras...,...) volatile noexcept> {
			using type=R(Paras...,...) volatile;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const volatile noexcept> {
			using type=R(Paras...,...) const volatile;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) & noexcept> {
			using type=R(Paras...,...)&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const& noexcept> {
			using type=R(Paras...,...)&&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) volatile& noexcept> {
			using type=R(Paras...,...) volatile&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const volatile& noexcept> {
			using type=R(Paras...,...) const volatile&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...)&&noexcept> {
			using type=R(Paras...,...)&&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const&& noexcept> {
			using type=R(Paras...,...) const&&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) volatile&& noexcept> {
			using type=R(Paras...,...) volatile&&;
		};

		template<typename R, typename ...Paras>
		struct remove_noexcept<R(Paras...,...) const volatile&& noexcept> {
			using type=R(Paras...,...) const volatile&&;
		};

		template<typename T>
		using remove_function_noexcept=typename remove_noexcept<T>::type;*/

		//romove_function_lvalue_reference
		//remove_function_rvalue_reference
		//five restriction total 120 partial specialization!
	}

	namespace detail {
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
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		//&/&&
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...)&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...)&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...)&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...)&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile&&> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		//noexcept
		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) & noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) & noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...)&&noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) volatile&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras...) const volatile&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=false;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...)&&noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) volatile&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};

		template<typename R, typename ...Paras>
		struct Is_function_<R(Paras..., ...) const volatile&& noexcept> :_true_type {
			using Return=R;
			static constexpr bool variadic=true;
		};
	}
	
}

#endif //TINYSTL_STL_TYPE_TRAITS_H
