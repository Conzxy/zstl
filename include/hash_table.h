#ifndef TINYSTL_HASH_TABLE_H
#define TINYSTL_HASH_TABLE_H

#include "stl_exception.h"
#include "config.h"
#include "allocator.h"
#include "user_allocator.h"
#include "vector.h"
#include "hash_aux.h"

#ifdef HASH_DEBUG
#include <iostream>
#endif 

namespace TinySTL {

/**
 * @class HashTable
 * @tparam V value type
 * @tparam K key type
 * @tparam H hash function which convertion key to natural number
 * @tparam GK method which get key from value
 * @tparam EK method which compares two key whether them is equivalent
 * @tparam Alloc Allocator type(default is TinySTL::allocator)
 * @brief 
 * Implementation of hash table.
 * Its average time complexity of search is O(1)
 * @see <<Introdunction To Algorithms>> 11.2
 */
template<
typename V,typename K,typename H,typename GK,typename EK,
typename Alloc=TinySTL::allocator<V>>
class HashTable;

/**
 * @struct HashNode
 * @tparam T value type of hashtable
 * @brief node of linked list which used in chaining hashtable
 */
template<typename T>
struct HashNode {
    T val;
    struct HashNode* next;

    explicit HashNode(T const& v, struct HashNode* n = nullptr)
        : val{ v }
        , next{ n }
    { }

    explicit HashNode(T&& v, struct HashNode* n = nullptr)
        : val{ STL_MOVE(v) }
        , next{ n }
    { }  
};

/**
 * @class HashConstIterator
 * @tparam V value type
 * @tparam K key type
 * @tparam H hash function which convertion key to natural number
 * @tparam GK method which get key from value
 * @tparam EK method which compares two key whether them is equivalent
 * @tparam Alloc Allocator type(default is TinySTL::allocator)
 * @brief
 * Used for iterating hashtable
 */
template<
typename V, typename K, typename H, typename GK, typename EK,
typename Alloc>
class HashConstIterator{
public:
    using value_type      = V;
    using reference       = V const &;
    using pointer         = V const *;
    using difference_type = std::ptrdiff_t;
    using size_type       = std::size_t;
    using iterator        = HashConstIterator<V, K, H, GK, EK, Alloc>;
    using const_iterator  = HashConstIterator<V, K, H, GK, EK, Alloc>;
    using self            = HashConstIterator;
    using node            = HashNode<V>;
    using hash_table      = HashTable<V, K, H, GK, EK, Alloc>;

    HashConstIterator(node *cur, hash_table const &ht)
        : cur_{ cur }
        , ht_{ ht }
    { }

    reference operator*() const TINYSTL_NOEXCEPT 
    { return cur_->val; }
    pointer   operator->() const TINYSTL_NOEXCEPT
    { return cur_; }

