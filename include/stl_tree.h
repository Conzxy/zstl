#ifndef _ZXY_ZSTL_STL_TREE
#define _ZXY_ZSTL_STL_TREE

#include "stl_move.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "stl_exception.h"
#include "stl_utility.h"
#include "stl_algobase.h"
#include "config.h"
#include "noncopyable.h"
#include "allocator.h"

#include <cstddef>
#include <cstring>
#include <assert.h>

#ifdef RBTREE_DEBUG
#include <iostream>
#endif

namespace zstl {

/**
 * @enum RBTreeColor
 * @brief 
 * Provide color of node is either red or black
 * to satifies the property: every node either red or black
 */
enum class RBTreeColor : bool {
	Red = true, 
  Black = false
};

/**
 * @struct RBTreeBaseNode
 * @brief base RBTree node type
 * there are two advantage:
 * (1) RBTree header no need value field, so base is better than actual node type
 * (2) unqualified type, so related algorithm can reuse for all type
 * e.g. RBTree<Val=int> and RBTree<Val=char>， their algorithm is same
 */
struct RBTreeBaseNode {
	using BasePtr = RBTreeBaseNode*;
	using ConstBasePtr = RBTreeBaseNode const*;

	RBTreeColor color;
	BasePtr parent;
	BasePtr left;
	BasePtr right;

	//helper
	static BasePtr Minimum(BasePtr x) noexcept {
		while(x->left)
			x = x->left;
		return x;
	} 

	static ConstBasePtr Minimum(ConstBasePtr x) noexcept {
		while(x->left)
			x = x->left;
		return x;
	}

	static BasePtr Maximum(BasePtr x) noexcept {
		while(x->right)
			x = x->right;
		return x;
	}

	static ConstBasePtr Maximum(ConstBasePtr x) noexcept {
		while(x->right)
			x = x->right;
		return x;
	}

};

/**
 * @struct RBTreeNode
 * @tparam Val value type
 * @brief actual RBTree node type
 */
template<typename Val>
struct RBTreeNode : public RBTreeBaseNode{
	using LinkType = RBTreeNode<Val>*;
	using ConstLinkType = RBTreeNode<Val> const*;
	
	LinkType Left() noexcept
	{ return static_cast<LinkType>(left); }

	ConstLinkType Left() const noexcept 
	{ return static_cast<ConstLinkType>(left); }

	LinkType Right() noexcept 
	{ return static_cast<LinkType>(right); }

	ConstLinkType Right() const noexcept 
	{ return static_cast<ConstLinkType>(right); }

	Val val;
};

/**
 * @struct RBTreeHeader
 * @brief as RBTree.end(), the left link to the minimum, and the right link to the maximum,
 * maintain the node count, the root node is the parent of each other
 */
struct RBTreeHeader {
	RBTreeBaseNode header;
	std::size_t node_count;

	RBTreeHeader(){
		header.color = RBTreeColor::Red;
		Reset();
	}
	
	RBTreeHeader(RBTreeHeader && other) noexcept{
		if(other.header.parent != nullptr)
			MoveData(other);
		else{
			header.color = RBTreeColor::Red;
			Reset();
		}
	}

protected:
	void MoveData(RBTreeHeader& from){
		header.color = from.header.color;
		header.left = from.header.left;
		header.right = from.header.right;
		header.parent = from.header.parent; // root
		header.parent->parent = &header; // reset header

		// FIXME test if error occur
		from.header.parent->parent = nullptr;		

		node_count = from.node_count;
	}

	/**
	 * @berif: reset the link to uninitialzied state
	 */
	void Reset(){
		header.parent = nullptr;
		header.left = &header;
		header.right = &header;
		
		node_count = 0;
	}
};

/**
 * @brief find the successor and predecessor of given node
 * the detail implementation of iterator operator++ and operator--
 * @see https://conzxy.github.io/2021/01/25/CLRS/Search-Tree/BinarySearchTree/
 */
RBTreeBaseNode* RBTreeIncrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeIncrement(RBTreeBaseNode const* node) noexcept;
RBTreeBaseNode* RBTreeDecrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeDecrement(RBTreeBaseNode const* node) noexcept;

/**
 * @struct RBTreeIterator
 * @tparam T value type
 * @note begin() = header.left, end() = header
 */
template<typename T>
struct RBTreeIterator {
	using value_type = T;
	using reference = T&;
	using pointer = T*;
	using distance_type = std::ptrdiff_t;
	using iterator_catogory = Bidirectional_iterator_tag;

	using BasePtr = RBTreeBaseNode::BasePtr;
	using LinkType = typename RBTreeNode<T>::LinkType;
	using ConstLinkType = typename RBTreeNode<T>::ConstLinkType;

	using Self = RBTreeIterator;

	RBTreeIterator()
	{}

	RBTreeIterator(BasePtr p)
		: node_(p)
	{}
  
	LinkType node() noexcept {
		return static_cast<LinkType>(node_);
	}
	
	ConstLinkType node() const noexcept {
		return static_cast<ConstLinkType>(node_);
	}

	reference operator*() const noexcept{
		return static_cast<LinkType>(node_)->val;
	}

	pointer operator->() const noexcept{
		return &static_cast<LinkType>(node_)->val;
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
		return lhs.node_ != rhs.node_;	//!(lhs == rhs)
	}

