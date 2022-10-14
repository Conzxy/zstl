//
// Created by conzxy on 2021/2/14.
// 于2021/7/18日重构
// 该文件包含:
// CLASS TEMPLATE deque_const_iterator
// CLASS TEMPLATE deque

#ifndef ZSTL_DEQUE_H
#define ZSTL_DEQUE_H

#include "config.h"
#include "stl_iterator.h"
#include "stl_algorithm.h"
#include "stl_uninitialized.h"
#include "allocator.h"
#include "utility.h"
#include <initializer_list>
#include "stl_exception.h"

size_t DequeMapInitSize = 8;

namespace zstl {

// allow user specified buffer size
// since I want test best init buffer size
constexpr int_ deque_buffer_size(int_ buf_sz,int_ value_sz){
	return (buf_sz != 0) ? buf_sz : ((value_sz < 512) ? 512 / value_sz : 1);
}

template<typename T, int_ BufSize>
struct deque_const_iterator 
{
	using Self = deque_const_iterator;
	using Buf = T*;
	using BufPtr = Buf*;

	using iterator_category = Random_access_iterator_tag;
	using difference_type = STD_ ptrdiff_t;
	using value_type = T;
	using reference = T const&;
	using pointer = T const*;

	static constexpr int_ buffer_size() noexcept 
	{ return deque_buffer_size(BufSize,sizeof(T)); }

	deque_const_iterator() 
	: cur(nullptr),first(nullptr),last(nullptr),node(nullptr)
	{ }

	deque_const_iterator(Buf buf, BufPtr map) 
	: cur(buf)
	, first(*map)
	, last(*map + buffer_size())
	, node(map)
	{ }

	void swap(Self& other) noexcept {
		STL_SWAP(cur,other.cur);
		STL_SWAP(first,other.cur);
		STL_SWAP(last,other.last);
		STL_SWAP(node,other.node);
	}
	
	// In iterator design
	// T* ==> buffer
	// T** ==> node
	// need node as member since buffers are not continuous
	// 
	void set_node(BufPtr new_node) noexcept {
		node = new_node;
		first = *node;
		last = first + static_cast<difference_type>(buffer_size());
	}

	reference operator*()const noexcept {
		return *cur;
	}

	pointer operator->()const noexcept {
		return &(*cur);
	}

	difference_type operator-(Self const& iter)const noexcept {
		return difference_type(buffer_size()) * (node - iter.node-1)
				+ (cur - first) + (iter.last - iter.cur);
	}

	Self& operator++() noexcept {
		if(++cur == last){
			set_node(node+1);
			cur=first;
		}
		return *this;
	}

	Self operator++(int) noexcept {
		Self tmp=*this;
		++*this;
		return tmp;
	}

	Self& operator--() noexcept {
		if(cur == first){
			set_node(node-1);
			cur=last;
		}
		--cur;
		return *this;
	}

	Self operator--(int) noexcept {
		Self tmp=*this;
		--*this;
		return tmp;
	}

	Self& operator+=(difference_type distance){
		difference_type offset=(cur-first)+distance;
		if(offset>=0 && offset < buffer_size()){
			cur+=distance;
		}
		else{
			difference_type node_offset=offset>0?
					offset/buffer_size():
					(-static_cast<difference_type>((-offset-1)/buffer_size())-1);
			set_node(node+node_offset);
			cur=first+(offset-node_offset*static_cast<difference_type>(buffer_size()));
		}
		return *this;
	}

	Self& operator-=(difference_type distance){
		return *this+=-distance;
	}

	Self operator+(difference_type n)const{
		Self tmp=*this;
		return tmp+=n;
	}

	Self operator-(difference_type n)const{
		Self tmp=*this;
		return tmp-=n;
	}

	reference operator[](difference_type n)const{
		return *(*this+n);
	}


	Buf cur;            
	Buf first;          
	Buf last;           
	BufPtr node;
};

template<typename T, int_ BufSize>
struct deque_iterator : deque_const_iterator<T, BufSize>
{
	using reference = T&;
	using pointer = T*;
	using Self = deque_iterator;
	using Base = deque_const_iterator<T, BufSize>;

	reference operator*()const noexcept {
		return *(this->cur);
	}

	pointer operator->()const noexcept {
		return &(*(this->cur));
	}

	reference operator[](typename Base::difference_type n)const{
		return *(*this+n);
	}
};

template<typename T, int_ N>
bool operator==(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	return lhs.cur == rhs.cur;
}

template<typename T, int_ N>
bool operator!=(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	return !(lhs == rhs);
}

template<typename T, int_ N>
bool operator<(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	//如果管理同一个buffer，那么就比较cur;否则比较buffer
	return lhs.node == rhs.node?lhs.cur<rhs.cur:lhs.node<rhs.node;
}

template<typename T, int_ N>
bool operator>=(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	return !(lhs < rhs);
}

template<typename T, int_ N>
bool operator>(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	return rhs < lhs;
}

template<typename T, int_ N>
bool operator<=(deque_const_iterator<T, N> const& lhs,deque_const_iterator<T, N> const& rhs){
	return !(lhs > rhs);
}


template<typename T, typename Allocator, int_ BufSize>
struct DequeBase : protected Allocator {
	using value_type                =T;
	using allocator_type            =typename allocator_traits<Allocator>::allocator_type;
	using pointer                   =typename allocator_traits<allocator_type>::pointer;
	using const_pointer             =typename allocator_traits<allocator_type>::const_pointer;
	using reference                 =value_type&;
	using const_reference           =value_type const&;
	using iterator                  =deque_iterator<T, BufSize>;
	using const_iterator            =deque_const_iterator<T, BufSize>;
	using size_type                 =STD_ size_t;
	using difference_type           =STD_ ptrdiff_t;
	using reverse_iterator          =STL_ reverse_iterator<iterator>;
	using const_reverse_iterator    =STL_ reverse_iterator<const_iterator>;

