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

/*
 * @class vector_base
 * @brief memery management implemetation of vector
 * @note: Use RAII technique or as base class both is OK
 *		  only delegate the momery management to other part instead of vector<> itself.
 *
 *		  Allocator as base class is based a basic facts: allocator is a empty class usually
 *		  so we can take advantage of EBCO(empty base class optimization)
 */
template<typename T,typename Allocator>
struct vector_base : Allocator {
	using AllocTraits = allocator_traits<Allocator>;
	using size_type   = typename AllocTraits::size_type;

	vector_base() 
		: first_{nullptr}
		, last_{nullptr}
		, capa_{nullptr}
	{ }
	
	// FIXME: Using two phase initialization is better?
	explicit vector_base(size_type n)
		STL_TRY : first_{AllocTraits::allocate(*this, n)}
				, last_{first_}
				, capa_{first_+n}
	{ }
	catch(std::bad_alloc const& e) {
		AllocTraits::deallocate(*this, this->first_);
		RETHROW
	}

	vector_base(T const* first,T const* last)
		: vector_base(last-first)
	{ }

	~vector_base()
	{ AllocTraits::deallocate(*this, first_, capa_ - last_); }

	vector_base(vector_base&& base) TINYSTL_NOEXCEPT
		: first_{base.first_}
		, last_{base.last_}
		, capa_{base.capa_} 
	{ base.first_ = base.last_ = base.capa_ = nullptr; }

	vector_base& operator=(vector_base&& base) TINYSTL_NOEXCEPT{
		this->swap(base); 
		return *this; 
	}