	BasePtr node_;
};

/**
 * @struct RBTreeConstIterator
 * @tparam T value type
 * @note RBTreeIterator must be able to convert into RBTreeConstIterator
 */
template<typename T>
struct RBTreeConstIterator{
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

	RBTreeConstIterator(BasePtr p)
		: node_(p)
	{}
	
	RBTreeConstIterator(RBTreeIterator<T> const& iter)
		: node_(iter.node_)
	{}
	
	LinkType node() const noexcept {
		return static_cast<LinkType>(node_);
	}
	
	RBTreeIterator<T> ConstCast() noexcept {
		return const_cast<RBTreeBaseNode*>(node_);
	}

	reference operator*() const noexcept {
		return static_cast<LinkType>(node_)->val;
	}

	pointer operator->() const noexcept {
		return &static_cast<LinkType>(node_)->val;
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

	BasePtr node_;
};

/**
 * @struct KeyCompare
 * @tparam Compare functor used to compare two key
 */
template<typename Compare>
struct KeyCompare{
	Compare key_compare_;

	KeyCompare()
	{ }

	explicit KeyCompare(Compare const& cmp)
		: key_compare_(cmp)
	{ }

	KeyCompare(KeyCompare const&) = default;
	
	KeyCompare(KeyCompare&& cmp) = default;
};

/**
 * @brief relink inserted node and its parent and fixup to maintain red-black property
 * @param insert_left indicate if insert left(i.e. left must be null)
 * @param x inserted node
 * @param p parents of x
 * @param header header sentinel pointing to leftmost, rightmost and root
 */
void RBTreeInsertAndFixup(
	const bool insert_left,
	RBTreeBaseNode* x,
	RBTreeBaseNode* p,
	RBTreeBaseNode* header) noexcept;

/**
 * @brief Red-Black rebalance algorithm for delete
 * @param x node to be deleted
 * @param root root of rbtree
 * @param leftmost the most left node of rbtree
 * @param rightmost the most right node of rbtree
 * @return actual deleted node location(i.e. x)
 * @note 
 * the reason why return x instead of void is that delete RBTreeBaseNode is imcompleted
 * because RBTreeBaseNode destructor is non-virtual
 * @see https://conzxy.github.io/2021/01/26/CLRS/Search-Tree/BlackRedTree/
 */
RBTreeBaseNode* 
RBTreeEraseAndFixup(
	RBTreeBaseNode* x, 
	RBTreeBaseNode*& root,
	RBTreeBaseNode*& leftmost,
	RBTreeBaseNode*& rightmost);

/**
 * @brief get the black height of given node
 * @param node given node
 * @param root root of rbtree
 * @pre node have no child
 * @return black height
 * @note black height not include root itself
 */
inline std::size_t BH(RBTreeBaseNode const* node, RBTreeBaseNode const* root) noexcept {
	if(!node)
		return 0;
	
	std::size_t bh = node->color == RBTreeColor::Black ? 1 : 0;
	if(node == root)
		return bh - 1;
	else
		return bh + BH(node->parent, root);
}

// For debug
#ifdef RBTREE_DEBUG
#define TEST_RB_PROPERTY(rbtree) \
	do{ \
		auto is_required = rbtree.IsRequired(); \
		if(!is_required.first) \
			printf("%s\n", is_required.second); \
		else \
			printf("rbtree's property maintained\n"); \
	}while(0)	
#endif

/**
 * @class RBTree
 * @brief implemetation of Red-Black tree
 * @tparam Key key type
 * @tparam Val value type
 * @tparam GetKey callable that can get the key
 * @tparam Compare predicate that compare element
 * @tparam Alloc allocator
 */
template<typename Key, typename Val, typename GetKey, typename Compare,
	typename Alloc = zstl::allocator<Val>>
class RBTree {
public:
	using key_type = Key;
	using value_type = Val;
	using key_compare = Compare;
	using allocator_type = Alloc;
	using pointer = Val*;
	using const_pointer = Val const*;
	using reference = Val&;
	using const_reference = Val const&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	
private:
  using get_key = GetKey;
	using NodeAllocator = typename Alloc::template rebind<RBTreeNode<Val>>;
	using AllocTraits = allocator_traits<NodeAllocator>;

protected:
	using BasePtr = RBTreeBaseNode*;
	using ConstBasePtr = RBTreeBaseNode const*;
	using LinkType = RBTreeNode<Val>*;
	using ConstLinkType = RBTreeNode<Val> const*;

public:
	/**
	 * @class ReuseOrAlloc
	 * @brief 
	 * reuse original nodes, destroy and reconstruct
	 * when no nodes, plain alloc
	 */
  struct ReuseOrAlloc : noncopyable {
  public:
    explicit ReuseOrAlloc(RBTree& rbtree) 
      : rbtree_{ rbtree }
      , node_{ rbtree.RightMost() }
      , root_{ rbtree.Root() }
    {
      if (root_) {
        // for Extract()
        root_->parent = nullptr;
        // node is rightmost, so left subtree
        // have a red child at most
        if (node_->left) {
          node_ = node_->left;
        }
      } else {
        // if root_ is NULL, node_ is header
        node_ = nullptr;
      }
    }

    ~ReuseOrAlloc() noexcept {		
  #ifdef RBTREE_DEBUG
      printf("reused node count: %lu\n", count_);
  #endif

      // if root_ is not NULL, there are some nodes can't be reused
      // we can only destroy and free them
      if (root_) {
        rbtree_.Erase(static_cast<LinkType>(root_));
      }
    }