	using Buf = T*;
	// as semantic of pointer
	// T** is not only pointer to buffer but also buffer array
	using Map = Buf*;
	using BufPtr = Buf*;
	using MapAllocTraits = allocator_traits<typename Allocator::template rebind<Buf>>;
	using BufAllocTraits = allocator_traits<Allocator>;

protected:
	iterator start_;
	iterator finish_;
	Map map_;
	size_type map_size_;

	void initMap(size_type n);
	void initBuffers(BufPtr start, BufPtr finish);
	void freeBuffers(BufPtr start, BufPtr finish);
	
	using MapAllocator_t = typename MapAllocTraits::allocator_type;
	using BufAllocator_t = typename BufAllocTraits::allocator_type;
	
	MapAllocator_t& getMapAllocator() noexcept
	{ static MapAllocator_t ret; return ret; }

	BufAllocator_t& getBufAllocator() noexcept
	{ return *this; }

	static size_type bufferSize_ = deque_buffer_size(BufSize, sizeof(T));
};

template<typename T, typename Alloc, int_ N>
void DequeBase<T, Alloc, N>::initMap(size_type n)
{
	// map init:
	// |  reserve-front | reserve-mid | reserve-back |
	size_type numNodes = n / bufferSize_;
	map_size_ = STL_ max(DequeMapInitSize, numNodes + 2);


	TRY_BEGIN
		map_ = MapAllocTraits::allocate(getMapAllocator(), map_size_);
	TRY_END
	CATCH_ALL_BEGIN
		MapAllocTraits::deallocate(map_);
	CATCH_END

	BufPtr start = map_ + (map_size_ - numNodes) / 2;
	BufPtr finish = start + map_size_ - 1;

	initBuffers(start, finish);
	
	start_.set_node(start);
	start_.cur = start_.first;

	finish_.set_node(finish);
	finish_.cur = finish_.first + n % bufferSize_;

}

template<typename T, typename Alloc, int_ N>
void DequeBase<T, Alloc, N>::initBuffers(BufPtr start, BufPtr finish)
{
	BufPtr start_copy = start;
	TRY_BEGIN
		for(; start != finish; ++start)
			*start = BufAllocTraits::allocate(getBufAllocator(), bufferSize_);
	TRY_END
	CATCH_ALL_BEGIN
		for(; start_copy != start; ++start_copy)
			BufAllocTraits::deallocate(getBufAllocator(), start_copy, bufferSize_);
	CATCH_END
}

template<typename T, typename Alloc, int_ N>
void DequeBase<T, Alloc, N>::freeBuffers(BufPtr start, BufPtr finish)
{
	for(; start != finish; ++start)
		BufAllocTraits::deallocate(getBufAllocator(), start, bufferSize_);
}

/**
 * @class deque
 * @tparam T value type
 * @tparam Allocator allocator type
 * @tparam BufSize size of every buffer
 * @brief double end queue, in fact, that is a buffer array
 * use special algorithm, make it like a linear continuous space
 */
template<typename T,typename Allocator=STL_ allocator<T>, int_ BufSize = 0>
class deque : protected DequeBase<T, Allocator, BufSize> {
public:
	using base = DequeBase<T, Allocator, BufSize>;
	using value_type                =typename base::value_type;
	using allocator_type            =typename base::allocator;
	using pointer                   =typename base::pointer;
	using const_pointer             =typename base::const_pointer;
	using reference                 =typename base::reference;
	using const_reference           =typename base::const_reference;
	using iterator                  =typename base::iterator;
	using const_iterator            =typename base::const_iterator;
	using size_type                 =typename base::size_type;
	using difference_type           =typename base::difference_type;
	using reverse_iterator          =typename base::reverse_iterator;
	using const_reverse_iterator    =typename base::const_reverse_iterator;
protected:
	using Buf = typename base::Buf;
	using Map = typename base::Map;
	using BufPtr = typename base::BufPtr;
	using MapAllocTraits = typename base::MapAllocTraits;
	using BufAllocTraits = typename base::BufAllocTraits;

public:
	deque()
	{ }

	deque(size_type n)
	{ deque_aux(n); }

	deque(size_type n,value_type const& val)
	{ fill_init(n, val); }

	template<typename Input>
	deque(Input first,Input last)
	{ range_init(first, last); }

