#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include "allocator.h"
#include "type_traits.h"
#include "stl_uninitialized.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "config.h"

#include <initializer_list>
#include <stdexcept>
#include <climits>

namespace TinySTL {
// sss
/**
 * @class VectorBase
 * @tparam T type of element
 * @tparam Allocator type of Allocator
 * @brief memery management implemetation of Vector
 * @note: Use RAII technique or as base class both is OK
 *		  only delegate the momery management to other part instead of Vector<> itself.
 *
 *		  Allocator as base class is based a basic facts: allocator is a empty class usually
 *		  so we can take advantage of EBCO(empty base class optimization)
 */
template<typename T,typename Allocator>
struct VectorBase : Allocator {
	using AllocTraits = allocator_traits<Allocator>;
	using size_type   = typename AllocTraits::size_type;

	VectorBase() 
		: first_{nullptr}
		, last_{nullptr}
		, capa_{nullptr}
	{ }

	// FIXME: Using two phase initialization may be better?
	explicit VectorBase(size_type n)
		STL_TRY : first_{AllocTraits::allocate(*this, n)}
				, last_{first_}
				, capa_{first_+n}
	{ }
	catch(std::bad_alloc const& e) {
		AllocTraits::deallocate(*this, this->first_);
		RETHROW
	}

	VectorBase(T const* first,T const* last)
		: VectorBase(last-first)
	{ }

	~VectorBase()
	{ AllocTraits::deallocate(*this, first_, capa_ - last_); }

	VectorBase(VectorBase&& base) TINYSTL_NOEXCEPT
		: first_{base.first_}
		, last_{base.last_}
		, capa_{base.capa_} 
	{ base.first_ = base.last_ = base.capa_ = nullptr; }

	VectorBase& operator=(VectorBase&& base) TINYSTL_NOEXCEPT{
		this->swap(base); 
		return *this; 
	}

	void swap(VectorBase& rhs) TINYSTL_NOEXCEPT {
		STL_SWAP(first_, rhs.first_);
		STL_SWAP(last_, rhs.last_);
		STL_SWAP(capa_, rhs.capa_);
	}
public:
	T* first_;
	T* last_;
	T* capa_; 
};

/**
 * @class Vector
 * @tparam T type of element
 * @tparam Allocator type of Allocator(default is TinySTL::allocator)
 * @brief 
 * Implemetation of linear space which
 * support expand automately.
 * Also, you reserve space by expand space exlicitly.
 * @see https://en.cppreference.com/w/cpp/container/vector for detail
 */
template<typename T,typename Allocator = TinySTL::allocator<T>>
class Vector : protected VectorBase<T, Allocator> {
public:
	using value_type             = T;
	using pointer                = typename allocator_traits<Allocator>::pointer;
	using const_pointer          = typename allocator_traits<Allocator>::const_pointer;
	using reference              = value_type&;
	using const_reference        = value_type const&;
	using iterator               = T*;
	using const_iterator         = T const*;
	using reverse_iterator       = TinySTL::reverse_iterator<iterator>;
	using const_reverse_iterator = TinySTL::reverse_iterator<const_iterator>;
	using allocator_type         = Allocator;

	using size_type              = std::size_t;
	using difference_type        = ptrdiff_t;
	using base                   = VectorBase<T,Allocator>;
	using AllocTraits            = typename base::AllocTraits;
public:
	// ctors:
	Vector()=default;

	Vector(const size_type n,value_type const& val)
		: base(n)
	{
		uninitialized_fill_n(begin(),n,val);
	}
	
	// if T is deleted default constructor, we just alloc space for it
	// we can call insert(), emplace_*() or push_*() to fill after
	//
	// @note: the feature is not supported by STL, it just is my added
	explicit Vector(size_type n)
		: base(n)
	{ Vector_aux<value_type>(n); }
	
	// The Enable_if_t is necessary here.
	// Because if you call Vector(int, int) to call <size, val> version,
	// but compiler ultimately call iterator range version
	// This is not a error, see overload resolution for detail
	template<typename InputIterator,
		Enable_if_t<is_input_iterator<InputIterator>::value,int> =0>
	Vector(InputIterator first,InputIterator last)
		: Vector(distance(first,last))
	{
		TinySTL::uninitialized_copy(first,last,begin());
	}

	// dtor:	
	// deallocate is delegated to base class
	~Vector(){ destroy(begin(),end()); }

	// copy & move ctor
	Vector(Vector const& rhs)
		: Vector(rhs.begin(),rhs.end()){}