    template<typename... Args>
    LinkType operator()(Args&&... args) noexcept {
      auto reuse_node = static_cast<LinkType>(Extract());

      if (reuse_node) {
  #ifdef RBTREE_DEBUG
        ++count_;
  #endif
        rbtree_.DestroyNode(reuse_node);
        rbtree_.ConstructNode(reuse_node, STL_FORWARD(Args, args...)...);
        return reuse_node;
      } else {
        return rbtree_.CreateNode(STL_FORWARD(Args, args)...);
      }
    }

  private:
    BasePtr Extract() noexcept {
      if (node_) {
        auto reuse_node = node_;
        node_ = reuse_node->parent;

        if (node_) {
          if (node_->right == reuse_node) {
						node_->right = nullptr;

            if (node_->left) {
              node_ = node_->left;

              while (node_->right) {
                node_ = node_->right;
              }

              if (node_->left) {
                node_ = node_->left;
              }
            }
          } else { // end if node->right == reuse_node
            node_->left = nullptr;
          }
        } else { // end if node_
          // reuse_node parent is NULL, set root_ to NULL
          // to avoid destroy root in top-down in destructor
          root_ = nullptr;
        }

        return reuse_node; 
      }

      return nullptr;
    }

    RBTree& rbtree_;
    BasePtr node_;
    BasePtr root_;

  #ifdef RBTREE_DEBUG
    size_t count_ = 0;
  #endif
  };


	/**
	 * @class PlainAlloc
	 */
	class PlainAlloc{
	public:
		explicit PlainAlloc(RBTree& tree)
			: tree_(tree)
		{ }

		template<typename ...Args>
		LinkType operator()(Args&&... args){
			return tree_.CreateNode(STL_FORWARD(Args, args)...);
		}

	private:
		RBTree& tree_;
	};

	NodeAllocator& GetNodeAllocator() noexcept {
		return impl_;
	}

	NodeAllocator const& GetNodeAllocator() const noexcept {
		return impl_;
	}

	allocator_type get_allcator() const noexcept {
		return allocator_type();
	} 

protected:
	//helper
	LinkType GetNode(){
		return AllocTraits::allocate(GetNodeAllocator());
	}

	void PutNode(LinkType p) noexcept {
		AllocTraits::deallocate(GetNodeAllocator(), p);
	}

	template<typename ...Args>
	void ConstructNode(LinkType ptr, Args&&... args){
		STL_TRY	
		{
			AllocTraits::construct(GetNodeAllocator(), &ptr->val,
				STL_FORWARD(Args, args)...);
			ptr->color = RBTreeColor::Red;
			ptr->left = nullptr;
			ptr->right = nullptr;
			ptr->parent = nullptr;

		}
		CATCH_ALL
		{
			ptr->~RBTreeNode<Val>();
			PutNode(ptr);
			RETHROW
		}	
	}

	void DestroyNode(LinkType ptr) noexcept {
		AllocTraits::destroy(GetNodeAllocator(), ptr);
	}

	template<typename ...Args>
	LinkType CreateNode(Args&&... args){
		auto new_node = GetNode();
		ConstructNode(new_node, zstl::forward<Args>(args)...);
		return new_node;
	}

	void DropNode(LinkType ptr) noexcept {
		DestroyNode(ptr);
		PutNode(ptr);
	}

	template<typename Policy>
	LinkType CloneNode(Val const& val, Policy& policy) {
		return policy(val);
	}

	/**
	 * @class RBTreeImpl
	 * @brief detail field of RBTree
	 * @note use a technique : EBCO
	 */
	struct RBTreeImpl
	: public NodeAllocator
	, public KeyCompare<Compare>
	, public RBTreeHeader{
		using CompBase = KeyCompare<Compare>;

		RBTreeImpl()
		 : NodeAllocator()
		{ }

		RBTreeImpl(NodeAllocator&& alloc)
		 : NodeAllocator(zstl::move(alloc))
		{ }

		RBTreeImpl(key_compare const& cmp)
			: CompBase(cmp)
		{ }

		RBTreeImpl(RBTreeImpl const&) = default;
		RBTreeImpl(RBTreeImpl&&) = default;

		using RBTreeHeader::Reset;
		using CompBase::key_compare_;
	};

protected:
	BasePtr Header() noexcept {
		return &impl_.header;
	}

	ConstBasePtr Header() const noexcept {
		return &impl_.header;
	}

	BasePtr& Root() noexcept {
		return impl_.header.parent;
	}

	ConstBasePtr Root() const noexcept {
		return impl_.header.parent;
	}

	BasePtr& LeftMost() noexcept {
		return impl_.header.left;
	}

	ConstBasePtr LeftMost() const noexcept {
		return impl_.header.left;
	}

	BasePtr& RightMost() noexcept {
		return impl_.header.right;
	}

	ConstBasePtr RightMost() const noexcept {
		return impl_.header.right;
	}

	static const Key
	_Key(ConstLinkType x) {
		//TODO: static_assert(Is_invocable_v)
		return GetKey()(x->val);
	}

	static const Key
	_Key(ConstBasePtr x) {
		return GetKey()(static_cast<ConstLinkType>(x)->val);
	}