	deque(deque const& other)
	{ range_init(other.begin(), other.end()); }

	deque(deque && other) noexcept {
		start = other.start;
		finish = other.finish;
		map = other.map;
		map_size = other.map_size;
		other.map=nullptr;
		other.map_size=0;
	}

	template<typename U, typename = Enable_if_t<Is_convertible<U, value_type>::value>>
	deque(STD_ initializer_list<U> il)
	{ renge_init(il.begin(), il.end()); }

	deque& operator=(deque const&);
	deque& operator=(deque&&)noexcept;
	~deque();

	template<typename Input>
	void assign(Input first,Input last);
	void assign(size_type n,value_type const&);
	void assign(STD_ initializer_list<value_type>);

	//iterator:
	iterator                    begin()       { return start_; }
	iterator                    end()         { return finish_; }
	const_iterator              begin()  const{ return static_cast<const_iterator>(start_); }
	const_iterator              end()    const{ return static_cast<const_iterator>(finish_); }
	const_iterator              cbegin() const{ return begin(); }
	const_iterator              cend()   const{ return end(); }
	reverse_iterator            rbegin()      { return reverse_iterator(begin()); }
	reverse_iterator            rend()        { return reverse_iterator(end()); }
	const_reverse_iterator      rbegin() const{ return const_reverse_iterator(begin()); }
	const_reverse_iterator      rend()   const{ return const_reverse_iterator(end()); }
	const_reverse_iterator      crbegin()const{ return const_reverse_iterator(cbegin()); }
	const_reverse_iterator      crend()  const{ return const_reverse_iterator(cend()); }

	//capacity
	size_type size() const{ return finish_ - start_; }
	bool      empty()const{ return start_ == finish_; }
	void      resize(size_type sz);
	void      resize(size_type sz,value_type const& val);
	void      shrink_to_fit();
	//element access
	reference       operator[](size_type index)     { return *(start_+index); }
	const_reference operator[](size_type index)const{ return *(start_+index); }
	reference       at(size_type index)     { ZSTL_DEBUG(index > 0 && index < size()); return *(start_+index); }
	const_reference at(size_type index)const{ ZSTL_DEBUG(index >0 && index < size()); return *(start_+index); }
	reference       front()      { return *start_; }
	const_reference front()const { return *start_; }
	reference       back()     { return *(finish_-1); }
	const_reference back()const{ return *(finish_-1); }

	//modifies
	template<typename ...Args> reference emplace_back(Args&&... args);
	template<typename ...Args> reference emplace_front(Args&&... args);
	template<typename ...Args> iterator emplace(const_iterator pos,Args&&... args);

	void push_back(value_type const& val);
	void push_back(value_type &&val);
	void push_front(value_type const& val);
	void push_front(value_type &&val);

	iterator insert(const_iterator pos,value_type const& val);
	iterator insert(const_iterator pos,value_type&& val);
	iterator insert(const_iterator pos,size_type n,value_type const &val);
	iterator insert(const_iterator pos,STD_ initializer_list<value_type>);
	template<typename Input>
	iterator insert(const_iterator pos,Input first,Input last);

	void pop_back();
	void pop_front();
	
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first,const_iterator last);

	void swap(deque& )noexcept;
	void clear();
private:
	void fill_init(size_type n,value_type const& val);
	template<typename Input>
	void range_init(Input first,Input last);
	void push_back_aux(value_type const& val);
	void push_front_aux(value_type const& val);
	iterator insert_aux(const_iterator pos,value_type const& val);
	iterator fill_insert(const_iterator pos,size_type n,value_type const& val);
	void reserve_map_back(size_type add_node=1);
	void reserve_map_front(size_type add_node=1);
	iterator make_iter(const_iterator iter);
	void reallocate_map(size_type add_node,bool isFront);

	template<typename U, STL_ Enable_if_t<Is_default_constructible_v<U>, int> = 0>
	void deque_aux(size_type n)
	{ fill_init(n, U{}); }

	template<typename U, STL_ Enable_if_t<!Is_default_constructible_v<U>, char> = 0>
	void deque_aux(size_type n)
	{ map_init(n); }

	friend void swap(deque& lhs,deque& rhs)noexcept(noexcept(lhs.swap(rhs))){ lhs.swap(rhs); }

protected:
	using base::start_;
	using base::finish_;
	using base::map_;
	using base::map_size_;
	using base::bufferSize_;

	using base::initMap;
	using base::initBuffers;
	using base::freeBuffers;
};

template<typename T, typename Alloc, int_ N>
template<typename... Args>
auto deque<T, Alloc, N>::emplace_back(Args&&... args) -> reference
{
	if(finish_.node + 1 == map_ + map_size){
		reserve_map_back();
		finish_.node + 1 = BufAllocTraits::allocate(getBufAllocator(), bufferSize_);
		finish_.set_node(finish_.node + 1);
		finish_.cur = finish_.first;	
	}
	STL_ construct(finish_.cur, STL_FORWARD(Args, args)...);
	finish_.cur 
	return *finish_.cur
}

} // namespace zstl

#endif //ZSTL_DEQUE_H