	Vector(Vector&& rhs) TINYSTL_NOEXCEPT 
		:  base(STL_MOVE(rhs))
	{ }
	
	// The Enable_if_t is not necessay here, it is just ensure instantiation safe	
	// (i.e. detect the error in compile time before the run time)
	template<typename U, typename =
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	Vector(std::initializer_list<U> il)
		: Vector(il.begin(),il.end()){}
	
	// operator=:
	Vector& operator=(Vector const&);
	Vector& operator=(Vector&& rhs) TINYSTL_NOEXCEPT {
		this->swap(rhs); 
		return *this;
	}
	
	template<typename U, typename = 
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	Vector& operator=(std::initializer_list<U>);

	// assign operation:	
	template<typename InputIterator,typename =Enable_if_t<
			is_input_iterator<InputIterator>::value>>
	void assign(InputIterator first,InputIterator last);
	void assign(size_type n,T const& t);

	allocator_type get_allocator()const TINYSTL_NOEXCEPT
	{ return Allocator(); }

	// iterators:
	iterator                begin()                     TINYSTL_NOEXCEPT
	{ return iterator(this->first_); }
	iterator                end()                       TINYSTL_NOEXCEPT
	{ return iterator(this->last_); }
	const_iterator          begin()             const   TINYSTL_NOEXCEPT
	{ return const_iterator(this->first_); }
	const_iterator          end()               const   TINYSTL_NOEXCEPT
	{ return const_iterator(this->last_); }  
	reverse_iterator        rbegin()                    TINYSTL_NOEXCEPT
	{ return reverse_iterator(end()); }
	const_reverse_iterator  rbegin()            const   TINYSTL_NOEXCEPT
	{ return const_reverse_iterator(end()); }
	reverse_iterator        rend()                      TINYSTL_NOEXCEPT
	{ return reverse_iterator(begin()); }
	const_reverse_iterator  rend()              const   TINYSTL_NOEXCEPT
	{ return const_reverse_iterator(begin()); }

	const_iterator          cbegin()            const   TINYSTL_NOEXCEPT
	{ return begin(); }
	const_iterator          cend()              const   TINYSTL_NOEXCEPT
	{ return end(); }
	const_reverse_iterator  crbegin()           const   TINYSTL_NOEXCEPT
	{ return rbegin(); }
	const_reverse_iterator  crend()             const   TINYSTL_NOEXCEPT
	{ return rend(); }

	// capacity:
	size_type   size()               const   TINYSTL_NOEXCEPT
	{ return end()-begin();}
	size_type   max_size()           const   TINYSTL_NOEXCEPT
	{ return size_type(UINT_MAX/sizeof(T)); }
	void        resize(size_type sz);
	void        resize(size_type sz,T const& c);
	size_type   capacity()              const   TINYSTL_NOEXCEPT
	{ return this->capa_ - this->first_;}
	bool        empty()                 const   TINYSTL_NOEXCEPT
	{return begin() == end(); }
	void        reserve(size_type n);

	void        shrink_to_fit();

	// element access:
	reference           operator[](size_type n) TINYSTL_NOEXCEPT
	{ return *(begin()+n); }
	const_reference     operator[](size_type n)const TINYSTL_NOEXCEPT
	{ return *(cbegin()+n); }
	reference           at(size_type n);
	const_reference     at(size_type n)const;
	reference           front() TINYSTL_NOEXCEPT
	{ return *begin(); }
	const_reference     front() const TINYSTL_NOEXCEPT
	{ return *begin(); }
	reference           back() TINYSTL_NOEXCEPT
	{ return *(end()-1); }
	const_reference     back()const TINYSTL_NOEXCEPT
	{ return *(end()-1); }

	// data access:
	T*          data()  TINYSTL_NOEXCEPT
	{ return this->first_; }
	T const*    data()  const TINYSTL_NOEXCEPT
	{ return this->first_; }

	// modifiers:
	template<typename...Args>
	void emplace_back(Args&&... args);
	void push_back(T const& x);
	void push_back(T&& x){ emplace_back(TinySTL::move(x)); }

	template<typename... Args> iterator emplace(const_iterator position,Args&&... args);

	iterator insert(const_iterator position,T const& x);
	iterator insert(const_iterator position,size_type n,T const& x);

	template<typename InputIterator,typename =
		Enable_if_t<is_input_iterator<InputIterator>::value>>
	iterator insert(const_iterator position,
					InputIterator first,InputIterator last);