	static const_reference
	Value(ConstLinkType x) noexcept {
		return x->val;
	}

	static const_reference
	Value(ConstBasePtr x) noexcept {
		return static_cast<ConstLinkType>(x)->val;
	}

	static LinkType
	Left(BasePtr x) noexcept {
		return static_cast<LinkType>(x->left);
	}

	static ConstLinkType
	Left(ConstBasePtr x) noexcept {
		return static_cast<ConstLinkType>(x->left);
	}

	static LinkType
	Right(BasePtr x) noexcept {
		return static_cast<LinkType>(x->right);
	}

	static ConstLinkType
	Right(ConstBasePtr x) noexcept {
		return static_cast<ConstLinkType>(x->right);
	}
	
	static BasePtr&
	Parent(BasePtr x) noexcept {
		return x->parent;
	}

	static ConstBasePtr
	Parent(ConstBasePtr x) noexcept {
		return x->parent;
	}

	static BasePtr
	Minimum(BasePtr x) noexcept {
		return RBTreeBaseNode::Minimum(x);
	}

	static ConstBasePtr
	Minimum(ConstBasePtr x) noexcept {
		return RBTreeBaseNode::Minimum(x);
	}

	static BasePtr
	Maximum(BasePtr x) noexcept {
		return RBTreeBaseNode::Maximum(x);
	}

	static ConstBasePtr
	Maximum(ConstBasePtr x) noexcept {
		return RBTreeBaseNode::Maximum(x);
	}

public:
	using iterator = RBTreeIterator<Val>;
	using const_iterator = RBTreeConstIterator<Val>;
	using reverse_iterator = zstl::reverse_iterator<iterator>;
	using const_reverse_iterator = zstl::reverse_iterator<const_iterator>;

	Compare key_comp() const noexcept {
		return impl_.key_compare_;
	}
  
	iterator begin() noexcept {
		return iterator(impl_.header.left);
	}

	const_iterator begin() const noexcept {
		return const_iterator(impl_.header.left);
	}

	const_iterator cbegin() const noexcept {
		return const_iterator(impl_.header.left);
	}	

	iterator end() noexcept {
		return iterator(&impl_.header);
	}

	const_iterator end() const noexcept {
		return const_iterator(&impl_.header);
	}

	const_iterator cend() const noexcept {
		return const_iterator(&impl_.header);
	}

	reverse_iterator rbegin() noexcept {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cbegin());
	}

