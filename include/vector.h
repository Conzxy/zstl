#ifndef ZSTL_VECTOR_H
#define ZSTL_VECTOR_H

#include "allocator.h"
#include "stl_exception.h"
#include "type_traits.h"
#include "stl_uninitialized.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "config.h"

#include <initializer_list>
#include <stdexcept>
#include <climits>

namespace zstl {
/**
 * @class VectorBase
 * @tparam T type of element
 * @tparam Allocator type of Allocator
 * @brief memory management implemetation of Vector
 * @note: 
 * Use RAII technique or as base class both is OK,
 * as long as it can delegate the momery management to other part instead of Vector<> itself.
 *
 * Allocator as base class is based a basic facts: allocator is a empty class usually
 * so we can take advantage of EBCO(empty base class optimization)
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
	{ 
		AllocTraits::deallocate(*this, first_, capa_ - last_);
		first_ = last_ = capa_ = nullptr;
	}

	VectorBase(VectorBase&& base) ZSTL_NOEXCEPT
		: first_{base.first_}
		, last_{base.last_}
		, capa_{base.capa_} 
	{ base.first_ = base.last_ = base.capa_ = nullptr; }

	VectorBase& operator=(VectorBase&& base) ZSTL_NOEXCEPT{
		this->swap(base); 
		return *this; 
	}

	void swap(VectorBase& rhs) ZSTL_NOEXCEPT {
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
 * @tparam Allocator type of Allocator(default is zstl::allocator)
 * @brief 
 * Implemetation of linear space which
 * support expand automately.
 * Also, you reserve space by expand space exlicitly.
 * @see https://en.cppreference.com/w/cpp/container/vector for detail
 */
template<typename T,typename Allocator = zstl::allocator<T>>
class Vector : protected VectorBase<T, Allocator> {
public:
	using value_type             = T;
	using pointer                = typename allocator_traits<Allocator>::pointer;
	using const_pointer          = typename allocator_traits<Allocator>::const_pointer;
	using reference              = value_type&;
	using const_reference        = value_type const&;
	using iterator               = T*;
	using const_iterator         = T const*;
	using reverse_iterator       = zstl::reverse_iterator<iterator>;
	using const_reverse_iterator = zstl::reverse_iterator<const_iterator>;
	using allocator_type         = Allocator;

	using size_type              = std::size_t;
	using difference_type        = ptrdiff_t;
	using base                   = VectorBase<T,Allocator>;
	using AllocTraits            = typename base::AllocTraits;
public:
	// ctors:
	Vector() = default;

	Vector(const size_type n,value_type const& val)
		: base(n)
	{
		STL_TRY {
			zstl::uninitialized_fill_n(begin(), n, val);
			this->last_ = this->first_ + n;
		}
		catch (...) {
			RETHROW	
		}
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
		// no need to catch exception
		// because RAII
		STL_TRY {
			zstl::uninitialized_copy(first,last,begin());
			this->last_ = this->first_ + capacity();
		}
		catch (...) {
			RETHROW
		}
	}

	// dtor:	
	// deallocate is delegated to base class
	~Vector()
	{ destroy(begin(),end()); }

	// copy & move ctor
	Vector(Vector const& rhs)
		: Vector(rhs.begin(),rhs.end())
	{ }

	Vector(Vector&& rhs) ZSTL_NOEXCEPT 
		:  base(STL_MOVE(rhs))
	{ }
	
	// The Enable_if_t is not necessay here, it is just ensure instantiation safe	
	// (i.e. detect the error in compile time before the run time)
	template<typename U, typename =
		zstl::Enable_if_t<Is_convertible<U, T>::value>>
	Vector(std::initializer_list<U> il)
		: Vector(il.begin(),il.end())
	{ }
	
	// operator=:
	Vector& operator=(Vector const&);
	Vector& operator=(Vector&& rhs) ZSTL_NOEXCEPT {
		this->swap(rhs); 
		return *this;
	}
	
	template<typename U, typename = 
		zstl::Enable_if_t<Is_convertible<U, T>::value>>
	Vector& operator=(std::initializer_list<U>);

	// assign operation:	
	template<typename InputIterator,typename =Enable_if_t<
			is_input_iterator<InputIterator>::value>>
	void assign(InputIterator first,InputIterator last);
	void assign(size_type n,T const& t);

	allocator_type get_allocator()const ZSTL_NOEXCEPT
	{ return Allocator(); }

