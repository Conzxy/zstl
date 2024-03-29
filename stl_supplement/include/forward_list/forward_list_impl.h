#pragma once
#ifndef STL_SUP_FORWARD_LIST_IMPL_H
#define STL_SUP_FORWARD_LIST_IMPL_H

#include "forward_list_fwd.h"
#include "algorithm.h"
#include "zassert.h"

#include <algorithm>
#include <utility>

#ifdef FORWARD_LIST_DEBUG
#include <iostream>
#endif

#define FORWARD_LIST_TEMPLATE ForwardList<T, A>

namespace zstl {

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::resize(SizeType n)
{
  if (size() <= n) {
    auto diff = n - size();

    while (diff--) {
      push_back(T{});
    }
  }
  else {
    auto it = zstl::advance_iter(before_begin(), n);

    while (it.next() != end()) {
      erase_after(it);
    }
  }
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::resize(SizeType n, ValueType const& val)
{
  if (size() <= n) {
    auto diff = n - size();

    while (diff--) {
      push_back(val);
    }
  }
  else {
    auto it = zstl::advance_iter(before_begin(), n);

    while (it.next() != end()) {
      erase_after(it);
    }
  }
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::assign(SizeType count, ValueType const& value)
{
  Iterator beg;
  for (beg = before_begin(); count != 0 && beg.next() != end(); ++beg) {
    AllocTraits::destroy(*this, &*beg.next());
    AllocTraits::construct(*this, &*beg.next(), value);
    --count;
  }
  
  // count > size() 
  if (count > 0) {
    while(count--) {
      push_front(value);
    }
  }
  else if (count < 0) {
    // count < size()
    while (beg.next() != end()) {
      erase_after(beg); 
    }
  }
}

template<typename T, typename A>
template<typename InputIterator, typename>
void FORWARD_LIST_TEMPLATE::assign(InputIterator first, InputIterator last)
{
  Iterator beg;
  for (beg = before_begin(); beg.next() != end() && first != last; ++beg, ++first) {
    AllocTraits::destroy(*this, &*beg.next());
    AllocTraits::construct(*this, &*beg.next(), *first);
  }
  
  // size() < distance(first, last) 
  if (beg.next() == end()) {
    while (first != last) {
      // insert_after(beg, *first);
      push_back(*first++);
    }  
  }
  else if (first == last) {
    // size() > distance(first, last)
    while (beg.next() != end()) {
      erase_after(beg);
    }
  }
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::operator=(Self const& other)
 -> Self&
{
  assign(other.begin(), other.end());
  return *this;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::push_front(Node* new_node) noexcept
{
  forward_list_detail::push_front(header_, new_node);
  ++header_->count;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::push_back(Node* new_node) noexcept 
{
  forward_list_detail::push_back(header_, new_node);
  ++header_->count;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::insert_after(ConstIterator pos, Node* new_node)
{
  auto node = pos.to_iterator().extract();
  forward_list_detail::insert_after(header_, node, new_node);

  ++header_->count;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::insert_after(ConstIterator pos, SizeType count, ValueType const& val)
{
  // At first, I want create a list whose length is count
  // the first node do some work like insert_after()
  // And update header_->prev in term of pos
  // But it is easy to make fault.

  // The push_back() is so simple, thus not wrong.
  while (count--) {
    insert_after(pos, create_node(val));++pos;
  }
}

template<typename T, typename A>
template<typename InputIterator, typename>
void FORWARD_LIST_TEMPLATE::insert_after(ConstIterator pos, InputIterator first, InputIterator last)
{
  for (; first != last; ++first) {
    //push_back(create_node(*first));
    insert_after(pos, create_node(*first));++pos;
  }
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::pop_front()
 ->ValueType
{
  auto node = extract_front();
  auto ret = std::move(node->val);
  drop_node(node);

  return ret;
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::extract_front() noexcept
 -> Node*
{
  --header_->count; 
  return static_cast<Node*>(forward_list_detail::extract_front(header_));
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::erase_after(ConstIterator pos)
 -> Iterator
{
  drop_node(extract_after(pos));
  return pos.next().to_iterator();
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::extract_after(ConstIterator pos) noexcept
 -> Node*
{
  --header_->count;
  return static_cast<Node*>(forward_list_detail::extract_after(header_, pos.to_iterator().extract()));
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::erase_after(ConstIterator first, ConstIterator last)
 -> Iterator
{
  auto first_next = forward_list_detail::extract_after(header_, 
                                                       first.to_iterator().extract(), last.to_iterator().extract());
  
  BaseNode* old_next; 
  
  // Drop all elements in (first, last)
  if (first_next != nullptr) {
    while (first_next != last.extract()) {
      old_next = first_next->next;
      drop_node(static_cast<Node*>(first_next));
      first_next = old_next;
      --header_->count;
    }
  }

  return last.to_iterator();
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::clear()
{
  while (header_->next) {
    auto node = header_->next;
    header_->next = node->next;
    drop_node(node);
  }

  Base::reset();  
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::search_before(ValueType const& val, ConstIterator pos)
 -> Iterator
{
  if (pos == end()) return end();

  for (BaseNode* beg = pos.to_iterator().extract(); beg->next != nullptr; beg = beg->next) {
    if (static_cast<Node*>(beg->next)->val == val) {
      return Iterator(beg);
    }
  }

  return end();
}

template<typename T, typename A>
template<typename UnaryPred>
auto FORWARD_LIST_TEMPLATE::search_before_if(UnaryPred pred, ConstIterator pos)
 -> Iterator
{
  if (pos == end()) return end();

  for (auto beg = pos.extract(); beg->next != nullptr; beg = beg->next) {
    if (pred(static_cast<Node*>(beg->next)->val)) {
      return Iterator(beg);
    }
  }

  return end();
}

template<typename T, typename A>
template<typename BinaryPred>
void FORWARD_LIST_TEMPLATE::merge(Self& list, BinaryPred pred)
{
  if (list.empty()) return;

  BaseNode* beg = header_;
  BaseNode* obeg = list.header_;
  
  while (beg->next != nullptr && obeg->next != nullptr) {
    if (pred(GET_LINKED_NODE_VALUE(obeg->next), GET_LINKED_NODE_VALUE(beg->next))) {
      auto old_node = obeg->next->next;
      insert_after(beg, list.extract_after(obeg));
      beg = beg->next;
      obeg->next = old_node;
    }
    else {
      beg = beg->next;
    }
  }

  if (beg->next == nullptr) {
    // It's also ok when list is empty
    splice_after(before_end(), list);
  }

  assert(list.empty());
  list.reset();
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::merge(Self& list)
{
  if (list.empty()) return;

  BaseNode* beg = header_;
  BaseNode* obeg = list.header_;
  
  while (beg->next != nullptr && obeg->next != nullptr) {
    if (GET_LINKED_NODE_VALUE(obeg->next) < GET_LINKED_NODE_VALUE(beg->next)) {
      auto old_node = obeg->next->next;
      insert_after(beg, list.extract_after(obeg));
      beg = beg->next;
      obeg->next = old_node;
    }
    else {
      beg = beg->next;
    }
  }

  if (beg->next == nullptr) {
    // It's also ok when list is empty
    splice_after(before_end(), list);
  }

  assert(list.empty());
  list.reset();
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::splice_after(ConstIterator pos, Self& list, ConstIterator it)
{
  insert_after(pos, list.extract_after(it));
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::splice_after(ConstIterator pos, Self& list, ConstIterator first, ConstIterator last)
{
  // (first, last)
  while (first.next() != last) {
    insert_after(pos, list.extract_after(first));
  }
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::splice_after(ConstIterator pos, Self& list)
{
  // If list is empty,
  // it is wrong to update prev.
  if (list.empty()) return ;

  auto pos_node = pos.to_iterator().extract();
  auto old_next = pos_node->next;

  if (pos == before_end()) 
  {
    header_->prev = list.before_end().extract();
  }

  pos_node->next = list.begin().extract();
  list.before_end().extract()->next = old_next;
  header_->count += list.size();

  list.reset();
  ZASSERT(list.empty(), "The list must be empty");
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::remove(ValueType const& val)
 -> SizeType
{
  SizeType count = 0;

  auto it = search_before(val);
  while (it != end()) {
    ++count;
    erase_after(it);
    it = search_before(val, it);
  }

  return count;
}

template<typename T, typename A>
template<typename UnaryPred>
auto FORWARD_LIST_TEMPLATE::remove_if(UnaryPred pred)
 -> SizeType
{
  SizeType count = 0;

  auto it = search_before(pred);
  while (it != end()) {
    ++count;
    erase_after(it);
    it = search_before(pred, it);
  }

  return count;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::reverse() noexcept
{
  if (size() < 2) return;
  if (size() == 2) {
    push_back(extract_front());
    return ;
  }

  forward_list_detail::reverse(header_);
}

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::unique()
 -> SizeType
{
  SizeType count = 0;
  for (BaseNode* beg = header_; beg->next != nullptr && beg->next->next != nullptr; ) {
    // Adjacent node with same value
    if (GET_LINKED_NODE_VALUE(beg->next) == GET_LINKED_NODE_VALUE(beg->next->next)) {
      erase_after(beg->next);
      ++count;
    }
    else {
      beg = beg->next;
      if (beg == nullptr) break;
    }
  }

  return count;
}

template<typename T, typename A>
template<typename BinaryPred>
auto FORWARD_LIST_TEMPLATE::unique(BinaryPred pred)
 -> SizeType
{
  SizeType count = 0;
  for (BaseNode* beg = header_; beg->next != nullptr && beg->next->next != nullptr; ) {
    // Adjacent node with same value
    if (pred(GET_LINKED_NODE_VALUE(beg->next), GET_LINKED_NODE_VALUE(beg->next->next))) {
      erase_after(beg->next);
      ++count;
    }
    else {
      beg = beg->next;
      if (beg == nullptr) break;
    }
  }

  return count;
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::sort()
{
  // TODO Optimization
  // lists store non-header list
  // To small list, maybe better a little.
  // It is not significant for large list.
  Self lists[64];
  Self list;  
  int8_t end_of_lists = 0;

  while (!empty()) {
    list.push_front(extract_front());
    
    for (int8_t i = 0; ; ++i) {
      if (lists[i].empty()) {
        list.swap(lists[i]);
        if (i == end_of_lists) end_of_lists++;
        break;
      } 
      else {
        // Merge non-empty list
        // larger list merge shorter list
        if (lists[i].size() > list.size()) {
          lists[i].merge(list);
          list.swap(lists[i]);
        }
        else
          list.merge(lists[i]);
      }
    }
  } 
    
  assert(list.empty());

  for (int i = end_of_lists - 1; i >= 0; --i) {
    list.merge(lists[i]);
  }

  *this = std::move(list);
}

template<typename T, typename A>
template<typename Compare>
void FORWARD_LIST_TEMPLATE::sort(Compare cmp)
{
  Self lists[64];
  Self list;  
  int8_t end_of_lists = 0;

  while (!empty()) {
    list.push_front(extract_front());
    
    for (int8_t i = 0; ; ++i) {
      if (lists[i].empty()) {
        list.swap(lists[i]);
        if (i == end_of_lists) end_of_lists++;
        break;
      } 
      else {
        // Merge non-empty list
        // larger list merge shorter list
        if (lists[i].size() > list.size()) {
          lists[i].merge(list, cmp);
          list.swap(lists[i]);
        }
        else
          list.merge(lists[i], cmp);
      }
    }
  } 
    
  assert(list.empty());

  for (int i = end_of_lists - 1; i >= 0; --i) {
    list.merge(lists[i], cmp);
  }

  *this = std::move(list);
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::sort2() 
{

  ForwardList less;
  ForwardList equal;
  ForwardList larger;
  
  if (size() < 2) {
    return ;
  } 

  if (size() == 2) {
    if (!(*begin() < *begin().next())) {
      push_back(extract_front());
    }

    return ;
  }

  auto pivot = *begin();
  Node* tmp;

  equal.push_back(extract_front());

  while (!empty()) {
    tmp = extract_front();
    if (tmp->val < pivot) {
      less.push_back(tmp);
    }
    else if (pivot < tmp->val) {
      larger.push_back(tmp);
    }
    else {
      equal.push_back(tmp);
    }
  }
  
  less.sort();
  larger.sort();

  less.splice_after(less.cbefore_end(), equal);
  less.splice_after(less.cbefore_end(), larger);

  *this = std::move(less);
}

template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::swap(Self& other) noexcept
{
  std::swap(this->header_, other.header_);
}

template<typename T, typename A>
inline void swap(ForwardList<T, A> const& x, ForwardList<T, A> const& y) noexcept(noexcept(x.swap(y))) 
{
  x.swap(y);
}

#ifdef FORWARD_LIST_DEBUG
template<typename T, typename A>
void FORWARD_LIST_TEMPLATE::print() const noexcept
{
  std::cout << "==== print forward_list ====\n";
  std::cout << "Header -> ";
  for (auto i = header_->next; i != nullptr; i = i->next) {
    std::cout << GET_LINKED_NODE_VALUE(i) << " -> ";
  }
  
  std::cout << "(NULL)\n";
  std::cout << "============================" << std::endl;
}
#endif 

template<typename T, typename A>
inline bool operator==(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return std::equal(x.begin(), x.end(), y.begin());
}

template<typename T, typename A>
inline bool operator!=(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return !(x == y);
}

template<typename T, typename A>
inline bool operator<(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template<typename T, typename A>
inline bool operator>(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return y < x;
}

template<typename T, typename A>
inline bool operator<=(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return !(x > y);
}

template<typename T, typename A>
inline bool operator>=(ForwardList<T, A> const& x, ForwardList<T, A> const& y)
{
  return !(x < y);
}

} // namespace zstl

#endif // STL_SUP_FORWARD_LIST_IMPL_H
