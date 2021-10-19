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
#include <cstring>
#include "stl_move.h"
#include "stl_iterator.h"
#include "type_traits.h"
#include "allocator.h"
#include "stl_exception.h"
#include "stl_utility.h"
#include "stl_algobase.h"
#include "config.h"

#ifdef RBTREE_DEBUG
#include <iostream>
#endif

#ifdef RBTREE_DEBUG
template<typename >
class RBTreeTest;
#endif

namespace TinySTL{

/*
 * @enum RBTreeColor
 * @brief provide color of node : red, black
 * satifies the property: every node either red or black
 */
enum class RBTreeColor : bool {
	Red = true, Black = false
};

/*
 * @struct RBTreeBaseNode
 * @brief base RBTree node type
 * there are two advantage:
 * (1) RBTree header no need value field, so base is better than actual node type
 * (2) unqualified type, so related algorithm can reuse for all type
 * e.g. RBTree<Val=int> and RBTree<Val=char>， their algorithm is same
 */
struct RBTreeBaseNode{
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

/*
 * @struct RBTreeNode
 * @tparam Val : value type
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

protected:
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

/* @fn
 * @brief find the successor and predecessor of given node
 * the detail implementation of iterator operator++ and operator--
 * @see https://conzxy.github.io/2021/01/25/CLRS/Search-Tree/BinarySearchTree/
 */
RBTreeBaseNode* RBTreeIncrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeIncrement(RBTreeBaseNode const* node) noexcept;
RBTreeBaseNode* RBTreeDecrement(RBTreeBaseNode* node) noexcept;
RBTreeBaseNode const* RBTreeDecrement(RBTreeBaseNode const* node) noexcept;

/*
 * @struct RBTreeIterator
 * @tparam T : value type
 * @note begin() = header.left, end() = header
 */
template<typename T>
struct RBTreeIterator{
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
		return lhs.node != rhs.node_;	//!(lhs == rhs)
	}

	BasePtr node_;
};

/*
 * @struct RBTreeConstIterator
 * @tparam T : value type
 * @note RBTreeIterator can convert into RBTreeConstIterator
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

/*
 * @struct KeyCompare
 * @tparam Compare : functor used to compare two key
 * @deprecated Compare is linkly a empty type
 */
template<typename Compare>
struct KeyCompare{
	Compare key_compare_;

	KeyCompare()
	{ }

	explicit KeyCompare(Compare const& cmp)
		: key_compare_(cmp)
	{ }

#if __cplusplus >= 201103L
	KeyCompare(KeyCompare const&) = default;
	
	KeyCompare(KeyCompare&& cmp)
		: key_compare_(TinySTL::move(cmp.key_comapre_))
	{ }
#endif
};

/*
 * @brief relink inserted node and its parent and fixup to maintain red-black property
 * @param insert_left indicate if insert left(i.e. left must be null)
 * @param x inserted node
 * @param p parents of x
 * @param header header sentinel pointing to leftmost, rightmost and root
 */
void RBTreeInsertAndFixup(const bool insert_left,
		RBTreeBaseNode* x,
		RBTreeBaseNode* p,
		RBTreeBaseNode* header) noexcept;

/*
 * @brief Red-Black rebalance algorithm for delete
 * @param x node to be deleted
 * @param root root of rbtree
 * @param leftmost the most left node of rbtree
 * @param rightmost the most right node of rbtree
 * @return actual deleted node location(i.e. x)
 * @note the reason why return x instead of void is that delete RBTreeBaseNode is imcompleted
 * because RBTreeBaseNode destructor is non-virtual
 * @see https://conzxy.github.io/2021/01/26/CLRS/Search-Tree/BlackRedTree/
 */
RBTreeBaseNode* 
RBTreeEraseAndFixup(RBTreeBaseNode* x, RBTreeBaseNode*& root
									 , RBTreeBaseNode*& leftmost
									 , RBTreeBaseNode*& rightmost);

/*
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
	using NodeAllocator = typename Alloc::template rebind<RBTreeNode<Val>>;
	using AllocTraits = allocator_traits<NodeAllocator>;

protected:
	using BasePtr = RBTreeBaseNode*;
	using ConstBasePtr = RBTreeBaseNode const*;
	using LinkType = RBTreeNode<Val>*;
	using ConstLinkType = RBTreeNode<Val> const*;

public:
	/*
	 * @class ReuseOrAlloc
	 * @brief reuse original nodes, destroy and reconstruct
	 * when no nodes, plain alloc
	 */
	class ReuseOrAlloc{
	public:
		explicit ReuseOrAlloc(RBTree& tree)
			: root_(tree.Root())
			, nodes_(tree.RightMost())
			, tree_(tree)
			  