	// iterators:
	iterator                begin()                     ZSTL_NOEXCEPT
	{ return iterator(this->first_); }
	iterator                end()                       ZSTL_NOEXCEPT
	{ return iterator(this->last_); }
	const_iterator          begin()             const   ZSTL_NOEXCEPT
	{ return const_iterator(this->first_); }
	const_iterator          end()               const   ZSTL_NOEXCEPT
	{ return const_iterator(this->last_); }  
	reverse_iterator        rbegin()                    ZSTL_NOEXCEPT
	{ return reverse_iterator(end()); }
	const_reverse_iterator  rbegin()            const   ZSTL_NOEXCEPT
	{ return const_reverse_iterator(end()); }
	reverse_iterator        rend()                      ZSTL_NOEXCEPT
	{ return reverse_iterator(begin()); }
	const_reverse_iterator  rend()              const   ZSTL_NOEXCEPT
	{ return const_reverse_iterator(begin()); }

	const_iterator          cbegin()            const   ZSTL_NOEXCEPT
	{ return begin(); }
	const_iterator          cend()              const   ZSTL_NOEXCEPT
	{ return end(); }
	const_reverse_iterator  crbegin()           const   ZSTL_NOEXCEPT
	{ return rbegin(); }
	const_reverse_iterator  crend()             const   ZSTL_NOEXCEPT
	{ return rend(); }

	// capacity:
	size_type   size()               const   ZSTL_NOEXCEPT
	{ return end()-begin(); }
	size_type   max_size()           const   ZSTL_NOEXCEPT
	{ return size_type(UINT_MAX/sizeof(T)); }
	void        resize(size_type sz);
	void        resize(size_type sz,T const& c);
	size_type   capacity()              const   ZSTL_NOEXCEPT
	{ return this->capa_ - this->first_; }
	bool        empty()                 const   ZSTL_NOEXCEPT
	{return begin() == end(); }
	void        reserve(size_type n);

	void        shrink_to_fit();

	// element access:
	reference           operator[](size_type n) ZSTL_NOEXCEPT
	{ return *(begin()+n); }
	const_reference     operator[](size_type n)const ZSTL_NOEXCEPT
	{ return *(cbegin()+n); }
	reference           at(size_type n);
	const_reference     at(size_type n)const;
	reference           front() ZSTL_NOEXCEPT
	{ return *begin(); }
	const_reference     front() const ZSTL_NOEXCEPT
	{ return *begin(); }
	reference           back() ZSTL_NOEXCEPT
	{ return *(end()-1); }
	const_reference     back()const ZSTL_NOEXCEPT
	{ return *(end()-1); }

	// data access:
	T*          data()  ZSTL_NOEXCEPT
	{ return this->first_; }
	T const*    data()  const ZSTL_NOEXCEPT
	{ return this->first_; }

	// modifiers:
	template<typename...Args>
	void emplace_back(Args&&... args);
	void push_back(T const& x);
	void push_back(T&& x){ emplace_back(zstl::move(x)); }

	template<typename... Args> iterator emplace(const_iterator position,Args&&... args);

	iterator insert(const_iterator position,T const& x);
	iterator insert(const_iterator position,size_type n,T const& x);

	template<typename InputIterator,typename =
		Enable_if_t<is_input_iterator<InputIterator>::value>>
	iterator insert(const_iterator position,
					InputIterator first,InputIterator last);

	template<typename U, typename = 
		zstl::Enable_if_t<Is_convertible<U, T>::value>>
	iterator insert(const_iterator position,std::initializer_list<U> il);
	void pop_back() ZSTL_NOEXCEPT;

	iterator erase(const_iterator position);
	iterator erase(const_iterator first,const_iterator last);

	void swap(Vector& rhs)ZSTL_NOEXCEPT
	{ this->base::swap(rhs); }

	// @warning clear() don't free space.
	// User can shrink size by call shrink_to_fit() in C++11,
	// also you can use Vector(v).swap(v)(used before c++11).
	// But the second approach is not recommended,
	// because 
	void clear() ZSTL_NOEXCEPT
	{ erase(begin(),end()); }

private:
	void checkSize(size_type n);
	void checkCapacity(size_type n);

	size_type getNewCapacity(size_type len)const;

	iterator insertAux(iterator position,T const& x);
	iterator insertFillNAux(iterator position, size_type n, T const& x);
	template<typename II> iterator insertRangeAux(iterator, II first, II last);
	template<typename ...Args> void expandAndEmplace(const_iterator position, Args&&... args);

