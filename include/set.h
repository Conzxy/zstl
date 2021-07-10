/*
 * @file set.h
 * implemetation of std::set
 * @date 8-7-2021
 * @author Conzxy
 */

#ifndef _ZXY_TINYSTL_INCLUDE_SET_H_
#define _ZXY_TINYSTL_INCLUDE_SET_H_
#include "stl_tree.h"
#include "functional.h"

namespace TinySTL{

template<typename T, 
	 typename Compare = TinySTL::less<T>,
	 typename Alloc = TinySTL::allocator<T>>
class set{
//TODO: BINARY_CALLABLE_CHECK
public:
	using Rep = RBTree<T, T, identity<T>, Compare, Alloc>;
	using key_type = typename Rep::key_type;
	using value_type = typename Rep::value_type;
	using key_compare = typename Rep::Compare;
	using allocator_type = typename Rep::allocator_type;
	using pointer = typename Rep::pointer;
	using const_pointer = typename Rep::const_pointer;
	using reference = typename Rep::reference;
	using const_reference = typename Rep::const_reference;
	using size_type = typename Rep::size_type;
	using defference_type = typename Rep::difference_type;
	using iterator = typename Rep::iterator;
	using const_iterator = typename Rep::const_iterator;
	using reverse_iterator = typename Rep::reverse_iterator;
	using const_reverse_iterator = typename Rep::const_reverse_iterator;

	set(){ }
	~set() { }

	set(set const& rhs) : rb_(rhs.rb_) { }
	set& operator=(set const& rhs){
		rb_ = rhs.rb_;
		return *this;
	}
	
	
#if __cplusplus > 201103L
	set(set&& rhs) noexcept 
		: rb_(STL_MOVE(rhs.rb_)) { }

	set& operator=(set&& rhs) noexcept {
		rb_ = STL_MOVE(rhs.rb_);
		return *this;
	}
#endif
	
	Alloc get_allocator() const noexcept 
	{ return Alloc(); }

	//iterator interface
	iterator begin() noexcept
	{ return rb_.begin(); }

	const_iterator begin() const noexcept
	{ return rb_.begin(); }
	
	iterator end() noexcept 
	{ return rb_.end(); }

	const_iterator end() const noexcept
	{ return rb_.end(); }
	
	const_iterator cbegin() const noexcept
	{ return rb_.cbegin(); }
	
	const_iterator cend() const noexcept
	{ return rb_.cend(); }

	reverse_iterator rbegin() noexcept
	{ return rb_.rbegin(); }

	const_reverse_iterator rbegin() const noexcept
	{ return rb_.rbegin(); }

	reverse_iterator rend() noexcept
	{ return rb_.rend(); }

	const_reverse_iterator rend() const noexcept
	{ return rb_.rend(); }

	//capacity
	size_type size() const noexcept
	{ return rb_.size(); }

	bool empty() const noexcept
	{ return size() == 0; }
	
	//modifiers
	void clear() noexcept 
	{ rb_.clear(); }

	TinySTL::pair<iterator, bool> insert(value_type const& x)
	{ return rb_.InsertUnique(x); }

	TinySTL::pair<iterator, bool> insert(value_type&& x)
	{ return rb_.InsertUnique(STL_MOVE(x)); }

	
private:
	Rep rb_;
};

} //namespace TinySTL


#endif //_ZXY_TINYSTL_INCLUDE_SET_H