		{
			if(root_){
				root_->parent = nullptr;
				if(nodes_->left)
					nodes_ = nodes_->left;
			}else
				nodes_ = nullptr;
		}

#if __cpluscplus > 201103L
		ReuseOrAlloc(ReuseOrAlloc const&) = delete;
#endif

		~ReuseOrAlloc(){
#ifdef RBTREE_DEBUG
		printf("reused node count: %lu\n", count_);
#endif
			tree_.Erase(static_cast<LinkType>(root_));
		}
		
		template<typename ...Args>
		LinkType operator()(Args&&... args){
			auto node = static_cast<LinkType>(Extract());

			if(node){
#ifdef RBTREE_DEBUG
				++count_;
#endif
				tree_.DestroyNode(node);
				tree_.ConstructNode(node, STL_FORWARD(Args, args)...);
				return node;
			}else
				return tree_.CreateNode(STL_FORWARD(Args, args)...);
		}

	private:
		BasePtr Extract() noexcept {
			if(! nodes_)
				return nodes_;
			
			auto node = nodes_;
			nodes_ = nodes_->parent;

			if(nodes_){
				if(nodes_->right == node){
					nodes_->right = nullptr;

					if(nodes_->left){
						nodes_ = nodes_->left;

						while(nodes_->right)
							nodes_ = nodes_->right;

						if(nodes_->left)
							nodes_ = nodes_->left;
					}
				}else
					nodes_->left = nullptr;
			}else{
				root_ = nullptr;
			}

			return node;
		}

		BasePtr root_;
		BasePtr nodes_;
		RBTree& tree_;

#ifdef RBTREE_DEBUG
		size_t count_ = 0;
#endif
	};

	/*
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
		ConstructNode(new_node, TinySTL::forward<Args>(args)...);
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

	/*
	 * @class RBTreeImpl
	 * @brief detail field of RBTree
	 * @note use a technique : EBCO
	 */
	struct RBTreeImpl
	: public NodeAllocator
	, public key_compare
	, public RBTreeHeader{
		RBTreeImpl()
		 : NodeAllocator()
		{ }

		RBTreeImpl(NodeAllocator&& alloc)
		 : NodeAllocator(TinySTL::move(alloc))
		{ }

		RBTreeImpl(key_compare const& cmp)
		 : key_compare(cmp)
		{ }

		RBTreeImpl(RBTreeImpl const&) = default;
#if __cpluscplus > 201103L
		RBTreeImpl(RBTreeImpl&&) = default;
#endif

		using RBTreeHeader::Reset;
	};

#ifdef RBTREE_DEBUG
	template<typename >
	friend class ::RBTreeTest;
#endif

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
	using reverse_iterator = TinySTL::reverse_iterator<iterator>;
	using const_reverse_iterator = TinySTL::reverse_iterator<const_iterator>;

	Compare key_comp() const noexcept {
		return Compare();
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
		if(!Root())
			Erase(static_cast<LinkType>(Root()));
	}

	RBTree(RBTree const& rhs) noexcept
	{
		Copy(rhs);
	}
	
	RBTree(RBTree&& rhs)
		: impl_(TinySTL::move(rhs.impl_))
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
			impl_.impl_ = TinySTL::move(rhs.impl_);
			rhs.Header()->Reset();

