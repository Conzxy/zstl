#ifndef LIST_IMPL_H
#define LIST_IMPL_H

#include <stl_list.h>
#include <Functional.h>

namespace TinySTL {
	//ctor:
	template<typename T,typename Alloc>
	list<T, Alloc>::list(size_type n,T const& x):node(empty_init()){
		insert(end(),n,x);
	}

	template<typename T,typename Alloc>
	list<T, Alloc>::list(size_type n)  {
		insert(end(),n);
	}

	template<typename T,typename Alloc>
	template<typename II,typename >
	list<T, Alloc>::list(II first, II last) :node(empty_init()){
		insert(end(),first,last);
	}

	template<typename T,typename Alloc>
	list<T, Alloc>::list(list const& other):node(empty_init()) {
		insert(end(),other.begin(),other.end());
	}

	template<typename T,typename Alloc>
	list<T, Alloc>::list(list&& other)noexcept:node(other.node) {
		other.node=nullptr;
	}

	template<typename T,typename Alloc>
	list<T, Alloc>::list(std::initializer_list<T> il) :node(empty_init()){
		insert(end(),il.begin(),il.end());
	}

	//assignment:
	template<typename T,typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(list const& other) {
		using TinySTL::swap;
		list tmp(other.begin(),other.end());
		swap(*this,tmp);
		return *this;
	}

	template<typename T,typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(list&& other)noexcept{
		clear();
		node=other.node;
		other.node=nullptr;
		return *this;
	}

	template<typename T,typename Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(std::initializer_list<T> il) {
		using TinySTL::swap;
		list tmp(il.begin(),il.end());
		swap(*this,tmp);
		return *this;
	}

	//assign
	template<typename T,typename Alloc>
	template<typename II,typename >
	void list<T, Alloc>::assign(II first, II last) {
		clear();
		insert(end(),first,last);
	}

	template<typename T,typename Alloc>
	void list<T, Alloc>::assign(size_type n, T const& t) {
		clear();
		insert(end(),n,t);
	}

	template<typename T,typename Alloc>
	void list<T, Alloc>::assign(std::initializer_list<T> il) {
		clear();
		insert(end(),il);
	}

	//resize:
	template<typename T,typename Alloc>
	void list<T, Alloc>::resize(size_type n) {
		if (size()<n) {
			insert(end(),n-size());
		}
		else {
			auto it=begin();
			TinySTL::advance(it,n);
			erase(it,end());
		}
	}

	template<typename T,typename Alloc>
	void list<T, Alloc>::resize(size_type n,T const& x) {
		if (size()<n) {
			insert(end(),n-size(),x);
		}
		else {
			auto it=begin();
			TinySTL::advance(it,n);
			erase(it,end());
		}
	}

	//emplace:
	template<typename T,typename Alloc>
	template<typename... Args>
	typename list<T, Alloc>::iterator list<T,Alloc>::emplace(const_iterator pos, Args&&... args) {
		link_type cur=pos.current;
		return {cur->prev=cur->prev->next=create_node(TinySTL::forward<Args>(args)...,cur->prev,cur)};
	}

	template<typename T,typename Alloc>
	template<typename... Args>
	void list<T, Alloc>::emplace_front(Args&&... args) {
		emplace(begin(),TinySTL::forward<Args>(args)...);
	}

	template<typename T,typename Alloc>
	template<typename... Args>
	void list<T, Alloc>::emplace_back(Args&&... args) {
		emplace(end(),TinySTL::forward<Args>(args)...);
	}

