#include "forward_list/algorithm.h"
#include "zassert.h"

#include <stdio.h>
#include <vector>

#define FORWARD_LIST_EMPTY (header->next == nullptr)

namespace zstl { namespace forward_list_detail {

void push_front(Header* header, BaseLinkedNode* new_node) noexcept
{
  // Update header_->prev only when list is empty
  if (FORWARD_LIST_EMPTY) {
    assert(header->prev == header);
    header->prev = new_node;
  }
  
  new_node->next = header->next;
  header->next = new_node;
}

void push_back(Header* header, BaseLinkedNode* new_node) noexcept
{
  if (header->prev != header) {
    header->prev->next = new_node;
  }
  else {
    assert(header->prev == header);
    assert(FORWARD_LIST_EMPTY);
    header->next = new_node;
  }

  header->prev = new_node;
}

BaseLinkedNode* extract_front(Header* header) noexcept
{
  assert(!FORWARD_LIST_EMPTY);

  auto ret = header->next;
  header->next = header->next->next;
  
  if (FORWARD_LIST_EMPTY) {
    header->prev = header;
  }
  
  // Reset returned node, to avoid construct loop in some cases
  ret->next = nullptr;

  return ret;
}

void insert_after(Header* header, BaseLinkedNode* pos, BaseLinkedNode* new_node) noexcept
{
  // Push to back, update header_->prev
  // If pos == header, and FORWARD_LIST_EMPTY == true
  // pos == heaer->prev also equal to true
  if (pos == header->prev) {
    header->prev = new_node;
  }

  new_node->next = pos->next;
  pos->next = new_node;
}

BaseLinkedNode* extract_after(Header* header, BaseLinkedNode* pos) noexcept
{
  ZASSERT(pos != nullptr, "The position argument must not be end()");
  ZASSERT(!FORWARD_LIST_EMPTY, "ForwardList must be not empty");
  ZASSERT(pos->next != nullptr, "The next node of the position argument mustn't be nullptr");

  auto ret = pos->next;

  // If pos->next is last element, update it
  if (ret == header->prev) {
    // FIXME Need to update head->prev = nullptr when pos == header
    header->prev = pos;
  }

  pos->next = pos->next->next;

  ret->next = nullptr;
  return ret;

}

BaseLinkedNode* extract_after(Header* header, BaseLinkedNode* first, BaseLinkedNode* last) noexcept
{
  // The length of the range must be 1 at least
  if (first->next != last) {
    BaseLinkedNode* old_next;

    // If last is the end iterator, indicates the header_->prev need to update
    if (last == nullptr) {
      header->prev = first;
    }

    auto first_next = first->next;
    first->next = last;

    return first_next;
  }

  return nullptr;
}

void reverse(Header* header) noexcept
{
  // Or use recurrsion to implemetation
  std::vector<BaseLinkedNode*> nodes;
  nodes.reserve(header->count);

  while (!FORWARD_LIST_EMPTY) {
    nodes.push_back(extract_front(header));
  }
  
  assert(FORWARD_LIST_EMPTY); 
  assert(nodes.size() == nodes.capacity());

  using SizeType = decltype(header->count);
  for (SizeType i = 0; i < nodes.size(); ++i) {
    push_front(header, nodes[i]);
  }
}

} } // namespace zstl::forward_list_detial
