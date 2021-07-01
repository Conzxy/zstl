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
#include "stl_move.h"
#include "stl_iterator.h"
#include "stl_type_traits.h"
#include "allocator.h"
#include "stl_exception.h"
#include "stl_utility.h"

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

#if __cplusplus > 201103L
	KeyCompare(KeyCompare const&) = default;
	
	KeyCompare(KeyCompare&& cmp)
		: key_compare_(TinySTL::move(cmp.key_comapre_))
	{ }
#endif
};

/*
 * @brief Red-Black rebalance alghorithm for insert
 * @param x inserted new node
 * @param root root of rbtree
 * @return void
 * @see https://conzxy.github.io/2021/01/26/CLRS/Search-Tree/BlackRedTree/
 */
void RBTreeInsertFixup(RBTreeBaseNode* x, RBTreeBaseNode*& root);

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
constexpr std::size_t BH(RBTreeBaseNode const* node, RBTreeBaseNode const* root) noexcept {
	if(!node)
		return 0;
	
	std::size_t bh = node->color == RBTreeColor::Black ? 1 : 0;
	if(node == root)
		return bh - 1;
	else
		return bh + BH(node->parent, root);
}

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
			::new(ptr) RBTreeNode<Val>();
			AllocTraits::construct(GetNodeAllocator(), &ptr->val,
				TinySTL::forward<Args>(args)...);
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

#if __cpluscplus > 201103L
		RBTreeImpl(RBTreeImpl&& other) = default;
#endif
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

	RBTree() = default;

	//TODO:copy

	/*
	 * @brief Insert new node whose value is given value
	 * @param val : given value
	 * @return a pair value, 
	 * the first value is iterator indicate inserted location
	 * and the second value is a boolean if insert successfully
	 */
	template<typename VT>
	pair<iterator, bool> InsertUnique(VT&& val);
	template<typename VT>
	iterator InsertEqual(VT&& val);
	
	template<typename ...Args>
	pair<iterator, bool> EmplaceUnique(Args&&... args);
	template<typename ...Args>
	iterator EmplaceEqual(Args&&... args);
	
	iterator erase(const_iterator pos){
		assert(pos != end());

		EraseAux(pos);
		return Next_Iter(pos).ConstCast();
	}
	
	void erase(const_iterator first, const_iterator last){
		for(auto it = begin(); it != end(); ++it)
			erase(it);

		assert(size() == 0 && begin() == end());
	}

private:
	template<typename VT>
	iterator InsertAux(BasePtr cur, BasePtr p, VT&& val);
	
	void EraseAux(const_iterator node) noexcept;

	//debug helper
#ifdef RBTREE_DEBUG
	pair<bool, const char*> IsRequired() const;
	void PrintRoot(ConstLinkType root,  std::ostream& os) const;
	void PrintSubTree(ConstLinkType root, std::ostream& os, std::string const& prefix = "") const;
	void Print(std::ostream& os = std::cout) const;
	
#endif

protected:
	RBTreeImpl impl_;
};

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename VT>
pair<typename RBTree<K, V, GK, CP, Alloc>::iterator, bool>
RBTree<K, V, GK, CP, Alloc>::InsertUnique(VT&& val){
	BasePtr p = Header();
	BasePtr cur = Root();

	bool cmp_res = false;

	while(cur){
		p = cur;
		cmp_res = key_comp()(GK()(val), _Key(cur));
		cur = cmp_res ? cur->left : cur->right;	
	}

	iterator pre(p);	//store the predecessor of p

	if(cmp_res){
		if(p == LeftMost())
			return make_pair(InsertAux(cur, p, TinySTL::forward<VT>(val)), true);
		else
			--pre;
	}

	//Predecessor(p) < val < p.val
	//if key_comp(pre.val, val) is false indicates that exists a value = val
	//becase predecessor is just less than p.val
	//only pre.val < val prove val is unique
	//that is, there is a "hole" in the sorted sequence
	if(key_comp()(_Key(pre.node_), GK()(val)))
		return make_pair(InsertAux(cur, p, TinySTL::forward<VT>(val)), true);
	else
		return make_pair(pre, false);
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename VT>
typename RBTree<K, V, GK, CP, Alloc>::iterator 
RBTree<K, V, GK, CP, Alloc>::InsertEqual(VT&& val){
	//LinkType p = static_cast<LinkType>(Header());
	//LinkType cur = static_cast<LinkType>(Root());
	auto p = Header();
	auto cur = Root();

	while(cur){
		p = cur;
		cur = key_comp()(GK()(val), _Key(cur)) ? cur->left : cur->right;
	}
	
	return InsertAux(cur, p, TinySTL::forward<VT>(val));
}

template<typename K, typename V, typename GK, typename CP, typename Alloc>
template<typename VT>
typename RBTree<K, V, GK, CP, Alloc>::iterator
RBTree<K, V, GK, CP, Alloc>::InsertAux(BasePtr cur, BasePtr p, VT&& val){
	LinkType new_node = CreateNode(TinySTL::forward<VT>(val));
	new_node->color = RBTreeColor::Red;
	new_node->left = nullptr;
	new_node->right = nullptr;
	
	assert(cur == nullptr);
	if(p == Header() || key_comp()(GK()(val), _Key(p))){
		p->left = new_node;

		if(p == Header()){
			Root() = new_node;
			RightMost() = new_node;
		}
		else if(p == LeftMost()){
			LeftMost() = new_node;
		}
		//printf("%d\n", val);
		//printf("header insert\n");
	}else{
		p->right = new_node;
		if(p == RightMost())
			RightMost() = new_node;
	}

	new_node->parent = p;
	++impl_.node_count;

	RBTreeInsertFixup(new_node, Root());

	return iterator(new_node);
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

}//namespace TinySTL

#endif //_ZXY_TINYSTL_STL_TREE