	reverse_iterator rend() noexcept {
		return reverse_iterator(end());
	}

	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(end());
	}

	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cend());
	}
	
	size_type size() const noexcept {
		return impl_.node_count;
	}
	
	bool empty() const noexcept {
		return impl_.node_count
			&& Header()->left == Header()
			&& Header()->right == Header()
			&& Header()->parent == nullptr;
	}

	void clear() noexcept {
		Erase(static_cast<LinkType>(Root()));
		impl_.Reset();
	}

	RBTree() = default;
	
	~RBTree(){
		if(Root())
			Erase(static_cast<LinkType>(Root()));
	}

	RBTree(RBTree const& rhs) {
		PlainAlloc policy(*this);
		Copy(rhs, policy);
	}
	
	RBTree(RBTree&& rhs) noexcept
		: impl_(zstl::move(rhs.impl_))
	{
		rhs.Header()->Reset;
	}
	
	RBTree& operator=(RBTree const& rhs){
		if(this != &rhs){
			ReuseOrAlloc policy(*this);
			Copy(rhs, policy);
		}

		return *this;
	}
	
	RBTree& operator=(RBTree&& rhs) noexcept {
		if(this != &rhs){
			impl_.impl_ = zstl::move(rhs.impl_);
			rhs.Header()->Reset();

			return *this;
		}
	}

	void swap(RBTree const& rhs) noexcept {
		using zstl::swap;
		swap(this->impl_, rhs.impl_);
	}

	///////////////////////////////
	////////INSERT_MODULE//////////
	///////////////////////////////
	/**
	 * @brief Insert new node whose value is given value
	 * @param arg given value
	 * @return a pair value, 
	 * the first value is iterator indicate inserted location
	 * and the second value is a boolean if insert successfully
	 */
	template<typename Arg>
	pair<iterator, bool> InsertUnique(Arg&& arg) {
    auto res = GetInsertUniquePos(GetKey()(arg));
    
    if (res.second == nullptr) {
      return zstl::make_pair(iterator(res.first), false);
    }

    return zstl::make_pair(
        InsertAux(STL_FORWARD(Arg, arg), res.first, res.second),
        true);
  }

	template<typename Arg>
	iterator InsertEqual(Arg&& arg) {
    auto res = GetInsertUniquePos(GetKey()(arg));
    return InsertAux(STL_FORWARD(Arg, arg), res.first, res.second);
  }

	template<
	typename II, 
	typename = Enable_if_t<is_input_iterator<II>::value>>
	void InsertUnique(II first, II last) {
		for (; first != last; ++first) {
			InsertUnique(*first);
		}
	}	

	template<
	typename II, 
	typename = Enable_if_t<is_input_iterator<II>::value>>
	void InsertEqual(II first, II last) {
		for (; first != last; ++first) {
			InsertEqual(*first);
		}
	}	

	template<typename ...Args>
	pair<iterator, bool> EmplaceUnique(Args&&... args) {
    auto new_node = CreateNode(STL_FORWARD(Args, args)...);
    
    STL_TRY { 
      auto res = GetInsertUniquePos(_Key(new_node));
      assert(res.first == nullptr);
      if (res.second) {
        return make_pair(EmplaceAux(new_node, res.first, res.second), true);
      } else {
        DropNode(new_node);
        return make_pair(iterator(res.first), false);
      }
    } CATCH_ALL {
      DropNode(new_node); 
      RETHROW
    }
  }

	template<typename ...Args>
	iterator EmplaceEqual(Args&&... args) {
    auto new_node = CreateNode(STL_FORWARD(Args, args)...);

    STL_TRY {
      auto res = GetInsertUniquePos(_Key(new_node));
      assert(res.first == nullptr);
    
      return EmplaceAux(new_node, res.first, res.second); 
    } CATCH_ALL {
      DropNode(new_node);
      RETHROW
    }
	}

	template<typename Arg>
	pair<iterator, bool> InsertHintUnique(const_iterator pos, Arg&& arg) {
    auto res = GetInsertUniqueHintPos(pos, GetKey()(arg));
  
    if (res.second == nullptr) {
      return make_pair(iterator(res.first), false);
    } 

    return make_pair(
        InsertAux(STL_FORWARD(Arg, arg), res.first, res.second),
        true);
  }
  
	template<typename Arg>
	iterator InsertHintEqual(const_iterator pos, Arg&& arg) {
    auto res = GetInsertEqualHintPos(pos, GetKey()(arg));

    return InsertAux(STL_FORWARD(Arg, arg), res.first, res.second);
  }

	template<typename ...Args>
	pair<iterator, bool> EmplaceHintUnique(
		const_iterator pos,
		Args&&... args) {
    auto new_node = CreateNode(STL_FORWARD(Args, args)...);
    auto res = GetInsertUniqueHintPos(pos, _key(new_node));

    if (res.second == nullptr) {
      return make_pair(res.first, false);
    }
  
    STL_TRY {
      return make_pair(
          EmplaceAux(new_node, res.first, res.second),
          true);
    } CATCH_ALL {
      DropNode(new_node);
      RETHROW
    }
  }

	template<typename ...Args>
	iterator EmplaceHintEqual(
		const_iterator pos,
		Args&&... args) {
    auto new_node = CreateNode(STL_FORWARD(Args, args)...);
    auto res = GetInsertEqualHintPos(pos, _Key(new_node));
    
    STL_TRY {
      return InsertAux(new_node, res.first, res.second);
    } CATCH_ALL {
      DropNode(new_node);
      RETHROW
    }
  }

	//////////////////////////////
	/////////ERASE MODULE/////////
	//////////////////////////////
	
	iterator erase(const_iterator pos){
		assert(pos != end());

#ifdef RBTREE_DEBUG
		//printf("erase node: %d\n", *pos);
		//Print();
		TEST_RB_PROPERTY((*this));
#endif
		auto next = NextIter(pos).ConstCast();
		EraseAux(pos);
		return next;
	}

	size_type erase(Key const& key){
		auto range = equal_range(key);
		int cnt = 0;
		auto first = range.first;
		auto last = range.second;

		while(first != last){
			++cnt;
			erase(first++);
		}

		return cnt;
	}
	
	void erase(const_iterator first, const_iterator last){
		if(first == begin() && last == end())
			clear();
		else
			while(first != last)
				erase(first++);
	}

	///////////////////////////////
	/////////LOOPUP MODULE/////////
	///////////////////////////////
	/**
	 * @brief find given key if in RBTree
	 * @exception Compare object may throw
	 * @return not found return end() otherwise return iterator which indicate key location
	 */
	const_iterator find(Key const& key) const{
		auto y = Header(); //last node which is not less than key
		auto x = Root(); //current node

		while(x){
			//x >= key
			if(!impl_.key_compare_(_Key(x), key)) {
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		const_iterator j(y);
		return (j == end() || impl_.key_compare_(key, _Key(y))) ?
				end() : j;
	}

	iterator find(Key const& key){
		auto y = Header();
		auto x = Root();

		while(x){
			if(!impl_.key_compare_(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		iterator j(y);
		return (j == end() || impl_.key_compare_(key, _Key(y))) ?
				end() : j;

	}
	
	/**
	 * @brief wrap find, just predicate if contains given key
	 */
	bool contains(Key const& k) const {
		return find(k) != end();
	}
	
	/**
	 * @brief acquire an iterator pointing to the first element
	 * that is greater than key
	 */
	iterator upper_bound(Key const& key) {
		auto y = Header();
		auto x = Root();

		while(x){
			if(key_comp(key, _Key(x))){
				y = x;
				x = x->right;
			}
			else x = x->left;
		}

		return y;
	}

	const_iterator upper_bound(Key const& key) const {
		auto y = Header();
		auto x = Root();

		while(x){
			if(impl_.key_compare_(key, _Key(x))){
				y = x;
				x = x->right;
			}
			else x = x->left;
		}

		return y;
	}
	
	/**
	 * @brief return an iterator pointing to the first 
	 * element that is not less than given key
	 */
	iterator lower_bound(Key const& key){
		auto y = Header();
		auto x = Root();

		while(x){
			if(impl_.key_compare_(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		return y;
	}

	const_iterator lower_bound(Key const& key) const {
		auto y = Header();
		auto x = Root();

		while(x){
			if(impl_.key_compare_(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		return y;
	}

	/**
	 * @brief return a range containing all elements
	 * with the given key in the container
	 */
	pair<iterator, iterator> equal_range(Key const& key){
		return make_pair(lower_bound(key), upper_bound(key));
	}

	pair<const_iterator, const_iterator> equal_range(Key const& key) const {
		return make_pair(lower_bound(key), upper_bound(key));
	}

	/**
	 * @brief the number of elements with key
	 */
	size_type count(Key const& key) const {
		auto range = equal_range(key);
		return distance(range.first, range.second);
	}

#ifdef RBTREE_DEBUG
	/**
	 * @brief print RBTree as a directory tree
	 */
	void Print(std::ostream& os = std::cout) const;

	/**
	 * @brief verify RBTree if requires red-black property
	 * @return a pair, the first value is a boolean indicate if requred, and the second indicate violated details
	 */
	pair<bool, const char*> IsRequired() const;
#endif

private:
	///////////////////
	/////INSERT_AUX////
	///////////////////
   template<typename Arg, typename = zstl::Enable_if_t<
	 	Is_convertible<Arg, value_type>::value>>
	//template<typename Arg>
	iterator InsertAux(Arg&& arg, BasePtr cur, BasePtr p) {
    auto new_node = CreateNode(STL_FORWARD(Arg, arg));

    STL_TRY {
      EmplaceAux(new_node, cur, p);
    } CATCH_ALL {
      DropNode(new_node);
      RETHROW
    }

    return new_node;
	}

  iterator EmplaceAux(LinkType new_node, BasePtr cur, BasePtr p) {
		// insert_left used for RBTreeInsertAndFixup() to reset link
		//
    // If cur is not nullptr, we can think caller insert into left
    // !This property can be used for GetInsertXXXPos() to reduce the 
		// overhead of one compare 
		//
		// if cur is nullptr, and it is not header, we should use key_comp()
		// to determine if insert into left or right

		assert(new_node);
		bool insert_left = cur || p == Header() ||
				impl_.key_compare_(_Key(new_node), _Key(p));
     
		RBTreeInsertAndFixup(insert_left, new_node, p, Header());
		++impl_.node_count;

		return new_node;
  }

  // The reason why I Set GetXXXPos() interface is that
  // when we pass constructed value to insert(), we can determine if
  // construct a new node in tree.
  // But for emplace(), we can't do it, because we don't know it's key from the
  // parameters that are used for constructing value
  pair<BasePtr, BasePtr> GetInsertUniquePos(key_type const& key);
  pair<BasePtr, BasePtr> GetInsertEqualPos(key_type const& key);
  pair<BasePtr, BasePtr> GetInsertUniqueHintPos(
      const_iterator pos, 
      key_type const& key);

  pair<BasePtr, BasePtr> GetInsertEqualHintPos(
      const_iterator pos,
      key_type const& key);

	////////////////
	/////COPY///////
	////////////////
	template<typename Policy>
	LinkType Copy(LinkType x, BasePtr p, Policy& policy);

	template<typename Policy>
	void Copy(RBTree const& rb, Policy& policy){
		if(rb.Root()) {
      Root() = Copy(
          static_cast<LinkType>(const_cast<BasePtr>(rb.Root())),
          Header(),
          policy);

      LeftMost() = Minimum(Root());
      RightMost() = Maximum(Root());
      impl_.node_count = rb.impl_.node_count;
    } else {
      clear();
      impl_.Reset(); 
    }
	}

	////////////////////
	//////ERASE AUX/////
	////////////////////
	/**
	 * @brief erase all node from root top-down
	 * @complexity O(h^2) = O((lgn)^2)
	 */
	void Erase(LinkType root) noexcept;
	void EraseAux(const_iterator node) noexcept;

	/////////////////////
	/////DEBUG HELPER////
	/////////////////////
#ifdef RBTREE_DEBUG	
	void PrintRoot(ConstLinkType root,  std::ostream& os) const;
	void PrintSubTree(ConstLinkType root, std::ostream& os, std::string const& prefix = "") const;
#endif
	friend inline bool operator==(RBTree const& lhs, RBTree const& rhs) noexcept {
		return lhs.size() == rhs.size()
			&& zstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	
	friend inline bool operator!=(RBTree const& lhs, RBTree const& rhs) noexcept {
		return !(lhs == rhs);
	}

	friend inline bool operator<(RBTree const& lhs, RBTree const& rhs) noexcept {
		return zstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin());
	}

	friend inline bool operator>=(RBTree const& lhs, RBTree const& rhs) noexcept {
		return !(lhs < rhs);
	}

	friend inline bool operator>(RBTree const& lhs, RBTree const& rhs) noexcept {
		return rhs < lhs;
	}

	friend inline bool operator<=(RBTree const& lhs, RBTree const& rhs) noexcept {
		return !(lhs > rhs);
	}

protected:
	RBTreeImpl impl_;
};


/******* implemetation ********/
template<typename K, typename V, typename GK, typename CP, typename Alloc>
inline void swap(RBTree<K, V, GK, CP, Alloc> const& lhs, RBTree<K, V, GK, CP, Alloc> const& rhs) noexcept {
	return lhs.swap(rhs);
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
auto RBTree<K, V, GK, CP, Alloc>::GetInsertUniquePos(key_type const& key) 
-> pair<BasePtr, BasePtr> {
	// use y as a trailing pointer to track the new_node
	auto y = Header();
	auto x = Root();

	// indicate new_node will insert into left or right
	// that is, position of "hole"	
	bool cmp_res = true;
	while(x){
		y = x;
		cmp_res = impl_.key_compare_(key, _Key(x));
		x = cmp_res ? x->left : x->right;
	}

	// if cmp_res is false, is is just a y
	// otherwise it is predecessor of y when y is not leftmost node
	iterator pre(y); 

	// if cmp_res is true,  <1>
	// there two cases:
	// 1) left is hole
	// we must compare new_node with predecessor of y
	// because it in right subtree of seccessor, i.e., less than or equal it.
	// But, if y is leftmost node, it is no need to compare and just insert.
  //
	// 2) y is header
	// just insert root to its left and reset leftmost and rightmost
	//
	// Why that must be a unique key?
	// suppose cmp_res is true and not case2,
	// then we have a inequality: Predecessor(p) <= val < p.val,
	// if not val >= predecessor(p), we can't get val < p.val(proof by contradiction)
	// if key_comp(pre.val, val) is false indicates that exists a value = val,
	// Therefore, pre.val < val can prove val is unique,
	// that is, there is a unique "hole" in the sorted sequence.
  typedef zstl::pair<BasePtr, BasePtr> Res;

	if(cmp_res){ // <1>
		// there are two cases:
		// 1) y is header(only when tree is empty)
		// 2) y is real leftmost
		// but can return the same result
		// @see comments of EmplaceAux()
		if(y == LeftMost())
			return Res{ x, y };
		else
			--pre;
	}

	assert(!x);

	// if cmp_res is false and pre is header(i.e. end())
	// we should exclude it, and also insert it, beacuse it have not key field
	if (impl_.key_compare_(_Key(pre.node_), key)) { // <2>
    // @note 
    // we don't reset root here, just forward RBTreeInsertAndFixup()
    // it will use the y to determine whether reset root
		return Res{ cmp_res ? y : x, y };
  }

	// return nullptr
	//  -- for insert, don't create new node
	//  -- for emplace, drop created node
	return Res{ pre.node_, nullptr };

}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
auto RBTree<K, V, GK, CP, Alloc>::GetInsertEqualPos(key_type const& key)
-> pair<BasePtr, BasePtr> {
	auto y = Header();
	auto x = Root();

	while(x){
		y = x;
		x = impl_.key_compare_(key, _Key(x)) ? x->left : x->right;
	}

	return make_pair(x, y);
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
auto RBTree<K, V, GK, CP, Alloc>::GetInsertUniqueHintPos(
	const_iterator pos_,
  key_type const& key)
-> pair<BasePtr, BasePtr> {
	auto pos = pos_.ConstCast();
  
  typedef zstl::pair<BasePtr, BasePtr> Res;

	// FIXME make a end_iterator to insert sorted element may be useful
	if(pos == end()) {
		// >= rightmost
		if(size() > 0 &&
			impl_.key_compare_(_Key(RightMost()), key)) {
			return Res{ nullptr, RightMost() };
		} else {
			return GetInsertUniquePos(key);
		}
	} else if (impl_.key_compare_(key, GK()(*pos))) {
    // compare with predesessor of pos,
    // if pre(pos) < key < pos, there a empty hole, to fill it.
    // if pos is begin()(pre(pos) must be not exists), just insert into left of leftmost to be new begin()
		auto before = pos;
		if(pos == begin()) {
			return Res{ LeftMost(), LeftMost() };
		} else if(impl_.key_compare_(GK()(*(--before)), key)) {
			// rightmost in left subtree
			if(before.node_->right == nullptr) {
				// insert into right hole of the rightmost
				return Res{ nullptr, before.node_ };
			}
			// the least ancestor whose right child is also an ancestor
			// insert into left of pos, it must be null
			else {
				return Res{ pos.node_, pos.node_ };
			}
		} else {
			return GetInsertUniquePos(key);
		}
	// symmetic case
	} else if (impl_.key_compare_(GK()(*pos), key)) {
		auto after = pos;
		if(pos.node_ == RightMost()) {
			return Res{ nullptr, RightMost() };
		}
		else if(impl_.key_compare_(key, GK()(*(++after)))) {
			// after is leftmost of right subtree
			if(after.node_->left == nullptr) {
				return Res{ after.node_, after.node_ };
			} else {
				// predecessor is an ancestor
				// insert right hole of pos, it must be null
				return Res{ nullptr, pos.node_ };
			}
		}else {
			return GetInsertUniquePos(key);
		}
	} else {
		// key equal pos
    // second is nullptr indicate there are have equal key element in tree
		return Res{ pos.node_, nullptr };
	}
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
auto RBTree<K, V, GK, CP, Alloc>::GetInsertEqualHintPos(
	const_iterator pos_, 
	key_type const& key) 
-> pair<BasePtr, BasePtr> {
	auto pos = pos_.ConstCast();

	typedef zstl::pair<BasePtr, BasePtr> Res;
  
  // the same logic as GetInsertEqualHintPos(),
  // but it allow same key value and no need to return nullptr to indicate failure
	if(pos == end()) {
		if(size() > 0
				&& !impl_.key_compare_(key, _Key(RightMost())))
			return Res{ nullptr, RightMost() };
		else
			return GetInsertEqualPos(key);
	}
  // key <= pos
	else if (!impl_.key_compare_(GK()(*pos), key)) {
		auto before = pos;
		
		if(before == begin())
			return Res{ LeftMost(), LeftMost() };
		else if(!impl_.key_compare_(key, GK()(*(--before)))) {
			if(before.node_->right == nullptr)
				return Res{ nullptr, before.node_ };
			else
				return Res{ pos.node_, pos.node_ };
		}else
			return GetInsertEqualPos(key);
	}
  // key >= pos
	else if (!impl_.key_compare_(key, GK()(*pos))) {
		auto after = pos;

		if(after.node_ == RightMost())
      return Res{ nullptr, RightMost() };
		else if(!impl_.key_compare_(GK()(*(++after)), key)) {
			if(after.node_->left == nullptr)
        return Res{ after.node_, after.node_ };
			else
        return Res{ nullptr, pos.node_ };
		}else
      return GetInsertEqualPos(key);
	}
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::EraseAux(const_iterator pos) noexcept {
	auto node = static_cast<LinkType>(
      RBTreeEraseAndFixup(const_cast<BasePtr>(pos.node_),
													Root(), LeftMost(), RightMost()));

	DropNode(node);
	--impl_.node_count;
}

#ifdef RBTREE_DEBUG
template<typename K, typename V, typename GK, typename CP, typename Alloc>
pair<bool, const char*> RBTree<K, V, GK, CP, Alloc>::IsRequired() const {
	if(size() == 0)
		return make_pair(begin() == end() && Header()->left == Header() 
			&& Header()->right == Header() && Header()->parent == nullptr, "Header()'s invariant broken when node_count == 0");

	int bh = BH(LeftMost(), Root());

	for(auto it = begin(); it != end(); ++it){
		auto cur = it.node();
		auto left = Left(cur);
		auto right = Right(cur);

		if(cur->color == RBTreeColor::Red){
			if((left && left->color == RBTreeColor::Red)
				|| (right && right->color == RBTreeColor::Red))
				return make_pair(false, "two red closed");
		}

		if(left && !key_compare()(_Key(left), _Key(cur)))
		{
			char buf[128];
			sprintf(buf, "left: %d and cur: %d isn't required", _Key(left), _Key(cur));
			return make_pair<bool, const char*>(false, buf);
		}
		
		if(right && !key_compare()(_Key(cur), _Key(right)))
		{
			char buf[128];
			sprintf(buf, "cur: %d and right: %d isn't required", _Key(cur), _Key(right));
			return make_pair<bool, const char*>(false, buf);
		}
		if(!left && !right && BH(cur, Root()) != bh)
			return make_pair(false, "black height isn't equal"); 
	}

	if(RightMost() != Maximum(Root()))
		return make_pair(false, "right most isn't maximum");
	if(LeftMost() != Minimum(Root()))
		return make_pair(false, "left most isn't minimum");

	return make_pair(true, "");
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::PrintRoot(ConstLinkType root, std::ostream& os) const {
	os << root->val << ((root->color == RBTreeColor::Red) ? "(Red)" : "(Black)") << '\n';
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::PrintSubTree(ConstLinkType root, std::ostream& os, std::string const& prefix) const {
    if(! root) return ;

    bool has_right = root->right;
    bool has_left = root->left;
	
	if(! has_right && ! has_left) return ;
	
	os << prefix;
    if(has_right && has_left)
        os << "├── ";
    if(has_right && ! has_left)
        os << "└── ";

    if(has_right){
		PrintRoot(root->Right(), os);
        if(has_left && (root->right->right || root->right->left))
            PrintSubTree(root->Right(), os, prefix + "|   ");
        else
            PrintSubTree(root->Right(), os, prefix + "    ");
    }

    if(has_left){
        os << ((has_right) ? prefix : "") << "└───";
		PrintRoot(root->Left(), os);
        PrintSubTree(root->Left(), os, prefix + "    "); 
    }
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::Print(std::ostream& os) const {
    if(!Root()) return ;

	auto root = static_cast<ConstLinkType>(Root()); 
	PrintRoot(root, os);
    PrintSubTree(root, os);
}

#endif

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename Policy>
typename RBTree<K, V, GK, CP, Alloc>::LinkType
RBTree<K, V, GK, CP, Alloc>::Copy(LinkType x, BasePtr p, Policy& policy){
  auto top = CloneNode(Value(x), policy);
  top->parent = p;

  STL_TRY{
    //because rbtree is also a binary tree, so copy one direction in recursion
    //just handle one direction
    if(x->right)
      top->right = Copy(Right(x), top, policy);
    p = top;
    x = Left(x);
    
    // x  
    while(x){
      auto y = CloneNode(Value(x), policy);
      p->left = y;
      y->parent = p;

      if(x->right)
        y->right = Copy(Right(x), y, policy);
      p = y;
      x = Left(x);
    }
  } CATCH_ALL {
    erase(top);
  }

  return top;
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::Erase(LinkType root) noexcept {
	//no rebalance
	while(root){
    // erase right subtree on the left child line
		Erase(Right(root));
		auto y = Left(root);
		DropNode(root);
		root = y;
	}
}


}//namespace zstl

#endif //_ZXY_ZSTL_STL_TREE
