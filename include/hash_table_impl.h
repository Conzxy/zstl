#ifndef _HASH_TABLE_IMPL_H
#define _HASH_TABLE_IMPL_H

#include <hash_table.h>

namespace TinySTL{
    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    void
    HashTable<V,K,H,GK,EK,A>::init_table(const size_type n){
        const size_type table_size=next_size(n);
        table_.reserve(table_size);
        for(size_type i=0;i != table_size ;++i){
            table_.emplace_back(nullptr);
        } 
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    pair<typename HashTable<V,K,H,GK,EK,A>::iterator,bool> 
    HashTable<V,K,H,GK,EK,A>::insert_unique(const value_type& val){
        rehash(num_elements_+1);
        return insert_unique_norehash(val);
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    pair<typename HASHTABLE::iterator,bool>
    HASHTABLE::insert_unique(value_type&& val){
        rehash(num_elements_+1);
        return insert_unique_norehash(STL_MOVE(val));
    }
    
    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    typename HASHTABLE::iterator
    HASHTABLE::insert_unique(const_iterator hint,value_type const& val){
        rehash(num_elements_+1);
        return insert_unique_norehash(hint,val);
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    typename HASHTABLE::iterator
    HASHTABLE::insert_unique(const_iterator hint,value_type&& val){
        rehash(num_elements_+1);
        return insert_unique_norehash(hint,STL_MOVE(val));
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    void
    HashTable<V,K,H,GK,EK,A>::rehash(size_type num_element_hint){
        //如果给定元素大小比slots大，意味着load_factor>1
        //而分离链表法无论插入是否成功，其时间复杂度为O(1+α)
        //因此这个条件是为了让时间复杂度保持在常量时间
        if(num_element_hint > table_size()){
            auto old_table=table_;
            
            auto new_table_size=next_size(num_element_hint);
            vector<node*> new_table(new_table_size);

            TRY_BEGIN
            for(size_type index=0;index != table_size();++index){
                auto first=table_.at(index);
                while(first){
                    table_[index]=first->next_;
                    size_type new_index=hash_val(first->val_,new_table_size);
                    first->next_=new_table[new_index];
                    new_table[new_index]=first;
                    first=table_[index];
                }
            }
            TRY_END

            CATCH_ALL_BEGIN
            vector<node*> null_vector;
            new_table.swap(null_vector);
            CATCH_END

            STL_SWAP(new_table,table_);
        }
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    template<typename Args>
    pair<typename HashTable<V,K,H,GK,EK,A>::iterator,bool> 
    HashTable<V,K,H,GK,EK,A>::insert_unique_norehash(Args&& value){
        const size_type mapped_index=hash_val(value);
        node* first=table_.at(mapped_index);

        for(auto cur=first;cur != nullptr ;++cur){
            if(equal_key_(get_key_(value),get_key_(cur->val_)))
            //如果key已存在在hashtable中，则返回该key的位置并设boolean为false表示插入失败
                return pair<iterator,bool>(iterator(cur,*this),false);
        }

        //insert node to hashtable
        auto node=new_node(STL_ forward<Args>(value));
        node->next_=first;
        table_[mapped_index]=node;
        ++num_elements_;
        return pair<iterator,bool>(iterator(node,*this),true);
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    template<typename Args>
    typename HashTable<V,K,H,GK,EK,A>::iterator 
    HashTable<V,K,H,GK,EK,A>::insert_equal_norehash(Args&& val){
        const size_type mapped_index=hash_val(val);
        auto first=table_.at(mapped_index);

        for(auto cur=first;cur;++cur){
            if(equal_key(get_key_(cur->val),get_key_(val))){
                auto node=new_node();
                node->next_=cur->next_;
                cur->next_=node;
                ++num_elements_;
                return iterator(node,*this);
            }
        }

        auto node=new_node(STL_ forward<Args>(val));
        node->next_=first;
        table_[mapped_index]=node;
        ++num_elements_;
        return iterator(node,*this);
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    typename HashTable<V,K,H,GK,EK,A>::iterator
    HashTable<V,K,H,GK,EK,A>::erase(const_iterator pos){
        auto cur=pos.cur_;
        if(cur){
            auto mapped_val=hash_val(cur->val_);
            auto& head=table_[mapped_val];

            if(cur == head){
                head=head->next_;
                destroy_node(cur);
                --num_elements_;
                return make_iterator(head);
            }else{
                //不需要考虑head==>直接删除
                auto prev=head->next_;
                while(prev->next_ != cur){
                   prev=prev->next_;
                   if(prev == nullptr){
                       throw STD_ overflow_error("element not exist!!");
                   }
                }
                prev->next_=cur->next_;
                destroy_node(cur);
                --num_elements_;
                return make_iterator(prev->next_);
            }
        }
        return make_iterator(cur);
    } 

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    void
    HashTable<V,K,H,GK,EK,A>::clear(){
        for(size_type cur=0;cur!=table_size();++cur){
            auto first=table_[cur];
            node* next=nullptr;
            while(first!=nullptr){
                next=first->next_;
                destroy_node(first);
                first=next;
            }
            table_[cur]=nullptr;
        }
        num_elements_=0;
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    typename HashTable<V,K,H,GK,EK,A>::node*
    HashTable<V,K,H,GK,EK,A>::begin_aux() const{
        for(auto& list : table_)
            if(list !=nullptr)
                return list;
        return nullptr;
    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    template<typename ...Args>
    typename HashTable<V,K,H,GK,EK,A>::node*
    HashTable<V,K,H,GK,EK,A>::new_node(Args&&... val){
        node* node;
        TRY_BEGIN
        node=node_allocator::allocate();
        construct(&node->val_,STL_ forward<Args>(val)...);
        return node;
        TRY_END

        CATCH_ALL_BEGIN
        destroy(node);
        node_allocator::deallocate(node);
        throw;
        CATCH_END

    }

    template<typename V,typename K,typename H,typename GK,typename EK,typename A>
    void
    HashTable<V,K,H,GK,EK,A>::destroy_node(node* _node){
        node_allocator::destroy(_node);
        node_allocator::deallocate(_node);
    }
}
#endif