	template<typename U, typename = 
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	iterator insert(const_iterator position,std::initializer_list<U> il);
	void pop_back() TINYSTL_NOEXCEPT;

	iterator erase(const_iterator position) TINYSTL_NOEXCEPT;
	iterator erase(const_iterator first,const_iterator last) TINYSTL_NOEXCEPT;

	void swap(Vector& rhs)TINYSTL_NOEXCEPT
	{ this->base::swap(rhs); }

	// @warning clear() don't free space.
	// User can shrink size by call shrink_to_fit() in C++11,
	// also you can use Vector(v).swap(v)(used before c++11).
	// But the second approach is not recommended,
	// because 

	void clear()TINYSTL_NOEXCEPT
	{ erase(begin(),end()); }

private:
	void checkSize(size_type n);
	void checkCapacity(size_type n);

	size_type getNewCapacity(size_type len)const;

	iterator insertAux(iterator position,T const& x);
	iterator insertFillNAux(iterator position, size_type n, T const& x);
	template<typename II> iterator insertRangeAux(iterator, II first, II last);
	template<typename ...Args> void expandAndEmplace(const_iterator position, Args&&... args);

	iterator erase_aux(iterator first,iterator last);
	iterator erase_aux(iterator position);
	
	template<typename U, TinySTL::Enable_if_t<Is_default_constructible<U>::value, char> = 0>
	void Vector_aux(size_type n) {
		TinySTL::uninitialized_fill_n(this->first_, n, U{});
	}
		
	template<typename U, TinySTL::Enable_if_t<!Is_default_constructible<U>::value, int> = 0>
	void Vector_aux(size_type )
	{ }

