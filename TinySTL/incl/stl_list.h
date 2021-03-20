#ifndef STL_LIST_H
#define STL_LIST_H

#include "Allocator.h"
#include <initializer_list>
#include "Iterator.h"
#include "stl_algorithm.h"
#include "type_traits.h"

namespace TinySTL {
	template<typename T>
	struct list_node {
		list_node* prev;
		list_node* next;
		T data;
		list_node(T const& d,list_node *p=nullptr,list_node *n=nullptr)
			:prev(p),next(n),data(d){}
		list_node(T&& d,list_node* p = nullptr, list_node* n = nullptr)
			:prev(p), next(n), data(TinySTL::move(d)) {}
	};

	template<typename T,typename ref,typename ptr>
	class list_iter :public iterator<Bidirectional_iterator_tag,T,std::ptrdiff_t,ptr,ref>{
	    template<typename ,typename >
	    friend class list;
	protected:
	    using base=iterator<Bidirectional_iterator_tag,T,std::ptrdiff_t,ptr,ref>;
	    using c_ref=Remove_reference_t<ref> const&;
	    using c_ptr=Remove_pointer_t<ptr> const*;
	    using self=list_iter;
        using link_type = list_node<T>*;
		link_type current;
	public:
		list_iter()=default;
		list_iter(link_type n):current(n){}

		list_iter(self const& other) :current(other.current){}
		//list_const_iter(list_const_iter &&other):current(TinySTL::move(other.current)){}

		typename base::reference operator*()const {
			return current->data;
		}

		typename base::pointer operator->()const {
			return &(operator*());
		}

        self& operator++() {
			current = current->next;
			return *this;
		}

        self operator++(int) {
			link_type tmp = *this;
			current=current->next;
			return tmp;
		}

        self& operator--() {
			current = current->prev;
			return *this;
		}

        self operator--(int) {
			link_type tmp = *this;
			current=current->prev;
			return tmp;
		}

		//conversion operator to const_iterator
		//to handle such as:
		//insert(const_iterator)
		//		but given iterator
		operator list_iter<T,c_ref,c_ptr>(){
		    return current;
		}

		friend bool operator==(self const& lhs, self const& rhs) {
			return lhs.current == rhs.current;
		}

		friend bool operator!=(self const& lhs, self const& rhs) {
			return !(lhs == rhs);
		}
	
	};

	template<typename T, typename Alloc = allocator<T>>
	class list {
	public:
		using value_type				= T;
		using reference					= value_type&;
		using const_reference			= value_type const&;
		using pointer					= typename Alloc::pointer;
		using const_pointer				= typename Alloc::const_pointer;
		using size_type					= std::size_t;
		using difference_type			= std::ptrdiff_t;

		using allocator_type			= Alloc;
		using iterator					= list_iter<T,reference,pointer>;
		using const_iterator			= list_iter<T,const_reference,const_pointer>;
		using reverse_iterator			= TinySTL::reverse_iterator<iterator>;
		using const_reverse_iterator	= TinySTL::reverse_iterator<const_iterator>;
	public:
		//construct/copy/destroy
		//1.default ctor
		//2.numeric ctor
		//2.1 default(value) initialization
		//2.2 given a value
		//3.range ctor
		explicit list():node(empty_init()){}
		explicit list(size_type n);
		list(size_type n, T const& value);
		template<typename II,typename =Enable_if_t<is_input_iterator<II>>>
		list(II first, II last);
		
		//copy ctor
		list(list const&);
		//move ctor
		list(list&&)noexcept;
		//initializer list 
		list(std::initializer_list<T>);
		//dtor
		~list(){ if(node) clear(); }
	
		//assignment:
		list& operator=(list const&);
		list& operator=(list&&)noexcept;
		list& operator=(std::initializer_list<T>);
		
		template<typename II,typename =Enable_if_t<is_input_iterator<II>>>
		void assign(II first,II last);
		void assign(size_type n, T const& t);
		void assign(std::initializer_list<T>);
		allocator_type get_allocator()const noexcept{ return allocator_type{}; }

		//iterators:
		iterator begin()noexcept {
			return iterator(node->next);
		}

		const_iterator begin()const noexcept {
			return const_iterator(node->next);
		}

		iterator end() noexcept{
			return iterator(node);
		}

		const_iterator end()const noexcept {
			return const_iterator(node);
		}

		reverse_iterator rbegin()noexcept {
			return reverse_iterator(begin());
		}

		const_reverse_iterator rbegin()const noexcept {
			return const_reverse_iterator(begin());
		}

		reverse_iterator rend()noexcept {
			return reverse_iterator(end());
		}

		const_reverse_iterator rend()const noexcept {
			return const_reverse_iterator(end());
		}

		const_iterator cbegin()const noexcept {
			return begin();
		}