    self& operator++() {
        const auto old_node = cur_;
        cur_ = cur_->next;

        if (!cur_) {
            auto hash_val = ht_.hashVal(old_node->val) + 1;

            for (; hash_val < ht_.tableSize(); ++hash_val) {
                cur_ = ht_.table()[hash_val]->next;
                if (cur_)
                    break;
            }
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

protected:
    node* cur_;
    hash_table const& ht_;

    friend class HashTable<V,K,H,GK,EK,Alloc>;
};

/**
 * @class HashIterator
 * @inheritby HashConstIterator
 * @tparam V value type
 * @tparam K key type
 * @tparam H hash function which convertion key to natural number
 * @tparam GK method which get key from value
 * @tparam EK method which compares two key whether them is equivalent
 * @tparam Alloc Allocator type(default is TinySTL::allocator)
 */
template <
typename V, typename K, typename H, typename GK, typename EK, 
typename Alloc>
class HashIterator : public HashConstIterator<V, K, H, GK, EK, Alloc>
{
public:
    using iterator          = HashIterator<V,K,H,GK,EK,Alloc>;
    using const_iterator    = HashIterator<V,K,H,GK,EK,Alloc>;
    using self              = HashIterator;

    using reference         = V&;
    using pointer           = V*;

    using base              = HashConstIterator<V,K,H,GK,EK,Alloc>;
    using node              = typename base::node;
    using hash_table        = typename base::hash_table;

    HashIterator(node *cur, hash_table const &ht)
        : base::HashConstIterator(cur, ht)
    { }

    reference operator*() const TINYSTL_NOEXCEPT 
    { return cur_->val; }
    pointer   operator->() const TINYSTL_NOEXCEPT
    { return cur_; }

    self& operator++() {
        const auto old_node = cur_;
        cur_ = cur_->next;

        if (!cur_) {
            auto hash_val = ht_.hashVal(old_node->val) + 1;

            for (; hash_val < ht_.tableSize(); ++hash_val) {
                cur_ = ht_.table()[hash_val]->next;
                if (cur_)
                    break;
            }
        }
        return *this;
    }

    self operator++(int) {
        auto tmp=*this;
        ++*this;
        return tmp;
    }

    friend class HashTable<V,K,H,GK,EK,Alloc>;

    using base::cur_;
    using base::ht_;
};

template<
typename V,typename K,typename H,typename GK,typename EK,
typename Alloc>
class HashTable {
    using Node                     = HashNode<V>;
    using NodeAllocator            = typename Alloc::template rebind<Node>;
    using NodeAllocTraits          = allocator_traits<NodeAllocator>;
    using HashMethod               = uint64_t(*)(uint64_t, uint64_t);
    using Table                    = Vector<Node*>;
public:
    //type alias
    using value_type      = V;
    using reference       = V&;
    using const_reference = const V&;
    using pointer         = V*;
    using const_pointer   = V const*;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type  = Alloc;
    using key_type        = K;
    using hash_fun_type   = H;
    using get_key_type    = GK;
    using equal_key_type  = EK;
    using iterator        = typename HashIterator<V, K, H, GK, EK, Alloc>::iterator;
    using const_iterator  = typename HashConstIterator<V, K, H, GK, EK, Alloc>::const_iterator;
    using Self            = HashTable;

    //contruct/copy/deconsturct
    HashTable() 
        : impl_{ 0 }
    { }

    explicit HashTable(size_type const n)
        : impl_{ n }
    { initTable(n); }

    ~HashTable() { clear(); }
    // HashTable(HashTable const &);
    // HashTable(HashTable &&);
    // HashTable &operator=(HashTable const &);
    // HashTable &operator=(HashTable &&);

    //modifiers
    template<typename... Args>
    TinySTL::pair<iterator, bool> insertUnique(Args&&... args);

    void clear();

    // position interface
    iterator begin() TINYSTL_NOEXCEPT
    { return makeIter(getFirstList()); }

    iterator end() TINYSTL_NOEXCEPT
    { return makeIter(nullptr); }

    const_iterator begin() const TINYSTL_NOEXCEPT
    { return makeConstIter(getFirstList()); }

    const_iterator end() const TINYSTL_NOEXCEPT
    { return makeConstIter(nullptr); }

    const_iterator cbegin() const TINYSTL_NOEXCEPT
    { return makeConstIter(getFirstList()); }

    const_iterator cend() const TINYSTL_NOEXCEPT
    { return makeConstIter(nullptr); }

    // reverse_iterator is wrapped by adapter


    // field information:
    size_type size() const TINYSTL_NOEXCEPT
    { return impl_.numElements; }

    bool empty() const TINYSTL_NOEXCEPT 
    { return size(); }

    size_type max_size() const TINYSTL_NOEXCEPT
    { return PRIME_LIST[PRIMES_NUM-1]; }

    Table const& table() const TINYSTL_NOEXCEPT
    { return impl_.table; }    

    size_type tableSize() const TINYSTL_NOEXCEPT
    { return impl_.table.size(); }

    EK equalKey() const TINYSTL_NOEXCEPT
    { return impl_.equalKey; }

    GK getKey() const TINYSTL_NOEXCEPT
    { return impl_.getKey; }

    H hash() const TINYSTL_NOEXCEPT
    { return impl_.hashFun; }

    HashMethod hashMethod() const TINYSTL_NOEXCEPT
    { return impl_.hashMethod; }

    size_type table_num(key_type const& key) const;
    size_type table_count(size_type table_num) const;

    // special search operation
    iterator find(key_type const& key);
    // size_type count(key_type const& key);
    // pair<iterator,iterator> equal_range(key_type const& key);
    // pair<const_iterator,const_iterator> equal_range(key_type const& key) const;

    
    // rehash
    void rehash(size_type hint);

    double load_factor() const TINYSTL_NOEXCEPT
    { return static_cast<double>(size()) / tableSize(); }

    // allocator
    allocator_type&
    get_allocator() const TINYSTL_NOEXCEPT 
    { return impl_; }

    TINYSTL_CONSTEXPR size_type 
    hashKey(key_type const& key) const TINYSTL_NOEXCEPT;

    TINYSTL_CONSTEXPR size_type 
    hashVal(value_type const& val) const TINYSTL_NOEXCEPT;

    // debug helper
    #ifdef HASH_DEBUG
    void printTableLayout() const;
    #endif

    friend bool operator==(HashTable const& lhs,HashTable const& rhs) noexcept;
    friend bool operator!=(HashTable const& lhs,HashTable const& rhs) noexcept;
private:
    NodeAllocator&
    getNodeAllocator() 
    { return impl_; }

    void initTable(size_type const n);
    Node* getFirstList() const;

    Table& table() TINYSTL_NOEXCEPT
    { return impl_.table; }

    void reclaimSentinel(Table& table);

    void incElemensNum(size_type n) TINYSTL_NOEXCEPT
    { impl_.numElements += n; }

    void decElementNums(size_type n) TINYSTL_NOEXCEPT
    { impl_.numElement -= n; }

    template<typename Args> pair<iterator,bool> insert_unique_norehash(Args&& val);
    template<typename Args> iterator insert_unique_norehash(const_iterator hint,Args&& val);

    template<typename Args> iterator insert_equal_norehash(Args&& val);
    template<typename Args> iterator insert_equal_norehash(const_iterator hint,Args&& val);
    
    // hash function forward

    //linked list helper
    template<typename ...Args> 
    Node* newNode(Args&&... val);
    void destroyNode(Node* node);

    //iterator construct helper
    iterator 
    makeIter(Node* node) const TINYSTL_NOEXCEPT
    { return iterator(node,*this); }


    const_iterator 
    makeConstIter(Node* node) const TINYSTL_NOEXCEPT
    { return const_iterator(node,*this); }

    //friend declaration
    friend class HashIterator<V,K,H,GK,EK,Alloc>;
    friend class HashConstIterator<V,K,H,GK,EK,Alloc>;

private:
    struct Impl 
    : NodeAllocator
    , Alloc {
        explicit Impl(
            size_type n, 
            H hashFun_ = H{},
            HashMethod hashMethod_ = &hashDivision)
            : hashFun{ hashFun_ }
            , hashMethod{ hashMethod_ }
            , numElements{ 0 }
            , table{ n, nullptr }
        { }

        GK getKey;
        EK equalKey;
        H hashFun;
        HashMethod hashMethod;
        size_type numElements;
        Vector<Node*> table;
    };

    Impl impl_;
};

#define TEMPLATE_OF_HASHTABLE \
    template<typename V, typename K, typename H, typename GK, typename EK, typename Alloc> \

#define HASHTABLE \
    HashTable<V, K, H, GK, EK, Alloc>

TEMPLATE_OF_HASHTABLE
TINYSTL_CONSTEXPR auto
HASHTABLE::hashKey(key_type const& key) const TINYSTL_NOEXCEPT
-> size_type {
    return hashMethod()(hash()(key), tableSize());
}

TEMPLATE_OF_HASHTABLE
TINYSTL_CONSTEXPR auto
HASHTABLE::hashVal(value_type const& val) const TINYSTL_NOEXCEPT
-> size_type {
    return hashKey(getKey()(val));
}

TEMPLATE_OF_HASHTABLE
template<typename ...Args>
auto
HASHTABLE::insertUnique(Args&& ...args) 
-> TinySTL::pair<iterator, bool> {
    rehash(size() + 1);

    const auto node = newNode(STL_FORWARD(Args, args)...);
    auto hashcode = hashVal(node->val);
    assert(hashcode < tableSize() && hashcode >= 0);

    auto head = table()[hashcode];
    // check if there are value with same key in linked list
    for (auto h = head->next; h != nullptr; h = h->next) {
        if (equalKey()(getKey()(h->val), node->val)) {
            // exist same key
            // destory newly constructed node
            // and return status code(denoetd in pair::second)
            destroyNode(node);
            return TinySTL::make_pair(makeIter(h), false);
        }
    }

    // insert the newly node to linked list
    // ensure it is unique key in this linked list
    node->next = head->next;
    head->next = node;
    incElemensNum(1);

    return TinySTL::make_pair(makeIter(node), true);
}

TEMPLATE_OF_HASHTABLE
auto
HASHTABLE::find(key_type const& key) 
-> iterator {
    const auto hashcode = hashKey(key);
    assert(hashcode >= 0 && hashcode < tableSize());

    auto head = table()[hashcode];

    for (; head->next != nullptr; head = head->next) {
        if (equalKey()(getKey()(head->val), key)) {
            return makeIter(head);
        }
    }

    return end();
}

TEMPLATE_OF_HASHTABLE
inline void 
HASHTABLE::rehash(size_type hint) {
    // If load_factor > 1.0, 
    // we expand the slots num
    if (hint > tableSize()) {
        const auto oldSize = tableSize();
        const auto nextSize = nextPrime(hint);

        Table oldTable;
        oldTable.swap(table());

        table().resize(nextSize);
        initTable(tableSize());
        // Since the tableSize() has changed, 
        // we should reset the linked list to proper location.
        // It is difficult to set in previous table directly,
        // we use a new table then swap them to complete.
        for (auto head : oldTable) {
            auto real = head->next;
            for (; real; real = head->next) {
                auto newHashcode = hashVal(real->val);

                auto newHead = table()[newHashcode];

                // remove the node from the old linked list
                head->next = real->next;

                // insert the old node to new linked list
                real->next = newHead->next;
                newHead->next = real;
            }
        }

        reclaimSentinel(oldTable);
    }
}

TEMPLATE_OF_HASHTABLE
inline auto
HASHTABLE::getFirstList() const
-> Node* {
    for (auto& head : table()) {
        if (head->next) {
            return head->next;
        }
    }

    return nullptr;
}

TEMPLATE_OF_HASHTABLE
inline void
HASHTABLE::initTable(size_type n) {
    for (auto& head : table()) {
        // create sentinel(dummy node):
        // To make implemetation of single linked list operation easier
        head = newNode();
    }
}

TEMPLATE_OF_HASHTABLE
inline void
HASHTABLE::reclaimSentinel(Table& table) {
    for (auto head : table) {
        assert(head->next == nullptr);
        destroyNode(head);
    }
}

TEMPLATE_OF_HASHTABLE
void
HASHTABLE::clear() {
    for (auto& head : table()) {
        while (head->next) {
            auto tmp = head->next;
            head->next = tmp->next;

            destroyNode(tmp);
        }                
    }
    
    reclaimSentinel(table());
}

TEMPLATE_OF_HASHTABLE
template<typename ...Args>
inline auto
HASHTABLE::newNode(Args&&... args)
-> Node* {
    Node* node = NodeAllocTraits::allocate(
        getNodeAllocator(), sizeof(Node));
    TRY_BEGIN
        NodeAllocTraits::construct(
            getNodeAllocator(),
            node,
            value_type(STL_FORWARD(Args, args)...));
    TRY_END
    CATCH_ALL_BEGIN
        NodeAllocTraits::deallocate(
            getNodeAllocator(), node);
        RETHROW
    CATCH_END

    return node;
}

TEMPLATE_OF_HASHTABLE
inline void
HASHTABLE::destroyNode(Node* node) {
    NodeAllocTraits::destroy(getNodeAllocator(), node);
    NodeAllocTraits::deallocate(getNodeAllocator(), node);
}

#ifdef HASH_DEBUG
TEMPLATE_OF_HASHTABLE
void
HASHTABLE::printTableLayout() const {
    for (int i = 0; i != tableSize(); ++i) {
        printf("[%d]: ", i);
        for (auto head = table()[i]->next;
             head != nullptr;
             head = head->next) {
            std::cout << "(" << head->val << ")";
            if (head->next != nullptr)
                std::cout << " -> ";
        }
        puts("");
    }
}
#endif

} // namespace TinySTL

#endif