			return *this;
		}
	}

	void swap(RBTree const& rhs) noexcept {
		using TinySTL::swap;
		swap(this->impl_, rhs.impl_);
	}

	///////////////////////////////
	////////INSERT_MODULE//////////
	///////////////////////////////
	/*
	 * @brief Insert new node whose value is given value
	 * @param val : given value
	 * @return a pair value, 
	 * the first value is iterator indicate inserted location
	 * and the second value is a boolean if insert successfully
	 */
	template<typename VT>
	pair<iterator, bool> InsertUnique(VT&& val)
	{ return EmplaceUnique(STL_FORWARD(VT, val)); }

	template<typename VT>
	iterator InsertEqual(VT&& val)
	{ return EmplaceEqual(STL_FORWARD(VT, val)); }
	
	template<typename ...Args>
	pair<iterator, bool> EmplaceUnique(Args&&... args) {
		return InsertUniqueNoAlloc(CreateNode(STL_FORWARD(Args, args)...));
	}

	template<typename ...Args>
	iterator EmplaceEqual(Args&&... args) {
		return InsertEqualNoAlloc(CreateNode(STL_FORWARD(Args, args)...));
	}
	
	template<typename Arg>
	pair<iterator, bool> InsertHintUnique(const_iterator pos, Arg&& arg)
	{ return EmplaceUnique(pos, STL_FORWARD(Arg, arg)); }

	template<typename Arg>
	iterator InsertHintEqual(const_iterator pos, Arg&& arg)
	{ return EmplaceEqual(pos, STL_FORWARD(Arg, arg)); }

	template<typename ...Args>
	pair<iterator, bool> EmplaceHintUnique(const_iterator pos,
											Args&&... args);

	template<typename ...Args>
	iterator EmplaceHintEqual(const_iterator pos,
							Args&&... args);

	//////////////////////////////
	/////////ERASE MODULE/////////
	//////////////////////////////
	
	iterator erase(const_iterator pos){
		assert(pos != end());

#ifdef RBTREE_DEBUG
		printf("erase node: %d\n", *pos);
		Print();
		TEST_RB_PROPERTY((*this));
#endif
		auto next = Next_Iter(pos).ConstCast();
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
	/*
	 * @brief find given key if in RBTree
	 * @exception Compare object may throw
	 * @return not found return end() otherwise return iterator which indicate key location
	 */

	// FIXME: find and some algorithm have logic problem
	const_iterator find(Key const& key) const{
		auto y = Header(); //last node which is not less than key
		auto x = Root(); //current node

		while(x){
			//x >= key
			if(!key_comp()(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		const_iterator j(y);
		return (j == end() || key_comp()(key, _Key(y))) ?
				end() : j;
	}

	iterator find(Key const& key){
		auto y = Header();
		auto x = Root();

		while(x){
			if(!key_comp()(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		iterator j(y);
		return (j = end() || key_comp()(key, _Key(y))) ?
				end() : j;
	}
	
	/*
	 * @brief wrap find, just predicate if contains given key
	 */
	bool contains(Key const& k) const {
		return find(k) != end();
	}
	
	/*
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
			if(key_comp()(key, _Key(x))){
				y = x;
				x = x->right;
			}
			else x = x->left;
		}

		return y;
	}
	
	/*
	 * @breif return an iterator pointing to the first 
	 * element that is not less than given key
	 */
	iterator lower_bound(Key const& key){
		auto y = Header();
		auto x = Root();

		while(x){
			if(key_comp()(_Key(x), key)){
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
			if(key_comp()(_Key(x), key)){
				y = x;
				x = x->left;
			}
			else x = x->right;
		}

		return y;
	}

	/*
	 * @brief return a range containing all elements
	 * with the given key in the container
	 */
	pair<iterator, iterator> equal_range(Key const& key){
		return make_pair(lower_bound(), upper_bound());
	}

	pair<const_iterator, const_iterator> equal_range(Key const& key) const {
		return make_pair(lower_bound(), upper_bound());
	}

	/*
	 * @brief the number of elements with key
	 */
	size_type count(Key const& key) const {
		auto range = equal_range(key);
		return distance(range.first, range.second);
	}


private:
	///////////////////
	/////INSERT_AUX////
	///////////////////
	/*
	 * @brief relink new_node and parent
	 * @param cur indicate right if null
	 * @return new_node position
	 */
	iterator InsertAux(LinkType new_node, BasePtr cur, BasePtr p){
		bool insert_left = cur || p == Header() ||
				key_comp()(_Key(new_node), _Key(p));
		
		RBTreeInsertAndFixup(insert_left, new_node, p, Header());
		++impl_.node_count;

		return new_node;
	}
	
	pair<iterator, bool> InsertUniqueNoAlloc(LinkType new_node) noexcept;
	iterator InsertEqualNoAlloc(LinkType new_node) noexcept;

	////////////////
	/////COPY///////
	////////////////
	template<typename Policy>
	LinkType Copy(LinkType x, BasePtr p, Policy& policy);

	template<typename Policy>
	void Copy(RBTree const& rb, Policy& policy){
		if(!rb.Root()) return ;

		Root() = Copy(static_cast<LinkType>(const_cast<BasePtr>(rb.Root())), Header(), policy);
		LeftMost() = Minimum(Root());
		RightMost() = Maximum(Root());
		impl_.node_count = rb.impl_.node_count;
	}

	void Copy(RBTree const& rb){
		PlainAlloc policy(*this);
		Copy(rb, policy);
	}

	////////////////////
	//////ERASE AUX/////
	////////////////////
	/*
	 * @brief erase all node from root top-down
	 * @complexity O(h^2) = O((lgn)^2)
	 */
	void Erase(LinkType root) noexcept;
	void EraseAux(const_iterator node) noexcept;

	/////////////////////
	/////DEBUG HELPER////
	/////////////////////
#ifdef RBTREE_DEBUG
	/*
	 * @brief verify RBTree if requires red-black property
	 * @return a pair, the first value is a boolean indicate if requred, and the second indicate violated details
	 */
	pair<bool, const char*> IsRequired() const;
	
	void PrintRoot(ConstLinkType root,  std::ostream& os) const;
	void PrintSubTree(ConstLinkType root, std::ostream& os, std::string const& prefix = "") const;
	/*
	 * @brief print RBTree as a directory tree
	 */
	void Print(std::ostream& os = std::cout) const;
	
#endif


	friend inline bool operator==(RBTree const& lhs, RBTree const& rhs) noexcept {
		return lhs.size() == rhs.size()
			&& TinySTL::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	
	friend inline bool operator!=(RBTree const& lhs, RBTree const& rhs) noexcept {
		return !(lhs == rhs);
	}

	friend inline bool operator<(RBTree const& lhs, RBTree const& rhs) noexcept {
		return TinySTL::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin());
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

template<typename K, typename V, typename GK, typename CP, typename Alloc>
inline void swap(RBTree<K, V, GK, CP, Alloc> const& lhs, RBTree<K, V, GK, CP, Alloc> const& rhs) noexcept {
	return lhs.swap(rhs);
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
pair<typename RBTree<K, V, GK, CP, Alloc>::iterator, bool>
RBTree<K, V, GK, CP, Alloc>::InsertUniqueNoAlloc(LinkType new_node) noexcept {
	auto y = Header();
	auto x = Root();
	
	bool cmp_res = false;
	while(x){
		y = x;
		cmp_res = key_comp()(_Key(new_node), _Key(x));
		x = cmp_res ? x->left : x->right;
	}

	iterator pre(y);

	//Predecessor(p) < val < p.val
	//if key_comp(pre.val, val) is false indicates that exists a value = val
	//becase predecessor is just less than p.val
	//only pre.val < val prove val is unique
	//that is, there is a "hole" in the sorted sequence
	if(cmp_res){
		if(y == LeftMost())
			return make_pair(InsertAux(new_node, x, y), true);
		else
			--pre;
	}

	if(key_comp()(_Key(pre.node_), _Key(new_node)))
		return make_pair(InsertAux(new_node, x, y), true);
	else {
		DropNode(new_node);
		return make_pair(pre, false);
	}

}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
typename RBTree<K, V, GK, CP, Alloc>::iterator
RBTree<K, V, GK, CP, Alloc>::InsertEqualNoAlloc(LinkType new_node) noexcept {
	auto y = Header();
	auto x = Root();

	while(x){
		y = x;
		x = key_comp()(_Key(new_node), _Key(x)) ? x->left : x->right;
	}

	return InsertAux(new_node, x, y);
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename ...Args>
pair<typename RBTree<K, V, GK, CP, Alloc>::iterator, bool>
RBTree<K, V, GK, CP, Alloc>::EmplaceHintUnique(const_iterator pos_,
												Args&&... args)	{
	auto pos = pos_.ConstCast();
	auto new_node = CreateNode(STL_FORWARD(Args, args)...);

	STL_TRY{
		if(pos == end()) {
			if(size() > 0 
					&& key_comp()(_Key(new_node), _Key(RightMost())))
				return InsertAux(new_node, nullptr, RightMost());
			else
				return InsertUniqueNoAlloc(new_node);
		}
		else if (key_comp()(_Key(new_node), GK()(*pos))) {
			//first find before
			auto before = pos;
			if(pos == begin())
				return InsertAux(new_node, LeftMost(), LeftMost());
			else if(key_comp()(GK()(*(--before))), _Key(new_node)) {
				//pos pointing left subtree's rightmost or least ancestor whose right child is also ancestor
				if(before->right == nullptr)
					return InsertAux(new_node, nullptr, before.node_);
				else
					return InsertAux(new_node, pos.node_, pos.node_);
			}else
				return InsertUniqueNoAlloc(new_node);
		}else if (key_comp()(GK()(*pos), _Key(new_node))) {
			//first find after
			auto after = pos;
			if(pos.node_ == RightMost())
				return InsertAux(new_node, RightMost(), RightMost());
			else if(key_comp()(_Key(new_node), GK()(*(++after)))) {
				if(after->right == nullptr)
					return InsertAux(new_node, nullptr, pos.node_);
				else
					return InsertAux(new_node, after.node_, after.node_);
			}else
				return InsertUniqueNoAlloc(new_node);
		}else //key equal pos
			return make_pair(pos, false);
	}
	CATCH_ALL_UNWIND(DropNode(new_node))
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename ...Args>
typename RBTree<K, V, GK, CP, Alloc>::iterator
RBTree<K, V, GK, CP, Alloc>::EmplaceHintEqual(const_iterator pos_, Args&&... args) {
	auto pos = pos_.ConstCast();
	auto new_node = CreateNode(STL_FORWARD(Args, args)...);

	STL_TRY{
		if(pos == end()) {
			if(size() > 0
					&& !key_comp()(_Key(new_node), _Key(RightMost())))
				return InsertAux(new_node, nullptr, RightMost());
			else
				return InsertEqualNoAlloc(new_node);
		}
		else if (!key_comp()(GK()(*pos), _Key(new_node))) {
			auto before = pos;
			
			if(before == begin())
				return InsertAux(new_node, LeftMost(), LeftMost());
			else if(!key_comp()(_Key(new_node), GK()(*(--before)))) {
				if(before->right == nullptr)
					return InsertAux(new_node, nullptr, before.node_);
				else
					return InsertAux(new_node, pos.node_, pos.node_);
			}else
				return InsertEqualNoAlloc(new_node);
		}
		else if (!key_comp()(_key(new_node), GK()(*pos))) {
			auto after = pos;

			if(after.node_ == RightMost())
				return InsertAux(new_node, RightMost(), RightMost());
			else if(!key_comp()(GK()(*(++after)), _key(new_node))) {
				if(after->right == nullptr)
					return InsertAux(new_node, nullptr, pos.node_);
				else
					return InsertAux(new_node, after.node_, after.node_);
			}else
				return nullptr;
		}
	}
	CATCH_ALL_UNWIND(DropNode(new_node))
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::EraseAux(const_iterator pos) noexcept {
	auto node = static_cast<LinkType>(RBTreeEraseAndFixup(const_cast<BasePtr>(pos.node_),
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
		return make_pair(false, "left most isn't mimumum");

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

#endif //RBTREE_DEBUG

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
		
		while(x){
			auto y = CloneNode(Value(x), policy);
			p->left = y;
			y->parent = p;

			if(x->right)
				y->right = Copy(Right(x), y, policy);
			p = y;
			x = Left(x);
		}
	}
	CATCH_ALL_UNWIND(erase(top));

	return top;
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
void RBTree<K, V, GK, CP, Alloc>::Erase(LinkType root) noexcept {
	//no rebalance
	while(root){
		Erase(Right(root));
		auto y = Left(root);
		DropNode(root);
		root = y;
	}
}


}//namespace TinySTL

#endif //_ZXY_TINYSTL_STL_TREE
