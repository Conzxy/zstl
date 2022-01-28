#pragma once
#include <algorithm>
#ifndef STL_SUP_FORWARD_LIST_IMPL_H
#define STL_SUP_FORWARD_LIST_IMPL_H

#include "forward_list_fwd.h"
#include "algorithm.h"
#include <vector>
#include "zassert.h"

#ifdef FORWARD_LIST_DEBUG
#include <iostream>
#endif

#define FORWARD_LIST_TEMPLATE ForwardList<T, A>

namespace zstl {

template<typename T, typename A>
auto FORWARD_LIST_TEMPLATE::operator=(Self const& other)
 -> Self&
{
  Iterator beg;
  auto obeg = other.begin();
  if (size() <= other.size()) {
    beg = begin();

    for (SizeType i = 0; i < size(); ++i) {
      AllocTraits::destroy(*this, &beg);
      AllocTraits::construct(*this, &beg, *obeg);
      ++beg; ++obeg;
    }

    for (SizeType i = other.size() - size();
          i > 0; --i)
    {
      push_back(*obeg++);
    }

    assert(size() == other.size());
  }
  else {
    for (SizeType i = size() - other.size(); i > 0; --i) {
      auto old_node = header_->next;
      header_->next = old_node->next;
      drop_node(old_node);
    }

    beg = begin();
    for (SizeType i = 0; i < other.size(); ++i) {

      AllocTraits::destroy(*this, &beg);
      AllocTraits::construct(*this, &beg, *obeg);
      ++beg; ++obeg;
    }
  }

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
    push_back(create_node(val));
  }
}

template<typename T, typename A>
template<typename InputIterator, typename>
void FORWARD_LIST_TEMPLATE::insert_after(ConstIterator pos, InputIterator first, InputIterator last)
{
  for (; first != last; ++first) {
    push_back(create_node(*first));
    // insert_after(pos, create_node(*first));++pos;
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
    while (obeg->next != nullptr) {
      auto old_node = obeg->next->next;
      push_back(list.extract_after(obeg));
      obeg->next = old_node;
    }
  }
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
  if (list.empty()) return ;

  auto pos_node = pos.to_iterator().extract();
  auto old_next = pos_node->next;

  if ((pos == before_begin() && empty())||
       pos == before_end()) 
  {
    header_->prev = list.before_end().extract();
  }

  pos_node->next = list.begin().extract();
  list.before_end().extract()->next = old_next;

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
void FORWARD_LIST_TEMPLATE::sort()
{
  ForwardList less;
  ForwardList equal;
  ForwardList larger;
  
  if (size() <= 2) {
    // reverse
    if (size() == 2) {
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
void FORWARD_LIST_TEMPLATE::swap(Self& other) const noexcept
{
  std::swap(other.header_->prev, this->header_->prev);
  std::swap(other.header_->next, this->header_->next);
  std::swap(other.header_->count, this->header_->count);
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
