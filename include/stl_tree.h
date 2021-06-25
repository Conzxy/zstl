/*
 * @file stl_tree.h
 * implementation of Red-Black tree
 * algorithm source from CLRS
 *
 * @author Conzxy
 * @date 24-6-2021
 */

#ifndef _ZXY_TINYSTL_STL_TREE
#define _ZXY_TINYSTL_STL_TREE

#include <cstddef>
#include "_move.h"
#include "stl_iterator.h"
#include "stl_type_traits.h"
#include "Allocator.h"

namespace TinySTL{

/*
 * @enum RBTreeColor
 * @brief provide color of node : red, black
 */
enum class RBTreeColor : bool {
	Red = true, Black = false
};

/*
 * @struct RBTreeBaseNode
 * @brief base RBTree node type(used for header)
 */
struct RBTreeBaseNode{
	using BasePtr = RBTreeBaseNode*;
	using ConstBasePtr = RBTreeBaseNode const*;

	RBTreeColor color;
	BasePtr parent;
	BasePtr left;
	BasePtr right;
};

/*
 * @struct RBTreeNode
 * @tparam Val : value type
 * @brief actual RBTree node type
 */
template<typename Val>
struct RBTreeNode : public RBTreeBaseNode{
	using LinkType = RBTreeNode<Val>*;
	using ConstLinkType = RBTreeNode<Val> const*;

	Val val;
};

/*
 * @struct RBTreeHeader
 * @brief as RBTree.end(), the left link to the minimum, and the right link to the maximum,
 * maintain the node count, the root node is the parent of each other
 */
struct RBTreeHeader{
	RBTreeBaseNode header;
	std::size_t node_count;

	RBTreeHeader(){
		header.color = RBTreeColor::Red;
		Reset();
	}

#if __cplusplus >= 201103L
	RBTreeHeader(RBTreeHeader && other) noexcept{
		if(other.header.parent != nullptr)
			MoveData(other);
		else{
			header.color = RBTreeColor::Red;
			Reset();
		}
	}
#endif

	void MoveData(RBTreeHeader& from){
		header.color = from.header.color;
		header.left = from.header.left;
		header.right = from.header.right;
		header.parent = from.header.parent;
		header.parent->parent = &header;

		node_count = from.node_count;
	}

	/*
	 * @berif: reset the link
	 */
	void Reset(){
		header.parent = nullptr;
		header.left = &header;
		header.right = &header;
		
		node_count = 0;
	}
};

RBTreeBaseNode* RBTreeIncrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeIncrement(RBTreeBaseNode const* node) noexcept;
RBTreeBaseNode* RBTreeDecrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeDecrement(RBTreeBaseNode const* node) noexcept;

/*
 * @class RBTreeIterator
 * @tparam T : value type
 * @note begin() = header.left, end() = header
 */
template<typename T>
class RBTreeIterator{
public:
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using distance_type = std::ptrdiff_t;
	using iterator_catogory = Bidirectional_iterator_tag;

	using BasePtr = RBTreeBaseNode::BasePtr;
	using LinkType = typename RBTreeNode<T>::LinkType;
	using Self = RBTreeIterator;

	RBTreeIterator()
	{}

	explicit RBTreeIterator(BasePtr p)
		: node_(p)
	{}

	reference operator*() const noexcept{
		return *static_cast<LinkType>(node_);
	}

	pointer operator->() const noexcept{
		return static_cast<LinkType>(node_);
	}

	Self& operator++() noexcept{
		node_ =  RBTreeIncrement(node_);
		return *this;
	}

	Self operator++(int) noexcept{
		auto ret = *this;
		node_ = RBTreeIncrement(node_);
		return ret;
	}

	Self& operator--() noexcept{
		node_ = RBTreeDecrement(node_);
		return *this;
	}

	Self operator--(int) noexcept{
		auto ret = *this;
		node_ = RBTreeDecrement(node_);
		return ret;
	}

	friend bool operator==(Self const& lhs, Self const& rhs) noexcept{
		return lhs.node_ == rhs.node_;
	}

	friend bool operator!=(Self const& lhs, Self const& rhs) noexcept{
		return lhs.node != rhs.node_;	//!(lhs == rhs)
	}

protected:
	BasePtr node_;
};

/*
 * @class RBTreeConstIterator
 * @tparam T : value type
 * @note RBTreeIterator can convert into RBTreeConstIterator
 */
template<typename T>
class RBTreeConstIterator{
public:
	using value_type = T;
	using reference = T const&;
	using pointer = T const*;
	using distance_type = std::ptrdiff_t;
	using iterator_catogory = Bidirectional_iterator_tag;

	using BasePtr = RBTreeBaseNode::ConstBasePtr;
	using LinkType = typename RBTreeNode<T>::ConstLinkType;
	using Self = RBTreeConstIterator;

	RBTreeConstIterator()
	{}

	explicit RBTreeConstIterator(BasePtr p)
		: node_(p)
	{}
	
	RBTreeConstIterator(RBTreeIterator<T> const& iter)
		: node_(iter.node_)
	{}

	reference operator*() const noexcept {
		return *static_cast<LinkType>(node_);
	}

	pointer operator->() const noexcept {
		return static_cast<LinkType>(node_);
	}

	Self& operator++() noexcept {
		node_ = RBTreeIncrement(node_);
		return *this;
	}

	Self& operator--() noexcept {
		node_ = RBTreeDecrement(node_);
		return *this;
	}

	Self operator++(int) noexcept {
		auto ret = *this;
		node_ = RBTreeIncrement(node_);
		return ret;
	}

	Self operator--(int) noexcept {
		auto ret = *this;
		node_ = RBTreeDecrement(node_);
		return ret;
	}

	friend bool operator==(Self const& lhs, Self const& rhs) noexcept {
		return lhs.node_ == rhs.node_;
	}

	friend bool operator!=(Self const& lhs, Self const& rhs) noexcept {
		return lhs.node_ != rhs.node_;
	}

protected:
	BasePtr node_;
};

/*
 * @struct KeyCompare
 */
template<typename Compare>
struct KeyCompare{
	Compare key_compare_;

	KeyCompare()
	{ }

	explicit KeyCompare(Compare const& cmp)
		: key_compare_(cmp)
	{ }

#if __cplusplus > 201103L
	KeyCompare(KeyCompare const&) = default;
	
	KeyCompare(KeyCompare&& cmp)
		: key_compare_(TinySTL::move(cmp.key_comapre_))
	{ }
#endif
};

/*
 * @class RBTree
 * @brief implemetation of Red-Black tree
 * @tparam Key : key type
 * @tparam Val : value type
 * @tparam GetKey : callable that can get the key
 * @tparam Compare : predicate that compare element
 * @tparam Alloc : allocator
 */
template<typename Key, typename Val, typename GetKey, typename Compare,
	typename Alloc = TinySTL::allocator<Val>>
class RBTree{
public:
	using Key
}

}//namespace TinySTL

#endif //_ZXY_TINYSTL_STL_TREE
