#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

#define TEMPLATE_HASHTABLE(Type) \
template<typename V,typename K,typename H,typename GK,typename EK,typename Alloc> \
Type HashTable<V,K,H,GK,EK,Alloc>::\

#define HASH_ITERATOR \
typename HashTable<V,K,H,GK,EK,Alloc>::iterator \

#define HASH_CONST_ITERATOR \
typename HashTable<V,K,H,GK,EK,Alloc>::const_iterator \

#define HASHTABLE HashTable<V,K,H,GK,EK,A>

#include <stl_exception.h>
#include <config.h>
#include <allocator.h>
#include <user_allocator.h>
#include <vector.h>
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
            : val_{val},next_{next}{}

        explicit HashNode(T&& val=T{},HashNode* next=nullptr)
            : val_{STL_MOVE(val)},next_{next}{}
    };

    template<typename V,typename K,typename H,typename GK,typename EK,typename Alloc>
    class HashConstIterator{
    public:
        using value_type                =V;
        using reference                 =V const&;
        using pointer                   =V const*;
        using difference_type           =STD_ ptrdiff_t;
        using size_type                 =STD_ size_t;

        using iterator                  =HashConstIterator<V,K,H,GK,EK,Alloc>;
        using const_iterator            =HashConstIterator<V,K,H,GK,EK,Alloc>;
        using self                      =HashConstIterator;

        using node              =HashNode<V>;
        using hash_table        =HashTable<V,K,H,GK,EK,Alloc>;

        HashConstIterator(node* cur,hash_table const& ht) : cur_{cur},ht_{ht}{}

        reference operator*() const { return cur_->val_; }
        pointer   operator->() const {return cur_; }
        self& operator++() {
            node const* old_node=cur_;
            cur_=cur_->next_;
            if(!cur_){
                size_t now_num=ht_.hash_val(old_node->val_);
                while(!cur_ && ++now_num < ht_.table_size())
                    cur_=ht_.table_[now_num];
            }
            return *this;
        }

        self operator++(int) {
            auto tmp=*this;
            ++*this;
            return tmp;
        }

        friend bool operator==(HashConstIterator const& lhs,HashConstIterator const& rhs){
            return (lhs.cur_ == rhs.cur_) ;//&& (lhs.ht_ == rhs.ht_);
        }

        friend bool operator!=(HashConstIterator const& lhs,HashConstIterator const& rhs){
            return !(lhs == rhs);
        }

        friend bool operator==(HashConstIterator const& lhs,STD_ nullptr_t ){
            return lhs.cur_ == nullptr;
        }

        friend bool operator!=(HashConstIterator const& lhs,STD_ nullptr_t){
            return lhs.cur_ !=nullptr;
        }
    protected:
        node* cur_;
        hash_table const& ht_;

        friend class HashTable<V,K,H,GK,EK,Alloc>;
    };

    template<typename V,typename K,typename H,typename GK,typename EK,typename Alloc>
    class HashIterator : public HashConstIterator<V,K,H,GK,EK,Alloc>{
    public:
        using iterator          =HashIterator<V,K,H,GK,EK,Alloc>;
        using const_iterator    =HashIterator<V,K,H,GK,EK,Alloc>;
        using self              =HashIterator;

        using reference         =V&;
        using pointer           =V*;

        using base              =HashConstIterator<V,K,H,GK,EK,Alloc>;
        using node              =typename base::node;
        using hash_table        =typename base::hash_table;

        HashIterator(node* cur,hash_table const& ht)
            : base::HashConstIterator(cur,ht){}

        friend class HashTable<V,K,H,GK,EK,Alloc>;
    };

    //V=value,K=key,H=hash function,GK=get_key,EK=equal_key
    template<typename V,typename K,typename H,typename GK,typename EK,
            typename Alloc>
    class HashTable{
    public:
        //type alias
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
        using const_iterator            =typename HashConstIterator<V,K,H,GK,EK,Alloc>::const_iterator;
        
        //contruct/copy/deconsturct
        HashTable()=default;
        ~HashTable(){ clear(); }
        HashTable(HashTable const&);
        HashTable(HashTable &&);
        HashTable& operator=(HashTable const&);
        HashTable& operator=(HashTable&&);

    
        explicit HashTable(size_type const n) : num_elements_{0}{ init_table(n); }

        //modifiers
        pair<iterator,bool> insert_unique(value_type const& val);
        pair<iterator,bool> insert_unique(value_type && val);
        iterator insert_unique(const_iterator hint,value_type const& value);
        iterator insert_unique(const_iterator hint,value_type && value);
        template<typename II>
        void insert_unique(II first,II last);
        void insert_unique(STD_ initializer_list<value_type> list);
        
        iterator insert_equal(value_type const& val);
        iterator insert_equal(value_type &&val);
        iterator insert_equal(const_iterator hint,value_type const& value);
        iterator insert_equal(const_iterator hint,value_type && value);
        template<typename II>
        void insert_equal(II first,II last);
        void insert_equal(STD_ initializer_list<value_type> list);

        iterator erase(const_iterator pos);
        size_type erase(key_type const& key);
        iterator erase(const_iterator first,const_iterator last);        
        void clear();

        //position interface
        iterator begin() noexcept 
        { return iterator(begin_aux(),*this); }

        iterator end() noexcept 
        { return iterator(nullptr,*this); }

        const_iterator begin() const noexcept
        { return const_iterator(begin_aux(),*this); }

        const_iterator end() const noexcept
        { return const_iterator(nullptr,*this); }
        
        const_iterator cbegin() const noexcept
        { return const_iterator(begin_aux(),*this); }

        const_iterator cend() const noexcept
        { return const_iterator(nullptr,*this); }

        //attributes interface
        //Nonmodifying operation
        size_type size() const { return num_elements_; }
        bool empty() const noexcept { return num_elements_; }
        size_type max_size() const noexcept { return PRIME_LIST[PRIMES_NUM-1]; }
        friend bool operator==(HashTable const& lhs,HashTable const& rhs) noexcept;
        friend bool operator!=(HashTable const& lhs,HashTable const& rhs) noexcept;

        //table interface
        size_type table_size() const { return table_.size(); }
        size_type table_num(key_type const& key) const;
        size_type table_count(size_type table_num) const;

        //special search operation
        size_type count(key_type const& key);
        iterator find(key_type const& key);
        pair<iterator,iterator> equal_range(key_type const& key);
        pair<const_iterator,const_iterator> equal_range(key_type const& key) const;

        
        //rehash
        void rehash(size_type element_num_hint);
        float load_factor() const noexcept { return static_cast<float>(num_elements_)/table_size(); }

        //allocator
        allocator_type&& get_allocator() const { return allocator_type(); }

    private:
        using node                      =HashNode<V>;
        using node_allocator            =UserAllocator<node,Alloc>;
    
        void init_table(size_type const n);
        size_type next_size(size_type n) const { return next_prime(n); }

        template<typename Args> pair<iterator,bool> insert_unique_norehash(Args&& val);
        template<typename Args> iterator insert_unique_norehash(const_iterator hint,Args&& val);

        template<typename Args> iterator insert_equal_norehash(Args&& val);
        template<typename Args> iterator insert_equal_norehash(const_iterator hint,Args&& val);
        
        node* begin_aux() const;

        //hash handle
        size_type hash_key(key_type const& key,size_type table_size) const
        { return hash_fun_(key)%table_size; }

        size_type hash_key(key_type const& key) const
        { return hash_key(key,table_size()); }

        size_type hash_val(value_type const& val,size_type table_size) const
        { return hash_key(get_key_(val),table_size); }

        size_type hash_val(value_type const& val) const
        { return hash_key(get_key_(val),table_size()); }

        //linked list helper
        template<typename ...Args>
        node* new_node(Args&&... val);
        void destroy_node(node* _node);

        //iterator construct helper
        iterator make_iterator(node* _node) { return iterator(_node,*this); }
        const_iterator make_const_iterator(node* _node) const { return const_iterator(_node,*this); }

        //friend declaration
        friend class HashIterator<V,K,H,GK,EK,Alloc>;
        friend class HashConstIterator<V,K,H,GK,EK,Alloc>;
    protected:
        GK get_key_;
        EK equal_key_;
        H hash_fun_;
        size_type num_elements_; 
        vector<node*> table_;
    };

    
}

#endif