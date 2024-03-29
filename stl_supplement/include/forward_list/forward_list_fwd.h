#pragma once
#include <forward_list>
#ifndef STL_SUP_FORWARD_LIST_FWD_H
#define STL_SUP_FORWARD_LIST_FWD_H

#include <memory>
#include <assert.h>


#include "../iterator.h"
#include "../type_traits.h"

#include "node_def.h"
#include "forward_list_iterator.h"
#include "algorithm.h"

namespace zstl { 
  
namespace forward_list_detail {

template<typename Alloc>
using HeaderAllocator = typename Alloc::template rebind<Header>::other;

template<typename T, typename Alloc>
using NodeAllocator = typename Alloc::template rebind<LinkedNode<T>>::other;

/**
 * EBCO
 * Need two allocator: header and node
 * This class also manage the header allocate and deallocate
 * but don't construct and destroy it, it just a POD object.
 */
template<typename T, typename Alloc=std::allocator<T>>
class ForwardListBase 
  : protected HeaderAllocator<Alloc>, protected NodeAllocator<T, Alloc> {
protected:
  using AllocTraits = std::allocator_traits<HeaderAllocator<Alloc>>;

public:
  ForwardListBase()
  try: header_(AllocTraits::allocate(*this, sizeof(Header)))
  {
    reset();
  }
  catch (std::bad_alloc const& ex) { 
    throw;
  }

  ForwardListBase(ForwardListBase&& other) noexcept
   : header_(other.header_)
  {
    other.header_ = nullptr;
  }

  ~ForwardListBase() noexcept 
  {
    // AllocTraits::destroy(this, header_);
    AllocTraits::deallocate(*this, header_, sizeof(Header));
  }

  void reset() noexcept
  {
    header_->prev = header_;
    header_->next = nullptr;
    header_->count = 0;
  }
protected:
  Header* header_;
};

} // namespace forward_list_detail


/**
 * ForwardList represents a single linked-list
 * The Implementation of STL is so trivial, such that I rewrite, and provide some useage
 * 1) The interface extract node in list but don't remove it from memory, so we can reuse node
 * 2) Support push_back() which time complexity is O(1), then we can implementation queue, 
 * such as std::queue<ForwardList>(I can't sure it is good, my zstl::queue<Container> support at least)
 */
template<typename T, typename Alloc=std::allocator<T>>
class ForwardList : protected forward_list_detail::ForwardListBase<T, Alloc> {
  using Self = ForwardList;
  using Base = forward_list_detail::ForwardListBase<T, Alloc>;
public: 
  // STL compatible
  using value_type      = T;
  using allocator_type  = Alloc;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference       = value_type&;
  using const_reference = value_type const&;
  using pointer         = typename std::allocator_traits<Alloc>::pointer;
  using const_pointer   = typename std::allocator_traits<Alloc>::const_pointer;
  using iterator        = ForwardListIterator<value_type>;
  using const_iterator  = ForwardListConstIterator<value_type>;

  using ValueType     = T;
  using SizeType      = std::size_t;
  using Ref           = T&;
  using ConstRef      = T const&;
  using Pointer       = typename std::allocator_traits<Alloc>::pointer;
  using ConstPointer  = typename std::allocator_traits<Alloc>::const_pointer;
  using Iterator      = ForwardListIterator<T>;
  using ConstIterator = ForwardListConstIterator<T>;
  using BaseNode      = typename Iterator::BaseNode;
  using Node          = typename Iterator::Node;
  using ConstNode     = typename ConstIterator::Node;
  using NodeAllocator = typename Alloc::template rebind<Node>::other;
  using AllocTraits   = std::allocator_traits<NodeAllocator>;

  ForwardList() = default;

  explicit ForwardList(SizeType n, ValueType const& val={})
  {
    insert_after(cbefore_begin(), n, val);
  }

  template<typename InputIterator, 
    typename = zstl::enable_if_t<zstl::is_input_iterator<InputIterator>::value>>
  ForwardList(InputIterator first, InputIterator last)
  {
    insert_after(cbefore_begin(), first, last);
  }

  template<typename E>
  ForwardList(std::initializer_list<E> il)
   : ForwardList(il.begin(), il.end())
  { }
  
