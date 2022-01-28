/**
 * @version: 0.1 2021-5-16
 * @author: Conzxy
 * @Description:
 *   提供一些用到了迭代器的API可能需要的接口：
 *   iterator_tag:can be used for tag dispatching
 *   iterator_traits: can be used for extracting iterator type
 *   is_XXXXX_iterator: predicate can be used for identifying iterator category
 *   advance(): move iterator according to the given distance
 *   distance(): caculate the distance between first and last iterator
 *   reverse_iterator: iterator adapter
 */

#ifndef ZSTL_STL_ITERATOR_H
#define ZSTL_STL_ITERATOR_H

#include <cstddef>
#include "type_traits.h"
#include "stl_move.h"
#include "config.h"

namespace zstl {

struct Input_iterator_tag{};
struct Output_iterator_tag{};
struct Forward_iterator_tag : Input_iterator_tag{};
struct Bidirectional_iterator_tag : Forward_iterator_tag{};
struct Random_access_iterator_tag : Bidirectional_iterator_tag{};

template<class Category,class T,class Distance=ptrdiff_t,
		class Pointer=T*,class Reference=T&>
struct iterator{
	using iterator_category =Category;
	using value_type        =T;
	using difference_type   =Distance;
	using pointer           =Pointer;
	using reference         =Reference;
};

// SFINAE-friendly
// Ensure iterator_traits<iterator> must be instantiated such that no hard error
// The hard error is typename iterator::XXX since type alias is declaration, 
// there is no partial instantiation
#if 1
template<typename iterator,typename =Void_t<>>
struct iterator_traits_helper{
};

template<typename iterator>
struct iterator_traits_helper<iterator,Void_t<
		typename iterator::iterator_category,
		typename iterator::value_type,
		typename iterator::difference_type,
		typename iterator::pointer,
		typename iterator::reference>>{
	using iterator_category        =typename iterator::iterator_category;
	using value_type               =typename iterator::value_type;
	using difference_type          =typename iterator::difference_type;
	using pointer                  =typename iterator::pointer;
	using reference                =typename iterator::reference;
};

template<typename iterator>
struct iterator_traits:iterator_traits_helper<iterator>{};

#else
template<typename iterator>
struct iterator_traits {
	using iterator_category        =typename iterator::iterator_category;
	using value_type               =typename iterator::value_type;
	using difference_type          =typename iterator::difference_type;
	using pointer                  =typename iterator::pointer;
	using reference                =typename iterator::reference;
};

#endif

template<typename T>
struct iterator_traits<T*>{
	typedef Random_access_iterator_tag iterator_category;
	typedef T                          value_type;
	typedef ptrdiff_t                  difference_type;
	typedef T*                         pointer;
	typedef T&                         reference;
};

template<typename T>
struct iterator_traits<T const*>{
	typedef Random_access_iterator_tag iterator_category;
	typedef T const                    value_type;
	typedef ptrdiff_t                  difference_type;
	typedef T const*                   pointer;
	typedef T const&                   reference;
};


//SFINAE-friendly predicate
// template<typename iter,typename= Void_t<>>
// struct has_iterator_helper:_false_type {
// };

// NOTE: the base class is non-SFINAE context
// so provide has_iterator(SFINAE wrapper) predicate to avoid hard error
//template<typename iter>
//struct has_iterator_helper<iter,Void_t<
		//typename iterator_traits<iter>::iterator_category>>:_true_type {
//};

//template<typename iter>
//struct has_iterator:has_iterator_helper<iter>::type{};
//template<typename iter,typename U,bool =has_iterator<iter>::value>
//struct has_iterator_of:Bool_constant<
		//Is_convertible_v<typename iterator_traits<iter>::iterator_category,
						 //U>>{};

//template<typename iter,typename U>
//struct has_iterator_of<iter,U,false>:_false_type{};

// Sure, you can use Void_t instead of has_iterator, because Void_t is SFINAE context
// more simple and elegent
template<typename iter,typename U,typename =void>
struct has_iterator_of:_false_type{};

template<typename iter,typename U>
struct has_iterator_of<iter,U,Void_t<
	Enable_if_t<Is_convertible<
		typename iterator_traits<iter>::iterator_category,U>::value>
		>
	>:_true_type{};

//iterator predicate:
#define is_XXX_iterator_TEMPLATE(name, tag) \
template<typename Iter> \
struct is_##name##_iterator : has_iterator_of<Iter, tag> {};

is_XXX_iterator_TEMPLATE(input, Input_iterator_tag)
is_XXX_iterator_TEMPLATE(output, Output_iterator_tag)
is_XXX_iterator_TEMPLATE(forward, Forward_iterator_tag)
is_XXX_iterator_TEMPLATE(bidirectional, Bidirectional_iterator_tag)
is_XXX_iterator_TEMPLATE(random_access, Random_access_iterator_tag)

template<typename Iter>
struct is_iterator : Disjunction<
			is_input_iterator<Iter>,
			is_output_iterator<Iter>>
{};

#if __cplusplus >= 201402L
template<typename iter>
constexpr bool is_input_iterator_v
=has_iterator_of<iter,Input_iterator_tag>::value;

template<typename iter>
constexpr bool is_output_iterator_v
=has_iterator_of<iter,Output_iterator_tag>::value;

template<typename iter>
constexpr bool is_forward_iterator_v
=has_iterator_of<iter,Forward_iterator_tag>::value;

template<typename iter>
constexpr bool is_bidirectional_iterator_v
=has_iterator_of<iter,Bidirectional_iterator_tag>::value;

template<typename iter>
constexpr bool is_random_access_iterator_v
=has_iterator_of<iter,Random_access_iterator_tag>::value;

template<typename iter>
constexpr bool is_iterator_v
=is_input_iterator_v<iter> || is_output_iterator_v<iter>;

#endif
//iterator_traits type interface:
//maybe you don't like use these...
template<typename Iter>
using Iter_value_type=typename iterator_traits<Iter>::value_type;

template<typename Iter>
using Iter_ref=typename iterator_traits<Iter>::reference;

template<typename Iter>
using Iter_pointer=typename iterator_traits<Iter>::pointer;

template<typename Iter>
using Iter_category=typename iterator_traits<Iter>::iterator_category;

template<typename Iter>
using Iter_diff_type=typename iterator_traits<Iter>::difference_type;

//yep...you should use Iter_category, Iter_diff_type and Iter_value_type to get typename
template<class Iterator>
constexpr typename iterator_traits<Iterator>::iterator_category
iterator_category(Iterator const&){
	using category=typename iterator_traits<Iterator>::iterator_category;
	return category();
}

template<class Iterator>
constexpr typename iterator_traits<Iterator>::difference_type*
distance_type(Iterator const&){
	return static_cast<typename iterator_traits<Iterator>::difference_type*>(nullptr);
}

template<class Iterator>
constexpr typename iterator_traits<Iterator>::value_type*
value_type(Iterator const&){
	return static_cast<typename iterator_traits<Iterator>::value_type*>(nullptr);
}

//FUNCTION TEMPLATE distance
//@brief: calculate distance between first and last iterator
template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first,InputIterator last,Input_iterator_tag){
	typename iterator_traits<InputIterator>::difference_type n=0;
	for(;first!=last;++first){
		++n;
	}
	return n;
}