	//insert:
	template<typename T,typename Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, T const& x) {
		link_type cur=pos.current;
		return {cur->prev=cur->prev->next=create_node(x, cur->prev, cur)};
	}

	template<typename T,typename Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, T&& x) {
		link_type cur=pos.current;
		return {cur->prev=cur->prev->next=create_node(TinySTL::move(x),cur->prev,cur)};
	}

	template<typename T,typename Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, size_type n, const T& x) {
		while(n--)
			insert(pos,x);
		TinySTL::advance(pos,-n);
		return pos.current;
	}

	template<typename T,typename Alloc>
	template<typename II,typename >
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos,II first, II last) {
		for(;first!=last;++first)
			insert(pos,*first);
		TinySTL::advance(pos,-distance(first,last));
		return pos.current;
	}

	template<typename T,typename Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, std::initializer_list<T> il) {
		return insert(pos,il.begin(),il.end());
	}

	//erase:
	template<typename T,typename Alloc>
	void list<T,Alloc>::erase(const_iterator pos){
	    link_type cur=pos.current;
	    cur->prev->next=cur->next;
	    cur->next->prev=cur->prev;
	    destroy_node(cur);
	}

	template<typename T,typename Alloc>
	void list<T, Alloc>::erase(const_iterator pos, const_iterator last) {
		for(;pos!=last;++pos)
			erase(pos);
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::push_back(T const& x) { insert(end(), x); }

	template<typename T,typename Alloc>
	void inline list<T,Alloc>::push_back(T&& x){ insert(end(),TinySTL::move(x)); }

	template<typename T,typename Alloc>
	void inline list<T,Alloc>::push_front(T const& x){insert(begin(),x); }

	template<typename T,typename Alloc>
	void inline list<T,Alloc>::push_front(T &&x){insert(begin(),TinySTL::move(x)); }

	template<typename T,typename Alloc>
	void inline list<T,Alloc>::pop_front(){erase(begin()); }

	template<typename T,typename Alloc>
	void inline list<T,Alloc>::pop_bacK(){erase(--end()); }

    template<typename T,typename Alloc>
    void list<T,Alloc>::clear()noexcept{
		if (node) {
			auto cur=node->next;
			while(cur!=node) {
				auto tmp = cur;
				cur = cur->next;
				erase(tmp);
			}
			node->prev=node;
			node->next=node;
		}
    }

	//list operations:
	//remove:
	template<typename T,typename Alloc>
	void list<T, Alloc>::remove(T const& value) {
		auto first=begin();
		auto last=end();
		while(first!=last) {
			auto cur=first;
			++cur;
			if(*first==value)
				erase(first);
			first=cur;
		}
	}

	template<typename T,typename Alloc>
	template<typename Pred>
	void list<T, Alloc>::remove_if(Pred pd) {
		auto first=begin();
		auto last=end();
		while (first!=last) {
			auto cur=first;
			++cur;
			if(pd(*first)!=false)
				erase(first);
			first=cur;
		}
	}

	
	template<typename T,typename Alloc>
	void list<T, Alloc>::unique() {
		auto first=begin();
		auto last=end();
		if(first==last) return ;
		auto next=first;
		while (++next!=last) {
			if(*next==*first)
				erase(next);
			else
				first=next;
			next=first;
		}
	}

	template<typename T,typename Alloc>
	template<typename BiPred>
	void list<T, Alloc>::unique(BiPred bp) {
		auto first=begin();
		auto last=end();
		if(first==last) return ;
		auto next=first;
		while (++next!=last) {
			if(bp(*next,*first))
				erase(next);
			else
				first=next;
			next=first;
		}
	}
	//transfer:
	template<typename T,typename Alloc>
	void list<T, Alloc>::transfer(const_iterator pos, const_iterator first, const_iterator last) {
		if(pos==last) return ;

		auto _pos=pos.current;
		auto _first=first.current;
		auto _last=last.current;

		//rewrite prev->next
		_pos->prev->next=_first;
		_first->prev->next=_last;
		_last->prev->next=_pos;

		//rewrite prev
		//notice: order of assignment
		auto tmp=_pos->prev;
		_pos->prev=_last->prev;
		_last->prev=_first->prev;
		_first->prev=tmp;
	}

	//splice:
	template<typename T,typename Alloc>
	void inline list<T,Alloc>::splice(const_iterator pos, list& x) {
		if (&x!=this&&!x.empty()) {
			transfer(pos,x.begin(),x.end());
		}
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::splice(const_iterator pos, list&& x) {
		splice(pos,x);
	}

	template<typename T,typename Alloc>
	void list<T, Alloc>::splice(const_iterator pos, list& x, const_iterator i) {
		auto i_next=i;
		++i_next;
		if(pos==i||pos==i_next) return ;
		transfer(pos,i,i_next);
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::splice(const_iterator pos, list&& x, const_iterator i) {
		auto i_next=i;
		++i_next;
	    splice(pos,i,i_next);
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last) {
		if(first!=last)
			transfer(pos,first,last);
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::splice(const_iterator pos, list&& x, const_iterator first, const_iterator last) {
		splice(pos,x,first,last);
	}

	//merge:
	template<typename T,typename Alloc>
	void list<T, Alloc>::merge(list& l) {
	    merge(l,TinySTL::less<T>{});
	}

	template<typename T,typename Alloc>
	void inline list<T, Alloc>::merge(list&& l) {
		merge(l);
	}

	//requires:comp shall define a strict weak ordering
	template<typename T,typename Alloc>
	template<typename Cmp>
	void list<T, Alloc>::merge(list& l, Cmp comp) {
		if (&l==this) return;
		auto first1=begin();
		auto last1=end();
		auto first2=l.begin();
		auto last2=l.end();

		while (first1!=last1&&first2!=last2) {
			if (comp(*first2,*first1)) {
				auto next=first2;
				transfer(first1, first2, ++next);
				first2=next;
			}
			else
				++first1;
		}
		if (first2!=last2)
			transfer(last1, first2, last2);
	}

	template<typename T,typename Alloc>
	template<typename Cmp>
	void inline list<T, Alloc>::merge(list&& l, Cmp comp) {
		merge(l,comp);
	}

	//reverse:
	template<typename T,typename Alloc>
	void list<T, Alloc>::reverse()noexcept {
		if(size()<=1)
			return ;
		
		auto first=begin().current;
		auto last=end().current;
		while (first!=last) {
			TinySTL::swap(first->prev,first->next);
			first=first->prev;
		}
		TinySTL::swap(last->prev,last->next);
		
		/*auto first=begin();
		auto last=end();
		++first;
		while (first!=last) {
			auto old=first;
			transfer(begin(),old,++first);
		}*/
	}

	//sort:
	template<typename T,typename Alloc>
	void list<T, Alloc>::sort() {
		sort(TinySTL::less<T>{});
	}

	template<typename T,typename Alloc>
	template<typename Cmp>
	void list<T, Alloc>::sort(Cmp comp) {
		if(size()<2)
			return ;

		list carry;
		list counter[64];	//最多可容纳2^64-1个元素，即64位数最大值
		int fill=0;

		using TinySTL::swap;
		while (!empty()) {
			carry.splice(carry.begin(),*this,begin());
			int i=0;
			while (i<fill&&!counter[i].empty()) {
				counter[i].merge(carry);
				swap(carry,counter[i++]);
			}

			swap(carry,counter[i]);
			if(i==fill)
				++fill;
		}

		for(int i=1;i!=fill;++i)
			counter[i].merge(counter[i-1]);
		swap(*this,counter[fill-1]);
	}

	//specilized algorithm:
	template<typename T,typename Alloc>
	void inline list<T, Alloc>::swap(list& other)noexcept {
		TinySTL::swap(node,other.node);
	}
}

#endif  //LIST_IMPL_H