  /**
   * Special member function
   */
  ~ForwardList() noexcept { if (header_) clear(); }

  ForwardList(ForwardList const& other)
   : Base()
  {
    insert_after(cbefore_begin(), other.cbegin(), other.cend());
  }

  ForwardList(ForwardList&& other) noexcept
   : Base(other)
  {
  }

  Self& operator=(Self const& other);
  
  Self& operator=(Self&& other) noexcept
  {
    this->swap(other);
    return *this;
  }
  
  allocator_type get_allocator() const noexcept { return allocator_type{}; }

  void assign(SizeType count, ValueType const& value);
  template<typename InputIterator,
    typename = zstl::enable_if_t<zstl::is_input_iterator<InputIterator>::value>>
  void assign(InputIterator first, InputIterator last);
  template<typename U>
  void assign(std::initializer_list<U> il) { assign(il.begin(), il.end()); }
  void resize(SizeType n);
  void resize(SizeType n, ValueType const& val);

  // Insert after header
  template<typename ...Args>
  void emplace_front(Args&&... args) { push_front(create_node(STD_FORWARD(Args, args)...)); }
  // It is may be better for builtin type that use value-passed parameter.
  // But we can not suppose the move operation for ValueType is cheap
  // Thus, I still write const&/&& or better.
  void push_front(ValueType const& val) { push_front(create_node(val)); }
  void push_front(ValueType&& val) { push_front(create_node(std::move(val))); }

  // Insert before header
  // Not standard required
  template<typename ...Args>
  void emplace_back(Args&&... args) { push_back(create_node(STD_FORWARD(Args, args)...)); }
  void push_back(ValueType const& val) { push_back(create_node(val)); }
  void push_back(ValueType&& val) { push_back(create_node(std::move(val))); }

  // Insert after given position(presented by iterator)
  template<typename ...Args> 
  void emplace_after(ConstIterator pos, Args&&... args) 
  { insert_after(pos, create_node(STD_FORWARD(Args, args)...)); }
  void insert_after(ConstIterator pos, ValueType const& val) 
  { insert_after(pos, create_node(val)); }
  void insert_after(ConstIterator pos, ValueType&& val) 
  { insert_after(pos, create_node(std::move(val))); }

  // Based on Node, thus we can reuse extracted node(throught call extract_xxx())
  // Maybe this is the implementation function of std::forward_list<>
  // But I expose these.
  // Not standard required
  inline void push_front(Node* new_node) noexcept;
  inline void push_back(Node* new_node) noexcept;
  inline void insert_after(ConstIterator pos, Node* new_node);

  // Range insert
  void insert_after(ConstIterator pos, SizeType count, ValueType const& val);
  template<typename InputIterator, typename = 
    zstl::enable_if_t<zstl::is_input_iterator<InputIterator>::value>>
  void insert_after(ConstIterator pos, InputIterator first, InputIterator last);

  // pop/erase/clear
  inline ValueType pop_front();
  inline Iterator erase_after(ConstIterator pos);
  inline Iterator erase_after(ConstIterator first, ConstIterator last);
  inline void clear();

  // The implematation detial of pop_front() and erase_after()
  // But these don't free the node in fact, just remove it from list
  // The user can reuse the returnd node
  // Not standard required
  inline Node* extract_front() noexcept;
  inline Node* extract_after(ConstIterator pos) noexcept;

  // accessor
  Iterator begin() noexcept { return { header_->next }; }
  // Don't write as header_->prev->next since header_->prev may be nullptr
  Iterator end() noexcept { return { nullptr }; }
  ConstIterator begin() const noexcept { return { header_->next }; }
  ConstIterator end() const noexcept { return { nullptr }; }
  ConstIterator cbegin() const noexcept { return begin(); }
  ConstIterator cend() const noexcept { return end(); }
  Iterator before_begin() noexcept { return header_; }
  ConstIterator before_begin() const noexcept { return header_; }
  ConstIterator cbefore_begin() const noexcept { return header_; }

  // Not standard required
  Iterator before_end() noexcept { return header_->prev; }
  ConstIterator before_end() const noexcept { return header_->prev; }
  ConstIterator cbefore_end() const noexcept { return header_->prev; }