template<class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first,RandomAccessIterator last,Random_access_iterator_tag){
	return last-first;
}

template<class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first,InputIterator last){
	return _distance(first,last,iterator_category(first));
}

//FUNCTION TEMPLATE 
//@brief: given a distance, move the iterator according to the distance
template<class Input_iterator,typename Distance>
inline void
_advance(Input_iterator &i,Distance n,Input_iterator_tag){
	while(n-- > 0){
		++i;
	}
}

template<class Bidirectional_iterator,typename Distance>
inline void
_advance(Bidirectional_iterator &i,Distance n,Bidirectional_iterator_tag){
	if(n>=0)
		while(n--)
			++i;
	else
		while(n++)
			--i;
}

template<class Random_access_iterator,typename Distance>
inline void
_advance(Random_access_iterator& i,Distance n,Random_access_iterator_tag){
		i+=n;
}

template<class Input_iterator,typename Distance>
inline void
advance(Input_iterator &i,Distance n){
	_advance(i,n,iterator_category(i));
}

/**
 * @fn advance_iter
 * @brief advance a iterator, but return result iterator(of cource, iterator is passed by value)
 * @note
 * STL does not provide the API
 * just I need somewhere.
 */
template<typename II, typename D>
inline II
advance_iter(II i, D n) {
	zstl::advance(i, n);
	return i;
}