		const_iterator cend()const noexcept {
			return end();
		}

		const_reverse_iterator crbegin()const noexcept {
			return rbegin();
		}

		const_reverse_iterator crend()const noexcept {
			return rend();
		}

		//capacity:
		bool empty()const noexcept {
			return begin() == end();
		}

		size_type size()const noexcept{ return distance(begin(),end()); }
		size_type max_size()const noexcept{ return static_cast<size_type>(-1) ;}
		void resize(size_type n);
		void resize(size_type n, T const& x);

		//elements access:
		reference front(){ return *begin(); }
		const_reference front()const{ return *begin(); }
		reference back(){ return *(--end()); }
		const_reference back()const{ return  *(--end()); }

		//modifies:
		//emplace:
		template<typename... Args>
		void emplace_front(Args&&... args);
		template<typename... Args>
		void emplace_back(Args&&... args);
		template<typename... Args>
		iterator emplace(const_iterator pos,Args&&... args);

		//pop
		void pop_front();
		void pop_bacK();

		//push
		void push_front(const T& x);
		void push_front(T&& x);
		void push_back(const T& x);
		void push_back(T&& x);

		//insert:
		//1.single point
		//1.1 copy version
		//1.2 move version
		//2.range version
		//3.initializer_list version
		iterator insert(const_iterator pos, const T& x);
		iterator insert(const_iterator pos, T&& x);
		iterator insert(const_iterator pos, size_type n, const T&);
		template<typename II,typename =Enable_if_t<is_input_iterator<II>>>
		iterator insert(const_iterator pos, II first, II last);
		iterator insert(const_iterator pos, std::initializer_list<T>);

		//erase:
		//single point
		//range
		void erase(const_iterator pos);
		void erase(const_iterator pos, const_iterator last);

		//list operations:
		void splice(const_iterator pos,list& x);
		void splice(const_iterator pos,list&& x);
		void splice(const_iterator pos,list& x,const_iterator i);
		void splice(const_iterator pos,list&& x,const_iterator i);
		void splice(const_iterator pos,list& x,const_iterator first,const_iterator last);
		void splice(const_iterator pos,list&& x,const_iterator first,const_iterator last);
		
		void remove(T const& value);

		template<typename Pred>
		void remove_if(Pred pred);
		
		void unique();

		template<typename BiPred>
		void unique(BiPred bp);

		void merge(list & l);
		void merge(list&& l);
		template<typename Cmp>
		void merge(list & l,Cmp comp);
		template<typename Cmp>
		void merge(list &&l,Cmp comp);

		void sort();
		template<typename Cmp>
		void sort(Cmp);

		void reverse()noexcept;
		//member swap:
		void swap(list&)noexcept;

		//destroy all elements
		void clear()noexcept;

	private:
		//helper function:
		using node_type			=list_node<T>;
		using node_allocator	=allocator<node_type>;
		using link_type			=node_type*;

		link_type get_node() {
			auto ptr = node_allocator::allocate();
			return ptr;
		}

		link_type create_node(T const& x,link_type prev=nullptr,link_type next=nullptr) {
			auto ptr = get_node();
			//node_allocator::construct(&ptr,x,);
			node_allocator::construct(ptr, x,prev,next);
			return ptr;
		}

		template<typename... Args>
		link_type create_node(Args&&... args, link_type prev=nullptr, link_type next=nullptr) {
			auto ptr=get_node();
			node_allocator::construct(ptr,TinySTL::forward<Args>(args)...);
			return ptr;
		}

		void put_node(link_type ptr) {
			node_allocator::deallocate(ptr);
		}

		void destroy_node(link_type ptr) {
			node_allocator::destroy(ptr);
			put_node(ptr);
		}

		link_type empty_init(){
			node=get_node();
			node_allocator::construct(node,T{},node,node);
			return node;
		}

		void transfer(const_iterator pos, const_iterator first, const_iterator last);

	protected:
		link_type node;	//����ײ��㣬Ϊͷ��㣨������Ϊ�գ�
	public:
		//compare operator:
		friend bool operator == (list const& x, list const& y) {
			return x.size()==y.size()&&TinySTL::equal(x.begin(), x.end(), y.begin());
		}

		friend bool operator != (list const& x, list const& y) {
			return !(x==y);
		}

		friend bool operator<(list const& x, list const& y) {
			return TinySTL::lexicographical_compare(x.begin(), x.end(),
													y.begin(), y.end());
		}

		friend bool operator>=(list const& x, list const& y) {
			return !(x<y);
		}

		friend bool operator >(list const& x, list const& y) {
			return y<x;
		}

		friend bool operator<=(list const& x, list const& y) {
			return !(x>y);
		}

		//specialized algorithm:
		friend void swap(list& lhs, list& rhs) {
			lhs.swap(rhs);
		}
	};
}

#endif //STL_LIST_H