  Ref front() noexcept { return GET_LINKED_NODE_VALUE(header_->next); }
  ConstRef front() const noexcept { return GET_LINKED_NODE_VALUE(header_->next); }

  // Not standard required
  Ref back() noexcept { return GET_LINKED_NODE_VALUE(header_->prev); }
  ConstRef back() const noexcept { return GET_LINKED_NODE_VALUE(header_->prev); }

  // capacity
  SizeType max_size() const noexcept { return static_cast<SizeType>(-1); }
  bool empty() const noexcept { return header_->next == nullptr; }
  // STL don't provide the size() API
  // Not standard required
  SizeType size() const noexcept { return header_->count; }
  inline void swap(Self& other) noexcept;

  // Search the before iterator of the given value.
  // It's useful for calling erase_after() and insert_after().
  // Not standard required
  Iterator search_before(ValueType const& val, ConstIterator pos);
  template<typename UnaryPred>
  Iterator search_before_if(UnaryPred pred, ConstIterator pos);
  Iterator search_before(ValueType const& val) { return search_before(val, cbefore_begin()); }
  template<typename UnaryPred>
  Iterator search_before(UnaryPred pred) { return search_before(pred, cbefore_begin()); }

  // Operations
  
  /**
   * Let len1 = std::distance(begin(), end()), len2 = std::distance(list.begin(), list.end())
   * It is O(len1+len2) to std::forward_list<>
   * but this is O(min(len1, len2)) + O(1) = O(min(len1, len2))
   * It is more efficient.
   */
  void merge(Self& list);
  void merge(Self&& list) { merge(list); }
  template<typename BinaryPred>
  void merge(Self& list, BinaryPred pred);
  template<typename BinaryPred>
  void merge(Self&& list, BinaryPred pred) { merge(list, std::move(pred)); }

  /**
   * It is O(n) to std::forward_list<>, but this is O(1) here since header->prev
   */
  void splice_after(ConstIterator pos, Self& list);
  void splice_after(ConstIterator pos, Self&& list) { splice_after(pos, list); }
  void splice_after(ConstIterator pos, Self& list, ConstIterator it);
  void splice_after(ConstIterator pos, Self&& list, ConstIterator it) { splice_after(pos, list, it); }
  void splice_after(ConstIterator pos, Self& list, ConstIterator first, ConstIterator last);
  void splice_after(ConstIterator pos, Self&& list, ConstIterator first, ConstIterator last)
  { splice_after(pos, list, first, last); }

  // In C++20, the return type is modified to size_type(i.e. SizeType here).
  // It indicates the number of removed elements
  SizeType remove(ValueType const& val);
  template<typename UnaryPred>
  SizeType remove_if(UnaryPred pred);

  void reverse() noexcept;

  // In C++20, the return type is modified to size_type(i.e. SizeType here).
  // It indicates the number of removed elements
  SizeType unique();
  template<typename BinaryPred>
  SizeType unique(BinaryPred pred);

  // O(nlgn) and stable(Don't destroy iterator)
  // Merge sort(but only merge stage, no need to partition)
  void sort();
  template<typename Compare>
  void sort(Compare cmp);

  // O(nlgn)
  // It take the quick sort, but the performance is wrong than sort()  
  void sort2();
#ifdef FORWARD_LIST_DEBUG
  // For Debugging
  void print() const noexcept;
#endif

private:
  /**
   * create_node has two step
   * allocate memory ==> construct object
   * Also drop_node has two step:
   * destroy object ==> free memory(/deallocate)
   */
  template<typename... Args>
  Node* create_node(Args&& ...args) 
  {
    auto node = AllocTraits::allocate(*this, sizeof(Node));

    node->next = nullptr;
    AllocTraits::construct(*this, &node->val, std::forward<Args>(args)...);

    return node;
  }

  void drop_node(BaseNode* _node) noexcept
  {
    auto node = static_cast<Node*>(_node);
    AllocTraits::destroy(*this, node);
    AllocTraits::deallocate(*this, node, sizeof(Node));
  }

  // Expose the header_ in Base class.
  // This is necessary for base class template.
  using Base::header_;
};


} // namespace zstl

#endif // STL_SUP_FORWARD_LIST_FWD_H