//Reverse Iterator
//@require:
//1.Iterator需满足Bidirectional的所有要求
//2.对于operator+，operator-，operator+=，operator-=，
//operator[]，operator<,operator>,operator<=,operator>=,
//operator-,operator+(global)，欲使用需满足Random Access Iterator的要求
template<class Iterator>
class reverse_iterator:public
	iterator<typename iterator_traits<Iterator>::iterator_category,
	typename iterator_traits<Iterator>::value_type,
	typename iterator_traits<Iterator>::difference_type,
	typename iterator_traits<Iterator>::pointer,
	typename iterator_traits<Iterator>::reference>
{
public:
	using iterator_type         =Iterator;
	using difference_type       =typename iterator_traits<Iterator>::difference_type;
	using reference             =typename iterator_traits<Iterator>::reference;
	using pointer               =typename iterator_traits<Iterator>::pointer;

public:
	reverse_iterator()
    : current(){};
	explicit reverse_iterator(Iterator x)
    : current(x){}

	template<typename U>
  reverse_iterator(reverse_iterator<U> const& u)
  : current(u.current){}

	template<typename U>
  reverse_iterator& operator=(reverse_iterator<U> const& u){
		current = u.base();
		return *this;
	};

	//Conversion:提取underlying Iterator
	Iterator base()const{return current;}

	reference operator*()const{
		//通过deref_tmp来避免dangling reference
		deref_tmp=current;
		--deref_tmp;    //反向迭代器的设计逻辑
		return *deref_tmp;
	}

	pointer operator->()const{ return &(operator*());}

	reverse_iterator& operator++(){
		--current;
		return *this;
	}

	reverse_iterator  operator++(int){
		reverse_iterator tmp=*this;
		--current;
		return tmp;
	}

	reverse_iterator& operator--(){
		++current;
		return *this;
	}

	reverse_iterator  operator--(int){
		reverse_iterator tmp=*this;
		++current;
		return tmp;
	}

	reverse_iterator operator+(difference_type n){ return reverse_iterator(current-n);}
	reverse_iterator& operator+=(difference_type n){
		current-=n;
		return *this;
	}

	reverse_iterator operator-(difference_type n){ return reverse_iterator(current+n);}
	reverse_iterator& operator-=(difference_type n){
		current+=n;
		return *this;
	}
	
	reference operator[](difference_type n)const{
		//return current[-n-1]; //这个依赖于underlying Iterator的[]实现
		//current[-n-1]=current[-(n+1)]，多减一个1是因为rbegin=end
		return *(*this+n);      //设计逻辑都是要“反向”
	}

protected:
	Iterator current;
private:
	mutable Iterator deref_tmp; //exposition only
	//dereference temporary
};

template<class Iterator1,class Iterator2>
bool operator==(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return x.base()==y.base();
}

template<class Iterator1,class Iterator2>
bool operator!=(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return !(x==y);
	//这里不是x.current!=y.current;
	//是为了以防没有重载!=的Iterator
}

template<class Iterator1,class Iterator2>
bool operator<(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return x.base()<y.base();
}

template<class Iterator1,class Iterator2>
bool operator>(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return x.base()>y.base();
}

template<class Iterator1,class Iterator2>
bool operator<=(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return !(x>y);
}

template<class Iterator1,class Iterator2>
bool operator>=(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y){
	return !(x<y);
}

template<class Iterator1,class Iterator2>
auto operator-(
		reverse_iterator<Iterator1> const& x,
		reverse_iterator<Iterator2> const& y)
-> decltype(y.current-x.current)
{
	return x.base()-y.base();     //C++14支持编译器推断auto
}

// n + iterator, because member version is iterator + n
template<class Iterator>
reverse_iterator<Iterator> operator+(
		typename reverse_iterator<Iterator>::difference_type n,
		reverse_iterator<Iterator>& x){
	return reverse_iterator<Iterator>(x.base()-n);
}

/**
 * @class MoveIterator
 * @tparam Iterator
 * @brief 
 * behavior as the underlying iterator but its dereference operator
 * return rvalue reference(i.e. xvalue), used in generic algorithm
 * can replace copying with moving
 */