	iterator eraseAux(iterator first,iterator last);
	iterator eraseAux(iterator position);
	
	template<typename U, zstl::Enable_if_t<Is_default_constructible<U>::value, char> = 0>
	void Vector_aux(size_type n) {
		STL_TRY {
			zstl::uninitialized_fill_n(this->first_, n, U{});
			this->last_ = this->first_ + n;
		}
		catch (...) {
			RETHROW
		}
	}
		
	template<typename U, zstl::Enable_if_t<!Is_default_constructible<U>::value, int> = 0>
	void Vector_aux(size_type )
	{ }

	// if element type is trivial and use default allocator
	// we use realloc() to reallocate then no need to move old element to new space
	static constexpr bool useReallocPolicy = 
		Conjunction_t<
			Is_same<Allocator, zstl::allocator<T>>,
			Is_trivially_copyable<T>>::value;

};

template<typename T,typename Allocator>
inline bool 
operator==(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y) {
	return x.size()==x.size() &&
			zstl::equal(x.begin(),x.end(),y.begin());
}

template<typename T,typename Allocator>
inline bool 
operator!=(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y)
{ return !(x==y); }

template<typename T,typename Allocator>
inline bool 
operator <(Vector<T,Allocator> const& x,Vector<T,Allocator> const& y) {
	return zstl::lexicographical_compare(
		x.begin(), x.end(), y.begin(), y.end());
}

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
ZSTL_NOEXCEPT(ZSTL_NOEXCEPT(x.swap(y)))
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
				= zstl::copy
				(rhs.begin(), rhs.end(), this->first_);

			AllocTraits::destroy(*this, this->last_, this->first_+old_sz);
		} else {
			// size() < new_sz < capacity()
			this->last_ 
				= zstl::copy
				(rhs.begin(), rhs.begin()+size(), begin());

			this->last_ 
				= zstl::uninitialized_copy
				(rhs.begin()+size(), rhs.end(), this->last_);
				

		}
	}	

	return *this;
}

template<typename T,typename Alloc>
template<typename U, typename>
inline Vector<T,Alloc>& 
Vector<T,Alloc>::operator=(std::initializer_list<U> il){
	auto tmp = Vector(il.begin(), il.end());
	swap(tmp);
	return *this;
}

