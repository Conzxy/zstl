#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#define TEMPLATE_HASHTABLE(Type) \
template<typename V,typename K,typename H,typename GK,typename EK,typename Alloc> \
Type HashTable<V,K,H,GK,EK,Alloc>::\

#define HASH_ITERATOR \
typename HashTable<V,K,H,GK,EK,Alloc>::iterator \

#define HASH_CONST_ITERATOR \
typename HashTable<V,K,H,GK,EK,Alloc>::const_iterator \

#include <stl_exception.h>
#include <config.h>
#include <Allocator.h>
#include <user_allocator.h>
#include <vector>
#include <hash_fun.h>

namespace TinySTL{
    //fwd 
    //以便iterator关联hash table
    template<typename V,typename K,typename H,typename GK,typename EK,
        typename Alloc=STL_ alloc>
    class HashTable;

    template<typename T>
    struct HashNode{
        T val_;
        struct HashNode* next_;

        explicit HashNode(const T& val,HashNode* next=nullptr)
            : val_{val},next_{next_}{}

        explicit HashNode(T&& val=T{},HashNode* next=nullptr)
            : val{STL_MOVE(val)},next_{next_}{}
    };

    template<typename V,typename K,typename H,typename GK,typename EK,typename Alloc>
    class HashIterator  : public iterator<forward_iterator_tag,V>
    protected:
        node* cur_;
        hash_table&  ht_;
    public:
        using iterator          =HashIterator<V,K,H,GK,EK>;
        using const_iterator    =HashIterator<V const,K,H,GK,EK>
        using self              =HashIterator;
        using hash_table        =HashTable<V,K,H,GK,EK,Alloc>;

        using node              =HashNode<V>;
        using base              =iterator<forward_iterator_tag,V>;

        HashIterator(node* cur,hash_table const& ht) : cur_{cur},ht_{ht}{}
        
        typename base::reference operator*() const { return cur_->val_; }        
        typename base::pointer   operator->() const {return cur_; }
        self& operator++() {
            node const* old_node=cur_;
            cur_=cur_->next;
            if(!cur){
                size_type now_num=ht_.get_mapped(old_node->val);
                while(!cur_ && ++now_num < ht_->tableSize())
                    cur_=ht_.table_[now_num];
            }
        }
 
        self operator++(int) {
            auto tmp=*this;
            ++*this;
            return tmp;
        }
            
        operator const_iterator(){
            return const_iterator(cur_,ht_);
        } 

        friend bool operator==(HashIterator const& lhs,HashIterator const& rhs){
            return (lhs.cur_ == rhs.cur_) && (lhs.ht_ == rhs.ht_);
        }

        friend bool operator!=(HashIterator const& lhs,HashIterator const& rhs){
            return !(lhs == rhs);
        }
    };

    //V=value,K=key,H=hash function,GK=get_key,EK=equal_key
    template<typename V,typename K,typename H,typename GK,typename EK
            typename Alloc>
    class HashTable{
    public:
        using value_type                =V;
        using reference                 =V&;
        using const_reference           =const V&;
        using pointer                   =V*;
        using const_pointer             =V const*;
        using size_type                 =STD_ size_t;
        using difference_type           =STD_ ptrdiff_t;
        using allocator_type            =Alloc;
        using key_type                  =K;
        using hash_fun_type             =H;
        using get_key_type              =GK;
        using equal_key_type            =EK;
        using iterator                  =typename HashIterator<V,K,H,GK,EK,Alloc>::iterator;
        using const_iterator            =typename HashIterator<V,K,H,GK,EK,Alloc>::const_iteator;

        HashTable()=default;
        
        insert_unique(const value_type& val);
        allocator_type&& get_allocator() const { return allocator_type(); }
    private:
        using node                      =HashNode<V>;
        using node_allocator            =UserAllocator<node,Alloc>;
    
        void init_table(size_type const n);
        size_type next_size(size_type n){ return next_prime(n); }
        size_type table_size(){ return table_.size(); }
        void rehash(size_type element_num_hint);
        pair<iterator,bool> insert_unique_norehash(const value_type& val);

        //hash handle
        size_type hash_key(key_type const& key,size_type table_size) const{
            return hash_fun_(key)%table_size;
        }

        size_type hash_key(key_type const& key) const{
            return hash_val(key,tabel_size());
        }

        size_type hash_val(value_type const& val,size_type table_size) const{
            return hash_key(get_key(val),table_size);
        }

        size_type hash_val(value_type const& val) const{
            return hash_key(get_key(val,table_size());
        }

        node* new_node(value_type const& val){
            TRY_BEGIN
            node* node=node_allocator::allocate();
            construct(&node->val_,val);
            return node;
            TRY_END

            CATCH_ALL_BEGIN
            node_allocator::deallocate(node);
            throw;
            CATCH_ALL_END
        }

        void destroy_node(node* _node){
            destory(&_node->val_);
            node_allocator::deallocate(_node);
        }

    protected:
        GK get_key_;
        EK equal_key_;
        H hash_fun_;
        size_type num_elements_; 
        vector<node*> table_;
    };

    
    TEMPLATE_HASHTABLE(void) init_table(const size_type n){
        const size_type table_size=next_size(n);
        table_.reserve(table_size);
        for(size_type i=0;i != table_size ;++i){
            table_.emplace_back(nullptr);
        } 
    }

    TEMPLATE_HASHTABLE(pair<HASH_ITERATOR,bool>) insert_unique(const value_type& val){
        rehash(num_elements_+1);
        return insert_unique_norehash(val);
    }

    TEMPLATE_HASHTABLE(void) rehash(size_type num_element_hint){
        //如果给定元素大小比slots大，意味着load_factor>1
        //而分离链表法无论插入是否成功，其时间复杂度为O(1+α)
        //因此这个条件是为了让时间复杂度保持在常量时间
        if(num_element_hint > table_size()){
            auto old_table=table_;
            
            auto new_table_size=next_size(num_element_hint);
            auto new_table(new_table_size,);

            TRY_BEGIN
            for(size_type index=0;i != table_size();++i){
                auto first=table_.at(index);
                while(first){
                    table_[index]=first->next_;
                    size_type new_index=hash_val(first->val_,new_table_size);
                    first->next_=new_table[new_index];
                    new_table[new_index]->next_=first;
                    first=table_[index];
                }
            }
            TRY_END

            CATCH_ALL_BEGIN
            vector<node*> null_vector;
            new_table.swap(null_vector);
            CATCH_ALL_END

            STL_SWAP(new_table,table_);
        }

        TEMPLATE_HASHTABLE(pair<HASH_ITERATOR,bool>) insert_unique_norehash(const value_type& value){
            const mapped_index=hash_val(value);
            node* first=table_.at(mapped_index);

            for(auto cur=first;cur != nullptr ;++cur){
                if(equal_key(get_key(value),get_key(cur->val_)))
                //如果key已存在在hashtable中，则返回该key的位置并设boolean为false表示插入失败
                    return pair<iterator,bool>(iterator(cur,false));
            }

            //insert node to hashtable
            auto new_node=new_node(value);
            new_node->next_=first;
            table_[mapped_index]=new_node;
            ++num_elements_;
            return pair<iterator,bool>(iterator(cur,true));
        }
    }
}

#endif