template<typename Iterator>
class MoveIterator
{
protected:
	typedef iterator_traits<Iterator> IteratorTraits;
	typedef Iterator Base;
	typedef typename IteratorTraits::reference BaseReference;
	typedef MoveIterator Self;
public:
	typedef typename IteratorTraits::value_type value_type;
	// if reference is not reference type, no change
	typedef Conditional_t<Is_reference<BaseReference>::value,
		Remove_reference_t<BaseReference>&&,
		BaseReference> reference;
	typedef Iterator iterator;
	typedef iterator pointer;
	typedef typename IteratorTraits::iterator_category
		iterator_category;
	typedef typename IteratorTraits::difference_type
		difference_type;
	
	explicit MoveIterator(Base const& iter)
		: base_{ iter }
	{ }

	inline  reference
	operator*() const noexcept
	{ return STL_MOVE(*base_); }
	
	ZSTL_CONSTEXPR pointer
	operator->() const noexcept
	{ return base_; }
	
	ZSTL_CONSTEXPR reference
	operator[](difference_type n) const noexcept
	{ return STL_MOVE(*(*this + n)); }

	ZSTL_CONSTEXPR Self&
	operator++()
	{
		++base_;
		return *this;
	}

	ZSTL_CONSTEXPR Self
	operator++(int)
	{
		Self tmp = *this;
		++base_;
		return tmp;
	}
	
	ZSTL_CONSTEXPR Self&
	operator--()
	{
		--base_;
		return *this;
	}

	ZSTL_CONSTEXPR Self
	operator--(int)
	{
		Self tmp = *this;
		--base_;
		return tmp;
	}
	
	ZSTL_CONSTEXPR Self
	operator+(difference_type n)
	{
		return base_ + n;
	}

	ZSTL_CONSTEXPR Self
	operator-(difference_type n)
	{
		return base_ - n;
	}

	ZSTL_CONSTEXPR Self
	operator+=(difference_type n)
	{
		return base_ += n;
	}

	ZSTL_CONSTEXPR Self
	operator-=(difference_type n)
	{
		return base_ -= n;
	}

	ZSTL_CONSTEXPR Iterator 
	base() const noexcept
	{ return base_; }
protected:
	Iterator base_;
};

template<typename I>
MoveIterator<I> operator+(typename MoveIterator<I>::difference_type n, MoveIterator<I>& x)
{ return x + n; }

template<typename I1, typename I2>
auto operator-(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
-> decltype(x.base() - y.base())
{ return x.base() - y.base(); }

template<typename I1, typename I2>
bool operator==(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y) 
{
	return x.base() == y.base(); 
}

template<typename I1, typename I2>
bool operator!=(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
{ return !(x == y); }


template<typename I1, typename I2>
bool operator<(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
{ return x.base() < y.base(); }

template<typename I1, typename I2>
bool operator>=(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
{ return !(x < y); }

template<typename I1, typename I2>
bool operator>(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
{ return y < x; }

template<typename I1, typename I2>
bool operator<=(
		MoveIterator<I1> const& x,
		MoveIterator<I2> const& y)
{ return !(y < x); }

template<typename Iter>
MoveIterator<Iter> makeMoveIterator(Iter const& x)
{ return MoveIterator<Iter>(x); }

namespace detail {	

template<typename Iter, bool = 
move_if_noexcept_cond<Iter_value_type<Iter>>::value>
struct make_move_if_noexcept_iterator_impl
{
static MoveIterator<Iter> apply(Iter const& x)
{ return makeMoveIterator(x); }
};

template<typename Iter>
struct make_move_if_noexcept_iterator_impl <Iter, true>
{
static Iter apply(Iter const& x)
{ return x; }
};

template<typename Iter>
auto make_move_if_noexcept_iterator(Iter const& x)
-> decltype(make_move_if_noexcept_iterator_impl<Iter>::apply(x))
{ return make_move_if_noexcept_iterator_impl<Iter>::apply(x); }

}  // namespace detail

template<typename Iter>
struct Is_MoveIterator : _false_type {};

template<typename Base>
struct Is_MoveIterator <MoveIterator<Base>> : _true_type {};

#define MAKE_MOVE_IF_NOEXCEPT_ITERATOR(x) detail::make_move_if_noexcept_iterator(x)

} // namespace zstl

#endif //ZSTL_STL_ITERATOR_H
