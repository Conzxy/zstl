#ifndef _ZXY_TINYSTL_INCLUDE_SET_H_
#define _ZXY_TINYSTL_INCLUDE_SET_H_
#include "stl_algobase.h"
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
	using key_compare = typename Rep::key_compare;
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

	iterator insert(const_iterator hint, value_type const& val)	
	{ return rb_.InsertHintUnique(hint, val); }

	template<typename II>
	void insert(II first, II last)
	{ rb_.Insert(first, last); }

	template<typename... Args>
	TinySTL::pair<iterator, bool> emplace(Args&&... args)
	{ return rb_.EmplaceEqual(STL_FORWARD(Args, args)...); }

	template<typename ...Args>
	TinySTL::pair<iterator, bool> emplace_hint(const_iterator hint, Args&&... args)
	{ return rb_.EmplaceHintUnique(hint, STL_FORWARD(Args, args)...); }
	
	iterator erase(const_iterator pos)
	{ return rb_.Erase(pos); }

	iterator erase(const_iterator first, const_iterator last)
	{ return rb_.Erase(first, last); }

	size_type erase(key_type const& key)
	{ return rb_.Erase(key); }

	void swap(set& rhs) noexcept(noexcept(this->rb_.swap(rhs.rb_)))
	{ rb_.swap(rhs.rb_); }

	// lookup
	size_type count(key_type const& key) const
	{ return rb_.count(key); }
	
	iterator find(key_type const& key) 
	{ return rb_.find(key); }

	const_iterator find(key_type const& key) const
	{ return rb_.find(key); }

	bool contains(key_type const& key) const
	{ return contains(key); }

	TinySTL::pair<iterator, iterator>
	equal_range(key_type const& key)
	{ return rb_.equal_range(key); }

	TinySTL::pair<const_iterator, const_iterator>
	equal_range(key_type const& key) const
	{ return rb_.equal_range(key); }
	
	iterator lower_bound(key_type const& key)
	{ return rb_.lower_bound(key); }
	
	const_iterator lower_bound(key_type const& key) const
	{ return rb_.lower_bound(key); }
	
	iterator upper_bound(key_type const& key)
	{ return rb_.upper_bound(key); }

	const_iterator upper_boound(key_type const& key) const
	{ return rb_.upper_bound(key); }
	
	key_compare key_comp() const
	{ return key_compare{}; }

private:
	Rep rb_;
};

template<typename T, typename Compare, typename Alloc>
bool operator==(
		set<T, Compare, Alloc> const& lhs,
		set<T, Compare, Alloc> const& rhs)
{ 
	return lexicographical_compare(lhs.begin(), lhs.end(),
									rhs.begin(), rhs.end());
}

template<typename T, typename Compare, typename Alloc>
bool operator!=(
		set<T, Compare, Alloc> const& lhs,
		set<T, Compare, Alloc> const& rhs)
{ return !(lhs == rhs); }

		
} //namespace TinySTL


#endif //_ZXY_TINYSTL_INCLUDE_SET_H