	void swap(vector_base& rhs) TINYSTL_NOEXCEPT {
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
 * @class vector
 * @brief implemetation of vector
 * @see https://en.cppreference.com/w/cpp/container/vector for detail
 */
template<typename T,typename Allocator=allocator<T>>
class vector : protected vector_base<T, Allocator> {
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
	using base                   = vector_base<T,Allocator>;
	using AllocTraits            = typename base::AllocTraits;
public:
	// ctors:
	vector()=default;

	vector(const size_type n,value_type const& val)
		: base(n)
	{
		uninitialized_fill_n(begin(),n,val);
	}
	
	// if T is deleted default constructor, we just alloc space for it
	// we can call insert(), emplace_*() or push_*() to fill after
	//
	// @note: the feature is not supported by STL, it just is my added
	explicit vector(size_type n)
		: base(n)
	{ vector_aux<value_type>(n); }
	
	// The Enable_if_t is necessary here.
	// Because if you call vector(int, int) to call <size, val> version,
	// but compiler ultimately call iterator range version
	// This is not a error, see overload resolution for detail
	template<typename InputIterator,
		Enable_if_t<is_input_iterator<InputIterator>::value,int> =0>
	vector(InputIterator first,InputIterator last)
		: vector(distance(first,last))
	{
		uninitialized_copy(first,last,begin());
	}
	
	// dtor:	
	~vector(){ destroy(begin(),end()); }

	// copy & move ctor
	vector(vector const& rhs)
		: vector(rhs.begin(),rhs.end()){}

	vector(vector&& rhs) TINYSTL_NOEXCEPT 
		:  base(STL_MOVE(rhs))
	{ }
	
	// The Enable_if_t is not necessay here, it is just ensure instantiation safe	
	// (i.e. detect the error in compile time before the run time)
	template<typename U, typename =
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	vector(std::initializer_list<U> il)
		: vector(il.begin(),il.end()){}
	
	// operator=:
	vector& operator=(vector const&);
	vector& operator=(vector&& rhs) TINYSTL_NOEXCEPT {
		this->swap(rhs); 
		return *this;
	}
	
	template<typename U, typename = 
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	vector& operator=(std::initializer_list<U>);

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

	template<typename... Args>
	iterator emplace(const_iterator position,Args&&... args);

	iterator insert(const_iterator position,T const& x) {
		auto pos = const_cast<iterator>(position);
		insert_aux(pos ,x);
		return pos;
	}

	iterator insert(const_iterator position,size_type n,T const& x) {
		auto pos = const_cast<iterator>(position);
		insert_aux(pos, n, x);
		return pos;
	}

	template<typename InputIterator,typename =
		Enable_if_t<is_input_iterator<InputIterator>::value>>
	iterator insert(const_iterator position,
					InputIterator first,InputIterator last) {
		auto pos = const_cast<iterator>(position);
		insert_aux(pos, first, last);
		return pos;
	}

	template<typename U, typename = 
		TinySTL::Enable_if_t<Is_convertible<U, T>::value>>
	iterator insert(const_iterator position,std::initializer_list<U> il) {
		auto pos = const_cast<iterator>(position);
		insert_aux(pos, il.begin(), il.end());
		return pos;
	}

	void pop_back(){
		TINYSTL_DEBUG(size());
		--this->last_;
		AllocTraits::destroy(*this, this->last_);
	}

	iterator erase(const_iterator position) {
		return erase_aux(const_cast<iterator>(position));
	}

	iterator erase(const_iterator first,const_iterator last) {
		auto f = const_cast<iterator>(first);
		auto l = const_cast<iterator>(last);
		return erase_aux(f, l);
	}

	void swap(vector& rhs)TINYSTL_NOEXCEPT
	{ this->base::swap(rhs); }

	void clear()TINYSTL_NOEXCEPT
	{ erase(begin(),end()); }

private:
	size_type getNewCapacity(size_type len)const;
	void insert_aux(iterator position,T const& x);
	void insert_aux(iterator position,size_type n,T const& x);
	template<typename II>
	void insert_aux(iterator position,II first,II last);
	iterator erase_aux(iterator first,iterator last);
	iterator erase_aux(iterator position);
	
	template<typename U, TinySTL::Enable_if_t<Is_default_constructible<U>::value, char> = 0>
	void vector_aux(size_type n) {
		uninitialized_fill_n(this->first_, n, U{});
	}
		
	template<typename U, TinySTL::Enable_if_t<!Is_default_constructible<U>::value, int> = 0>
	void vector_aux(size_type )
	{ }
};

template<typename T,typename Allocator>
inline bool operator==(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return x.size()==x.size() && TinySTL::equal(x.begin(),x.end(),y.begin()); }

template<typename T,typename Allocator>
inline bool operator!=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return !(x==y); }

template<typename T,typename Allocator>
inline bool operator <(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return TinySTL::lexicographical_compare(x.begin(), x.end(),
										  y.begin(), y.end()); }

template<typename T,typename Allocator>
inline bool operator>=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return !(x<y); }

template<typename T,typename Allocator>
inline bool operator >(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return y<x; }

template<typename T,typename Allocator>
inline bool operator<=(vector<T,Allocator> const& x,vector<T,Allocator> const& y)
{ return !(y<x); }

// The reason for defining non-member function of swap is 
// to be compatible with STL(?).(just a convention)
template<typename T,typename Allocator>
inline void swap(vector<T,Allocator>& x,vector<T,Allocator>& y) 
TINYSTL_NOEXCEPT(TINYSTL_NOEXCEPT(x.swap(y)))
{ x.swap(y); }

template<typename T,typename Alloc>
vector<T,Alloc>& vector<T,Alloc>::operator=(vector const& rhs){
	if (this != &rhs) {                 
		auto r_size=rhs.size();
		
		if(r_size>capacity()){
			vector tmp(rhs);
			this->swap(tmp);
			return *this;
		}
		//容量足够时根据元素数量进行copy
		else if(r_size<=size()){
			auto new_end = TinySTL::copy(rhs.begin(),rhs.end(),begin());
			destroy(new_end ,end());
		}else{//size()<rhs.size()<=capacity()
			TinySTL::copy(rhs.begin(),rhs.begin()+size(),begin());
			TinySTL::uninitialized_copy(rhs.begin()+size(),rhs.end(),end());
		}
		this->last_=this->first_+r_size;
	}
	return *this;
}