	static constexpr bool useReallocPolicy = 
		Conjunction_t<
			Is_same<Allocator, TinySTL::allocator<T>>,
			Is_trivially_copyable<T>>::value;

};

template<typename T,typename Allocator>
inline bool 
operator==(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return x.size()==x.size() && TinySTL::equal(x.begin(),x.end(),y.begin()); }

template<typename T,typename Allocator>
inline bool 
operator!=(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return !(x==y); }

template<typename T,typename Allocator>
inline bool 
operator <(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return TinySTL::lexicographical_compare(x.begin(), x.end(),
										  y.begin(), y.end()); }

template<typename T,typename Allocator>
inline bool 
operator>=(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return !(x<y); }

template<typename T,typename Allocator>
inline bool 
operator >(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return y<x; }

template<typename T,typename Allocator>
inline bool 
operator<=(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return !(y<x); }

// The reason for defining non-member function of swap is 
// to be compatible with STL(?).(just a convention)
template<typename T,typename Allocator>
inline void 
swap(Vector<T,Allocator>& x,Vector<T,Allocator>& y) 
TINYSTL_NOEXCEPT(TINYSTL_NOEXCEPT(x.swap(y)))
{ x.swap(y); }

template<typename T,typename Alloc>
inline Vector<T,Alloc>& 
Vector<T,Alloc>::operator=(Vector const& rhs){
	// if this == rhs, it can get true result
	// but meaningless copy also have cost
	if (this != &rhs) {                
		auto new_sz = rhs.size();
		auto old_sz = size();

		if (new_sz >= capacity()) {
			// You should not use reserve() here,
			// it need copy old elements to new space
			Vector<T,Alloc> tmp{ rhs.begin(), rhs.end() };
			swap(tmp);
		} else if (new_sz <= size()) {
			this->last_ 
				= TinySTL::copy
				(rhs.begin(), rhs.end(), this->first_);

			AllocTraits::destroy(*this, this->last_, this->first_+old_sz);
		} else {
			// size() < new_sz < capacity()
			this->last_ 
				= TinySTL::copy
				(rhs.begin(), rhs.begin()+size(), begin());

			this->last_ 
				= TinySTL::uninitialized_copy
				(rhs.begin()+size(), rhs.end(), this->last_);
				

		}
	}	

	return *this;
}

template<typename T,typename Alloc>
template<typename U, typename>
inline Vector<T,Alloc>& 
Vector<T,Alloc>::operator=(std::initializer_list<U> il){
	auto tmp=Vector(il.begin(),il.end());
	swap(tmp);
	return *this;
}

template<typename T,typename Alloc>
template<typename InputIterator,typename>
inline void 
Vector<T,Alloc>::assign(InputIterator first,InputIterator last){
	erase(begin(),end());
	insert(begin(),first,last);
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::assign(size_type n,T const& t){
	erase(begin(),end());
	insert(begin(),n,t);
}

template<typename T,typename Alloc>
auto
Vector<T,Alloc>::at(size_type n) 
-> reference {
	checkSize(n);
	return *(begin() + n);
}

template<typename T,typename Alloc>
auto
Vector<T,Alloc>::at(size_type n) const 
-> const_reference {
	checkSize(n);
	return *(begin() + n);
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::resize(size_type sz){
	if(sz<=size())
		erase(begin()+sz,end());
	else
		resize(sz,T{});
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::resize(size_type sz,T const& c){
	if(sz<size())
		erase(begin()+sz,end());
	else if(sz>size())
		insert(end(),sz-size(),c);
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::reserve(size_type n){
	checkCapacity(n);
	
	const auto old_sz = size();	
	pointer new_first;
	
	if (n > capacity()) {
		// NOTE: STL use constexpr if here, but this is not necessary, I guess it is just acquire compile-time branch switch
		if (useReallocPolicy) {
			new_first = static_cast<pointer>(realloc(this->first_, n * sizeof(T)));
			if (!new_first) {
				throw std::bad_alloc {};
			}
		}
		else {
			new_first = AllocTraits::allocate(*this, n);
			// In fact, TinySTL::copy use __builtin_memmove() when 
			// * iterator is pointer(so, it must be RandomAccessIterator)
			// * value_type of range iteraot and result is same
			// * value_type is trivially_copyable
			// @see stl_algobase.h
			TRY_BEGIN	
				TinySTL::uninitialized_copy(
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(begin()),
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end()),
						new_first);
			TRY_END
			CATCH_ALL_BEGIN
				AllocTraits::deallocate(*this, new_first, n);	
				RETHROW
			CATCH_END

			// non-POD type, we should destroy it to call dtor which may reclaim the resource
			if (! Is_trivially_copyable<T>::value)
				AllocTraits::destroy(*this, begin(), end());			
			AllocTraits::deallocate(*this, this->first_, capacity());
		}

		this->first_ = new_first;
		this->last_ = new_first + old_sz;
		this->capa_ = new_first + n;
	}	

}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::shrink_to_fit(){
	Vector<T, Alloc> self(size());
	TinySTL::uninitialized_move_if_noexcept(begin(), end(), self.begin());
	this->swap(self);
}

//modifiers:
template<typename T,typename Alloc>
template<typename...Args>
auto
Vector<T,Alloc>::emplace(const_iterator position,Args&&...args)
-> iterator {
	const auto offset = position - begin();
	auto pos = const_cast<iterator>(position);

	if(this->last_ < this->capa_ ){
		if(position == end())
			emplace_back(TinySTL::forward<Args>(args)...);
		else{
			AllocTraits::construct(*this, end(),STL_MOVE(back()));
			//avoid the intersection of source interval and destination interval
			//should use copy_backward
			copy_backward(
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(pos),
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(this->last_-1),
				this->last_);
			*position = value_type(TinySTL::forward<Args>(args)...);
			++this->last_;
		}
	}
	else{
		expandAndEmplace(position, STL_FORWARD(Args, args)...);
	}

	return begin() + offset;
}

template<typename T,typename Alloc>
template<typename...Args>
void 
Vector<T,Alloc>::emplace_back(Args&&... args){
	if (this->last_ < this->capa_) {
		AllocTraits::construct(*this, ADDRESSOF(*end()), TinySTL::forward<Args>(args)...);
		++this->last_;
	} else {
		//expandAndEmplace(end(), STL_FORWARD(Args, args)...);
		reserve(getNewCapacity(1));
		emplace_back(STL_FORWARD(Args, args)...);
	}
}

template<typename T,typename Alloc>
inline void 
Vector<T,Alloc>::push_back(T const& x){
	emplace_back(x);
}

template<typename T, typename Alloc>
inline auto 
Vector<T, Alloc>::insert(const_iterator position, T const& x) 
-> iterator {
	return iteratorAux(const_cast<iterator>(position), x);
}

template<typename T, typename Alloc>
inline auto 
Vector<T, Alloc>::insert(const_iterator position, size_type n, T const& x)
-> iterator {
	return insertFillAux(const_cast<iterator>(position), n, x);
}

template<typename T, typename Alloc>
template<typename II, typename>
inline auto 
Vector<T, Alloc>::insert(const_iterator position, II first, II last) 
-> iterator {
	return insertRangeAux(const_cast<iterator>(position), first, last);
}

template<typename T, typename Alloc>
template<typename U, typename>
inline auto 
Vector<T, Alloc>::insert(const_iterator position, std::initializer_list<U> il) 
-> iterator {
	return insert(position, il.begin(), il.end());
}

template<typename T, typename Alloc>
inline void
Vector<T, Alloc>::pop_back() TINYSTL_NOEXCEPT {
	checkSize();
	--this->last_;
	AllocTraits::destroy(*this, this->last_);
}

template<typename T, typename Alloc>
inline auto 
Vector<T, Alloc>::erase(const_iterator position) TINYSTL_NOEXCEPT
-> iterator  {
	return erase_aux(const_cast<iterator>(position));
}

template<typename T, typename Alloc>
inline auto
Vector<T, Alloc>::erase(const_iterator first, const_iterator last) TINYSTL_NOEXCEPT
-> iterator {
	return erase_aux(
		const_cast<iterator>(first),
		const_cast<iterator>(last));
}

template<typename T,typename Alloc>
auto
Vector<T,Alloc>::insertAux(iterator position,T const& x)
-> iterator {
	if(this->last_ < this->capa_) {
		AllocTraits::construct(*this, ADDRESS(*end()), STL_MOVE(back()));
		TinySTL::copy_backward(
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position),
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end() - 1),
			end());
		++this->last_;
		*position = x;
	} else {
		return insertFillNAux(position, size_type(1), x);
	}
}

template<typename T,typename Alloc>
auto 
Vector<T,Alloc>::insertFillNAux(iterator position,size_type n,T const& x)
-> iterator {
	const size_type offset = position - begin();

	if (n > 0) {
		const size_type remaining_storage = capacity() - size();
		const size_type eles_after_pos = end() - position;

		if (remaining_storage >= n) {
			if (eles_after_pos <= n) {
				auto tmp = TinySTL::uninitialized_move_if_noexcept(position, end(), position+n);
				TinySTL::fill_n(position, eles_after_pos, x);
				TinySTL::uninitialized_fill_n(end(), n - eles_after_pos, x);
				this->last_ = tmp;	
			} else {
				auto tmp = TinySTL::uninitialized_move_if_noexcept(end()-n, end(), end());
				TinySTL::copy_backward(
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position), 
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(
							position + eles_after_pos - n), 
						position+n);
				TinySTL::fill_n(position, n, x);
				this->last_ = tmp;
			}
		} else {
			const size_type old_size = size();
			const auto new_capa = getNewCapacity(n);
			const auto new_first = AllocTraits::allocate(*this, new_capa);
			decltype(new_first) tmp = new_first;
			TRY_BEGIN
				tmp = TinySTL::uninitialized_move_if_noexcept(begin(), position, tmp);
				tmp = TinySTL::uninitialized_fill_n(tmp, n, x);
				tmp = TinySTL::uninitialized_move_if_noexcept(position, end(), tmp);
			TRY_END
			CATCH_ALL_BEGIN
				// No need to call destroy because uninitialized_* has handle this case
				AllocTraits::deallocate(*this, new_first, new_capa);
				RETHROW
			CATCH_END
			
			AllocTraits::deallocate(*this, this->first_, capacity());

			this->first_ = new_first;			
			this->last_ = this->first_ + old_size + n;
			this->capa_ = this->first_ + new_capa;
		}
	}	
	
	return begin() + offset;
}

template<typename T, typename Alloc>
template<typename II>
auto 
Vector<T, Alloc>::insertRangeAux(iterator position, II first, II last) 
-> iterator {
	const size_type offset = position - begin();
	const size_type n = TinySTL::distance(first, last);

	if (n > 0) {
		const size_type remaining_storage = capacity() - size();
		const size_type eles_after_pos = end() - position;

		if (remaining_storage >= n) {
			if (eles_after_pos <= n) {
				auto tmp = TinySTL::uninitialized_move_if_noexcept(position, end(), position+n);
				auto tmp2 = TinySTL::advance(first, eles_after_pos);
				TinySTL::copy(first, tmp2, position);
				TinySTL::uninitialized_copy(tmp2, last, begin() + eles_after_pos);
				this->last_ = tmp;	
			} else {
				auto tmp = TinySTL::uninitialized_move_if_noexcept(end()-n, end(), end());
				TinySTL::copy_backward(
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position), 
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position + eles_after_pos - n), 
						position+n);
				TinySTL::copy(first, last, position);
				this->last_ = tmp;
			}
		} else {
			const size_type old_size = size();
			const auto new_capa = getNewCapacity(n);
			const auto new_first = AllocTraits::allocate(*this, new_capa);
			decltype(new_first) tmp = new_first;
			TRY_BEGIN
				tmp = TinySTL::uninitialized_move_if_noexcept(begin(), position, tmp);
				tmp = TinySTL::uninitialized_copy(first, last, tmp);
				tmp = TinySTL::uninitialized_move_if_noexcept(position, end(), tmp);
			TRY_END
			CATCH_ALL_BEGIN
				// No need to call destroy because uninitialized_* has handle this case
				AllocTraits::deallocate(*this, new_first, new_capa);
				RETHROW
			CATCH_END
			
			AllocTraits::deallocate(*this, this->first_, capacity());

			this->first_ = new_first;			
			this->last_ = this->first_ + old_size + n;
			this->capa_ = this->first_ + new_capa;
		}
	}	
	
	return begin() + offset;
}