// erase all elements at first and re-insert elements which are given by parameters
// @see N337(a standard draft in C++11) 23.3.6.2 page 755 for detail
template<typename T,typename Alloc>
template<typename InputIterator,typename>
inline void 
Vector<T,Alloc>::assign(InputIterator first,InputIterator last){
	clear();
	insert(begin(), first, last);
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::assign(size_type n,T const& t){
	clear();
	insert(begin(), n, t);
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
Vector<T,Alloc>::resize(size_type sz) {
	if(sz <= size())
		erase(begin()+sz, end());
	else
		resize(sz, T{});
}

template<typename T,typename Alloc>
void 
Vector<T,Alloc>::resize(size_type sz,T const& c) {
	if(sz < size())
		erase(begin()+sz, end());
	else if(sz > size())
		insert(end(), sz-size(), c);
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
			// In fact, zstl::copy use __builtin_memmove() when 
			// * iterator is pointer(so, it must be RandomAccessIterator)
			// * value_type of range iteraot and result is same
			// * value_type is trivially_copyable
			// @see stl_algobase.h
			TRY_BEGIN	
				zstl::uninitialized_copy(
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
	zstl::uninitialized_move_if_noexcept(begin(), end(), self.begin());
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
			emplace_back(zstl::forward<Args>(args)...);
		else{
			AllocTraits::construct(*this, end(),STL_MOVE(back()));
			//avoid the intersection of source interval and destination interval
			//should use copy_backward
			copy_backward(
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(pos),
				MAKE_MOVE_IF_NOEXCEPT_ITERATOR(this->last_-1),
				this->last_);
			*position = value_type(zstl::forward<Args>(args)...);
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
		AllocTraits::construct(*this, ADDRESSOF(*end()), zstl::forward<Args>(args)...);
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
	return insertAux(const_cast<iterator>(position), x);
}

template<typename T, typename Alloc>
inline auto 
Vector<T, Alloc>::insert(const_iterator position, size_type n, T const& x)
-> iterator {
	return insertFillNAux(const_cast<iterator>(position), n, x);
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
Vector<T, Alloc>::pop_back() ZSTL_NOEXCEPT {
	checkSize();
	--this->last_;
	AllocTraits::destroy(*this, this->last_);
}

template<typename T, typename Alloc>
inline auto 
Vector<T, Alloc>::erase(const_iterator position) 
-> iterator  {
	return eraseAux(const_cast<iterator>(position));
}

template<typename T, typename Alloc>
inline auto
Vector<T, Alloc>::erase(const_iterator first, const_iterator last) 
-> iterator {
	return eraseAux(
		const_cast<iterator>(first),
		const_cast<iterator>(last));
}

template<typename T,typename Alloc>
auto
Vector<T,Alloc>::insertAux(iterator position,T const& x)
-> iterator {
	const auto offset = position - begin();
	if(this->last_ < this->capa_) {
		AllocTraits::construct(*this, ADDRESSOF(*end()), STL_MOVE(back()));
		zstl::copy_backward(
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position),
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end() - 1),
			end());
		++this->last_;
		*position = x;

		return begin() + offset;
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
				auto tmp = zstl::uninitialized_move_if_noexcept(position, end(), position+n);
				zstl::fill_n(position, eles_after_pos, x);
				zstl::uninitialized_fill_n(end(), n - eles_after_pos, x);
				this->last_ = tmp;	
			} else {
				auto tmp = zstl::uninitialized_move_if_noexcept(end()-n, end(), end());
				zstl::copy_backward(
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position), 
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(
							position + eles_after_pos - n), 
						position+n);
				zstl::fill_n(position, n, x);
				this->last_ = tmp;
			}
		} else {
			const size_type old_size = size();
			const auto new_capa = getNewCapacity(n);
			const auto new_first = AllocTraits::allocate(*this, new_capa);
			iterator tmp = new_first;
			TRY_BEGIN
				tmp = zstl::uninitialized_move_if_noexcept(begin(), position, tmp);
				tmp = zstl::uninitialized_fill_n(tmp, n, x);
				tmp = zstl::uninitialized_move_if_noexcept(position, end(), tmp);
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
	const size_type n = zstl::distance(first, last);

	if (n > 0) {
		const size_type remaining_storage = capacity() - size();
		const size_type eles_after_pos = end() - position;

		if (remaining_storage >= n) {
			if (eles_after_pos <= n) {
				auto tmp = zstl::uninitialized_move_if_noexcept(position, end(), position+n);
				auto tmp2 = zstl::advance_iter(first, eles_after_pos);
				zstl::copy(first, tmp2, position);
				zstl::uninitialized_copy(tmp2, last, begin() + eles_after_pos);
				this->last_ = tmp;	
			} else {
				auto tmp = zstl::uninitialized_move_if_noexcept(end()-n, end(), end());
				zstl::copy_backward(
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position), 
						MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position + eles_after_pos - n), 
						position+n);
				zstl::copy(first, last, position);
				this->last_ = tmp;
			}
		} else {
			const size_type old_size = size();
			const auto new_capa = getNewCapacity(n);
			const auto new_first = AllocTraits::allocate(*this, new_capa);
			iterator tmp = new_first;
			TRY_BEGIN
				tmp = zstl::uninitialized_move_if_noexcept(begin(), position, tmp);
				tmp = zstl::uninitialized_copy(first, last, tmp);
				tmp = zstl::uninitialized_move_if_noexcept(position, end(), tmp);
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
		tmp = zstl::uninitialized_move_if_noexcept(
			begin(), position, tmp);
		AllocTraits::construct(*this, tmp, STL_FORWARD(Args, args)...);
		tmp = zstl::uninitialized_move_if_noexcept(
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
Vector<T,Alloc>::eraseAux(iterator first,iterator last)
-> iterator {
	const auto offset = first - begin();
	auto tmp = zstl::copy(
		MAKE_MOVE_IF_NOEXCEPT_ITERATOR(last),
		MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end()),
		first);

	AllocTraits::destroy(*this, tmp, end());


	return begin() + offset;
}

// Swap the element which want to erase but not back() and back(), 
// make the operation time complexity to O(1)
// but it requires element provide such interface that can get its index in vector
// Therefore, this is a choice of user, not a lib task.
template<typename T,typename Alloc>
auto
Vector<T,Alloc>::eraseAux(iterator position)
-> iterator {
	const auto offset = position - begin();
	if (NextIter(position) != end())  {
		zstl::copy(
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(position + 1),
			MAKE_MOVE_IF_NOEXCEPT_ITERATOR(end()),
			position);
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
		snprintf(
			buf, sizeof buf, 
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

} // namespace zstl

#endif // ZSTL_VECTOR_H