template<typename T,typename Alloc>
template<typename U, typename>
vector<T,Alloc>& vector<T,Alloc>::operator=(std::initializer_list<U> il){
	auto tmp=vector(il.begin(),il.end());
	swap(tmp);
	return *this;
}

//接受[first,last)，将其中的元素赋给vector
template<typename T,typename Alloc>
template<typename InputIterator,typename>
void vector<T,Alloc>::assign(InputIterator first,InputIterator last){
	erase(begin(),end());
	insert(begin(),first,last);
}

//接受n个元素赋值给vector
template<typename T,typename Alloc>
void vector<T,Alloc>::assign(size_type n,T const& t){
	erase(begin(),end());
	insert(begin(),n,t);
}

//at:
template<typename T,typename Alloc>
typename vector<T,Alloc>::reference 
vector<T,Alloc>::at(size_type n) {
	if(n > size()) {
		char buf[64];
		snprintf(buf, sizeof buf, "RANGE_ERROR: The location %lu is not exist(size: %lu)\n", n, size());
		throw std::out_of_range(buf);
	}
	return *(begin() + n);
}

template<typename T,typename Alloc>
typename vector<T,Alloc>::const_reference 
vector<T,Alloc>::at(size_type n) const {
	if(n > size()) {
		char buf[64];
		snprintf(buf, sizeof buf, "RANGE_ERROR: The location %lu is not exist(size: %lu)\n", n, size());
		throw std::out_of_range(buf);
	}
	return *(begin() + n);
}

//capacity:
//将元素数量改为sz（如果size（）增大，则多出的元素值初始化）
template<typename T,typename Alloc>
void vector<T,Alloc>::resize(size_type sz){
	if(sz<=size())
		erase(begin()+sz,end());
	else
		resize(sz,T{});
}

template<typename T,typename Alloc>
void vector<T,Alloc>::resize(size_type sz,T const& c){
	if(sz<size())
		erase(begin()+sz,end());
	else if(sz>size())
		insert(end(),sz-size(),c);
}

//如果容量不足，扩大
template<typename T,typename Alloc>
void vector<T,Alloc>::reserve(size_type n){
	if(n <= capacity())
		return ;
	THROW_LENGTH_ERROR_IF(n>max_size(),
						  "The requirement is greater than maximum number of elements!");

	vector<T, Alloc> self(n);
	self.last_ = TinySTL::uninitialized_move(begin(), end(), self.begin());
	this->swap(self);
}

//缩减容量以适应元素个数
template<typename T,typename Alloc>
void vector<T,Alloc>::shrink_to_fit(){
	vector<T, Alloc> self(size());
	TinySTL::uninitialized_move(begin(), end(), self.begin());
	this->swap(self);
}

//modifiers:
template<typename T,typename Alloc>
template<typename...Args>
typename vector<T,Alloc>::iterator
vector<T,Alloc>::emplace(const_iterator position,Args&&...args){
	if(this->last_ < this->capa_ ){
		if(position == end())
			emplace_back(TinySTL::forward<Args>(args)...);
		else{
			AllocTraits::construct(*this, end(),back());
			//avoid the intersection of source interval and destination interval
			//should use copy_backward
			copy_backward(position,const_iterator(this->last_-1),this->last_);
			*position=value_type(TinySTL::forward<Args>(args)...);
			++this->last_;
		}
	}
	else{
		reserve(getNewCapacity(1));
		return emplace(position,TinySTL::forward<Args>(args)...);
	}
	return begin()+(position-begin());
}

template<typename T,typename Alloc>
template<typename...Args>
void vector<T,Alloc>::emplace_back(Args&&... args){
	if(this->last_ >= this->capa_)
		reserve(getNewCapacity(1));

	AllocTraits::construct(*this, &*end(),TinySTL::forward<Args>(args)...);
	++this->last_;
}

template<typename T,typename Alloc>
void vector<T,Alloc>::push_back(T const& x){
	if(this->last_ != this->capa_){
		AllocTraits::construct(*this, &*end(),x);
		++this->last_;
	}
	else{
		reserve(getNewCapacity(1));
		push_back(x);
	}
}