template<typename T, typename Alloc>
template<typename ...Args>
void 
Vector<T, Alloc>::expandAndEmplace(const_iterator pos, Args&&... args) {
	const auto new_capa = getNewCapacity(1);
	const auto old_size = size();
	const auto offset = pos - begin();
	auto position = const_cast<iterator>(pos);

	const auto new_first = AllocTraits::allocate(*this, new_capa);
	auto tmp = new_first;

	TRY_BEGIN
		tmp = TinySTL::uninitialized_move_if_noexcept(
			begin(), position, tmp);
		AllocTraits::construct(*this, tmp, STL_FORWARD(Args, args)...);
		tmp = TinySTL::uninitialized_move_if_noexcept(
			position, end(), tmp + 1);
	TRY_END
	CATCH_ALL_BEGIN
		AllocTraits::deallocate(*this, new_first, new_capa);
	CATCH_END

	this->first_ = new_first;
	this->last_ = this->first_ + old_size + 1;
	this->capa_ = this->first_ + new_capa;
}

// Because erase no need to expand space by reallocate
// The policy is very simple
template<typename T,typename Alloc>
auto
Vector<T,Alloc>::erase_aux(iterator first,iterator last)
-> iterator {
	const auto offset = first - begin();
	auto tmp = TinySTL::copy(
		MAKE_MOVE_IF_NOEXCEPT_ITERATOR(last),
		MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end()),
		first);

	AllocTraits::destroy(*this, tmp, end());


	return begin() + offset;
}

