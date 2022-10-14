#ifndef ZSTL_HASH_TABLE_HASH_NODE_H
#define ZSTL_HASH_TABLE_HASH_NODE_H

namespace zstl {

/**
 * Represent a single linked-list
 * @note
 * Don't use version with header
 * since Vector<> is consecutive memory
 * if a bucket in Vector<> is not used, it is a waste.
 * Also, you can use HashNode** that pointer to header
 * but this is an additional indirectly access to decrease performance(In space and time)
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

} // namespace zstl

#endif // ZSTL_HASH_TABLE_HASH_NODE_H