//insert_aux:
//1.position
//2.position,n
//3.range
template<typename T,typename Alloc>
void vector<T,Alloc>::insert_aux(iterator position,T const& x){
	//将[position,end())往后面移一位，x占据position位置
	if(this->last_ < this->capa_){
		AllocTraits::construct(*this, &*end(), back());
		TinySTL::copy_backward(position,iterator(this->last_ - 1), iterator(this->last_));
		++this->last_;
		*position=x;
	}else{
		insert_aux(position,size_type(1),x);
	}
}

template<typename T,typename Alloc>
void vector<T,Alloc>::insert_aux(iterator position,size_type n,T const& x){
	if(n==0)
		return ;
	size_type storage_left=this->capa_-this->last_;
	if(storage_left>=n){    //剩余空间大于新增元素
		const size_type elems_after=this->last_-position;
		iterator old_end=this->last_;
		if(elems_after <= n){
			//此时last_后面有n-elems_after个未初始化空间，初始化为x并拷贝原来的到新last_后面
			this->last_ = uninitialized_fill_n(end(),n-elems_after,x);
			this->last_ = uninitialized_copy(position,old_end,end());
			fill_n(position,elems_after,x);
		}else{
			//先将last_前n个元素往后移然后移动剩余elems_after-n个元素
			this->last_=uninitialized_copy(end()-n, end(),end());
			copy_backward(position,end()-n,old_end);
			fill_n(position,n,x);
		}
	}
	else{
		vector<T, Alloc> self;
		self.last_=uninitialized_move(begin(),position, self.begin());
		self.last_=uninitialized_fill_n(self.end(),n,x);
		self.last_=uninitialized_move(position,end(),self.end());
		this->swap(self);
	}
}

template<typename T,typename Alloc>
template<typename II>
void vector<T,Alloc>::insert_aux(iterator position,II first,II last){
	const size_type storage_left=this->capa_-this->last_;
	const size_type n=last-first;
	if(storage_left >= n){
		const size_type elems_after=end()-position;
		iterator old_end=this->last_;
		if(elems_after < n){
			this->last_=uninitialized_copy(first+elems_after,last,end());
			this->last_=uninitialized_copy(position,old_end,end());
			copy(first,first+elems_after,position);
		}else{
			this->last_=uninitialized_copy(end()-n,end(),end());
			copy_backward(position,old_end-n,old_end);
			copy(first,last,position);
		}
	}
	else{
		vector<T, Alloc> self;
		self.last_=uninitialized_copy(begin(),position,self.begin());
		self.last_=uninitialized_copy(first,last,self.end());
		self.last_=uninitialized_copy(position,end(), self.end());
		this->swap(self);
	}
}

//return:下一个元素的位置
template<typename T,typename Alloc>
typename vector<T,Alloc>::iterator
vector<T,Alloc>::erase_aux(iterator first,iterator last){
	iterator i=TinySTL::copy(last,end(),first);
	AllocTraits::destroy(*this, i,end());
	this->last_=end()-(last-first);
	return first;
}

//TO DO:删除末尾元素，若不是末尾元素=>转化成末尾元素i.e将后面的元素移前面去覆盖欲删除元素
//return:删除元素的下一个元素
template<typename T,typename Alloc>
typename vector<T,Alloc>::iterator
vector<T,Alloc>::erase_aux(iterator position){
	if(position+1!=end())
		copy(position+1,end(),position);
	--this->last_;
	AllocTraits::destroy(*this, end());
	return position;
}

//获取更大的容量
//alloc policy:
//if required size(let len) more than old_capacity,new_capacity=old_capacity+len
//otherwise,new_capacity=old_capacity*2
//But,if old_capacity=0,new_capacity=len
template<typename T,typename Alloc>
inline typename vector<T,Alloc>::size_type
vector<T,Alloc>::getNewCapacity(size_type len)const{
	return capacity() + ((len > capacity()) ? len : capacity());
}

} // namespace TinySTL

#endif // TINYSTL_VECTOR_H