// Swap the element which want to erase but not back() and back(), 
// make the operation time complexity to O(1)
// @note
// The treatment is not taken by STL.
// In STL, if position is not last element location,
// will call copy().
// @see https://en.cppreference.com/w/cpp/container/vector/erase
// So, it's time complexity is O(n).
template<typename T,typename Alloc>
auto
Vector<T,Alloc>::erase_aux(iterator position)
-> iterator {
	const auto offset = position - begin();
	if (NextIter(position) != end())  {
		TinySTL::iter_swap(position, PrevIter(end()));	
	}
	--this->last_;
	AllocTraits::destroy(*this, end());

	return begin() + offset;
}

template<typename T, typename Alloc>
void
Vector<T, Alloc>::checkSize(size_type n) {
	if (n > size()) {
		char buf[64];
		snprintf(buf, sizeof buf, 
		"out_of_range: The location %lu is not exist(size: %lu)\n", n, size());
		throw std::out_of_range(buf);
	}
}

template<typename T, typename Alloc>
void
Vector<T, Alloc>::checkCapacity(size_type n) {
	if (n > max_size()) {
		throw std::length_error {
			"length_error: The given Capacity to expand is greater than"
			"the max_size"
		};
	}
}

// Reallocate policy:
// if len above capacity(), new capacity is len + capacity()
// else new capacity is 2 * capacity
// @note
// I don't take the policy which select reallocator factor
// is 1.5 instead of 2, because I get some information which
// say 2 is better.
// But, folly and MSVC is take 1.5 currently.
// (I don't view any paper about the topic)
template<typename T,typename Alloc>
inline auto
Vector<T,Alloc>::getNewCapacity(size_type len)const 
-> size_type {
	return capacity() + ((len > capacity()) ? len : capacity());
	//return 2 * capacity() + len;
}

} // namespace TinySTL

#endif // TINYSTL_VECTOR_H