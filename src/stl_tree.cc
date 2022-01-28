/**
 * @file stl_tree.cc
 * implemetation of Algorithm of RBTree
 * @author Conzxy
 * @date 28-6-2021
 */
#include "../include/stl_tree.h"

namespace zstl{

RBTreeBaseNode const* 
RBTreeIncrement(RBTreeBaseNode const* node) noexcept {
		// if right subtree is not empty
		// search the minimum of right subtree
		// otherwise, search the least ancestor of given node whose 
		// left child is also an ancestor
		if (node->right != nullptr){
				return RBTreeBaseNode::Minimum(node->right);
		} else {
				// if right of root is null
				// then header.left = header.right = node
				// so we should handle it explicitly
				auto p = node->parent;
				while (p->right == node){
						node = p;
						p = node->parent;
				}

				// if node = root, p = header
				// now, root is maximum, node = header, p = node
				if (node->right == p)
						return node;
				
				return p;
		}
}

RBTreeBaseNode* 
RBTreeIncrement(RBTreeBaseNode* node) noexcept {
		return const_cast<RBTreeBaseNode*>(
				RBTreeIncrement(static_cast<RBTreeBaseNode const*>(node))
		);
}

RBTreeBaseNode const* 
RBTreeDecrement(RBTreeBaseNode const* node) noexcept {
		// if node is header, should return leftmost
		// because node->parent->parent = node not only header but also root
		// set header.color to red is a implemetation trick
		if (node->parent->parent == node && node->color == RBTreeColor::Red)
				return node->right;
		
		if (node->left != nullptr)
				return RBTreeBaseNode::Maximum(node->left);
		else {
			auto p = node->parent;
			while (p->left == node) {
				node = p;
				p = node->parent;
			}

			// althrough node = root => p = root
			// we don't want return header that make begin() and begin() be a loop
			// so trivially return p
			return p;
		}
}

RBTreeBaseNode* 
RBTreeDecrement(RBTreeBaseNode* node) noexcept {
		return const_cast<RBTreeBaseNode*>(
				RBTreeDecrement(static_cast<RBTreeBaseNode const*>(node))
		);
}

/**
 * @brief x             y
 *         \    =>     /
 *          y         x
 * let the link edge of x and y be a "pivot",
 * rotate the pivot 90 degrees to the left, i.e. \ => /
 */
static void
LeftRotation(RBTreeBaseNode*& root, RBTreeBaseNode* x){
		//transparent subtree
		auto y = x->right;
		x->right = y->left;
		if(y->left != nullptr)
				y->left->parent = x;
		 
		//y and x's parent link
		y->parent = x->parent;
		if(x == root){
				root = y;
	}
		else if(x->parent->right == x)
				x->parent->right = y;
		else //x->parent->left == x
				x->parent->left = y;
		
		//x and y link
		y->left = x;
		x->parent = y;
}

/**
 * @brief   y       x
 *         /    =>   \
 *        x           y
 */
static void
RightRotation(RBTreeBaseNode*& root, RBTreeBaseNode* y){
		auto x = y->left;
		y->left = x->right;
		if(x->right != nullptr)
				x->right->parent = y;
		
		x->parent = y->parent;
		if(y == root){
				root = x;
		}else if(y->parent->left == y)
				y->parent->left = x;
		else
				y->parent->right = x;

		x->right = y;
		y->parent = x;
}

/**
 * @brief Red-Black rebalance alghorithm for insert
 * @param z inserted new node
 * @param root root of rbtree
 * @return void
 * @see https://conzxy.github.io/2021/01/26/CLRS/Search-Tree/BlackRedTree/
 */
static void 
RBTreeInsertFixup(RBTreeBaseNode* z, RBTreeBaseNode*& root){
		while(z->parent->color == RBTreeColor::Red &&
					z != root ) {
				// z is the left child 
				if(z->parent->parent->left == z->parent){
						auto uncle = z->parent->parent->right;
						//CASE1 : uncle's color is red
						//recolor uncle and parent, then new_node up by 2 level(grandpa)
						if(uncle && uncle->color == RBTreeColor::Red){
								z->parent->color = RBTreeColor::Black;
								uncle->color = RBTreeColor::Black;
								z->parent->parent->color = RBTreeColor::Red;
								z = z->parent->parent;
						}
						// if uncle is NULL, it is also NIL leaf whose color is black
						else{ 
								//uncle's color is BLACK
								//CASE2: parent right is new_node
								//now, grandpa, parent and new_node are not in one line
								//so left rotate parent make them in one change to CASE3
								if(z->parent->right == z){
										z = z->parent;
										LeftRotation(root, z);
								}

								//CASE3: parent left is new_node
								//just right rotate the grandpa, and recolor
								//that rebalance the RBTree
								z->parent->parent->color = RBTreeColor::Red;
								z->parent->color = RBTreeColor::Black;
								RightRotation(root, z->parent->parent);
						}
				}
				else{
						//symmetric cases
						auto uncle = z->parent->parent->left;
						if(uncle && uncle->color == RBTreeColor::Red){
								z->parent->color = RBTreeColor::Black;
								uncle->color = RBTreeColor::Black;
								z->parent->parent->color = RBTreeColor::Red;
								z = z->parent->parent;
						}           
						else{
								if(z->parent->left == z){
										z = z->parent;
										RightRotation(root, z);
								}

								z->parent->parent->color = RBTreeColor::Red;
								z->parent->color = RBTreeColor::Black;
								LeftRotation(root, z->parent->parent);
						}
				}//if(grandpa->left == parent)
		}//while

		// when case 1 up to root, recolor root to maintain property 2
		root->color = RBTreeColor::Black;
}

void RBTreeInsertAndFixup(
	const bool insert_left, 
	RBTreeBaseNode* x,
	RBTreeBaseNode* p,
	RBTreeBaseNode* header) noexcept {
	if(insert_left){
		p->left = x;
	
		// if p is header, update root
		// and set rightmost and leftmost
		if(p == header){
			header->parent = x;
			header->right = x;
		// if p is the leftmost, update it
		}else if(p == header->left){
			header->left = x;
		}
	}else{
		p->right = x;
		
		// if p is the rightmost, update it
		if(p == header->right)
			header->right = x;
	}

	x->parent = p;
	RBTreeInsertFixup(x, header->parent);
}

/**
 * @brief transplant the newnode to oldnode location
 * @param root the root of BST
 * @param newnode replace the oldnode
 * @param oldnode oldnode location
 * @note transplant just link the new_node and old_node's parent, don't break the old_node's parent and left/right
 */
static void 
Transplant(RBTreeBaseNode*& root, RBTreeBaseNode* oldnode, RBTreeBaseNode* newnode) noexcept {
	if(oldnode->parent->parent == oldnode 
	&& oldnode->color == RBTreeColor::Black){
		root = newnode;	
	}

	//link oldnode's parent and newnode
	if(oldnode->parent->right == oldnode){
		oldnode->parent->right = newnode;
	}else if(oldnode->parent->left == oldnode){
		oldnode->parent->left = newnode;
	}

	if(newnode){
		newnode->parent = oldnode->parent;
	}
}

/**
 * @brief fixup balance loss RBTree 
 * @param root the root of BST
 * @param x double black node(lose balance)
 * @return void
 */
static void 
RBTreeEraseFixup(
	RBTreeBaseNode* x, 
	RBTreeBaseNode* x_parent,
	RBTreeBaseNode*& root) {
	// If x is NULL, that is black leaf, also include
	while(x != root
		 && (!x || x->color == RBTreeColor::Black)){
		if (x_parent->left == x) {	//sibling in parent's right
			auto sibling = x_parent->right;
			//CASE1 : sibling's color is red
			// change to case 2 which sbling's color is black

			// ! sibling must not be NULL
			assert(sibling);

			if (sibling->color == RBTreeColor::Red) {
				x_parent->color = RBTreeColor::Red;
				sibling->color = RBTreeColor::Black;
				LeftRotation(root, x_parent);
			} else { //sibing's color is black
				//CASE2 : sibling's two children's color is black

				// ! the two child also can be black leaf,
				// ! that is, it may be NULL
				if((!sibling->right || sibling->right->color == RBTreeColor::Black)
				&& (!sibling->left || sibling->left->color == RBTreeColor::Black)){
					sibling->color = RBTreeColor::Red;
					x = x_parent;	//if x's parent's color is red, exit loop and recolor to black

					assert(x);
					x_parent = x->parent;
				} else {
					if(!sibling->right || sibling->right->color == RBTreeColor::Black){
						assert(sibling->left);
						assert(sibling->left->color == RBTreeColor::Red);
						// CASE3: sibling's left child's color is red, and right child's color is black

						// change to such case which the color of right child of brother is red
						sibling->left->color = RBTreeColor::Black; //sibling->color
						sibling->color = RBTreeColor::Red; //sibling->left->color
						RightRotation(root, sibling);
						sibling = x_parent->right;
					}
					// CASE4 : sibling's right child's color is red
					// left ratation parent and recolor
					sibling->color = x_parent->color;
					x_parent->color = RBTreeColor::Black;
					sibling->right->color = RBTreeColor::Black;
					LeftRotation(root, x_parent);
					x = root;
				} // if sibling's has two black child
			} // if sibling's color is red
		} // if parent->left = x
		else{//parent->right = x, i.e. sibling in left
			auto sibling = x_parent->left;

			assert(sibling);
			if(sibling->color == RBTreeColor::Red){
				x_parent->color = RBTreeColor::Red;
				sibling->color = RBTreeColor::Black;
				LeftRotation(root, x_parent);
			}else{
				if((!sibling->left || sibling->left->color == RBTreeColor::Black)
				&& (!sibling->right || sibling->right->color == RBTreeColor::Black)){
					sibling->color = RBTreeColor::Red;
					x = x_parent;
					assert(x);
					x_parent = x->parent;
				}else{
					assert(sibling->right);
					assert(sibling->right->color == RBTreeColor::Red);
					if(!sibling->left || sibling->left->color == RBTreeColor::Black){
						sibling->right->color = RBTreeColor::Black;
						sibling->color = RBTreeColor::Red;
						LeftRotation(root, sibling);
						sibling = x_parent->left;
					}
					sibling->color = x_parent->color;
					x_parent->color = RBTreeColor::Black;
					sibling->left->color = RBTreeColor::Black;
					RightRotation(root, x_parent);
					x = root;
				}
			}
		}
	}//while x != root and x->color == black
	if(x) 
		x->color = RBTreeColor::Black;
}

/**
 * @brief algorithm of deleting node in RBTree
 * @param root the root of RBTree
 * @param node that will be deleted
 * @return void
 */
RBTreeBaseNode* RBTreeEraseAndFixup(
	RBTreeBaseNode* z,
	RBTreeBaseNode*& root,
 	RBTreeBaseNode*& leftmost,
	RBTreeBaseNode*& rightmost) {
	auto y = z;
	auto y_old_color = y->color;
	//x_parent imitate black leaf(sentinel)'s parent because no actual leaf here(might be null)
	RBTreeBaseNode* x = nullptr;
	RBTreeBaseNode* x_parent = nullptr;

	//note: if z is root, then x to be a new root
	//this case no need to rebalance
	//because if y_old_color is red, just recolor to black
	//otherwise, no handler
	//in fact, only case1 and case2 might happen
	if(!z->left){ //z's left is null
		x = z->right; //x migth be null

		Transplant(root, z, x);

		if(z == leftmost){
			//z->left must be null
			if(z->right)
				leftmost = RBTreeBaseNode::Minimum(z->right);
			else{//two null child
				leftmost = z->parent;
			}
		}

		x_parent = z->parent;
	}else if(!z->right){
		x = z->left; //x must not be null
		
		Transplant(root, z, x);
		if(z == rightmost)
			rightmost = RBTreeBaseNode::Maximum(z->left);

		x_parent = z->parent;
	}else{		//two child
		y = RBTreeBaseNode::Minimum(z->right);
		//y's left child must be null
		y_old_color = y->color;
	
		x = y->right;	//x might be null
		if(y == z->right){
			x_parent = y;
		}else{
			Transplant(root, y, x);
			//becase y no left child,
			//no transfer subtree after transplant
			y->right = z->right;
			z->right->parent = y;

			x_parent = y->parent;
		}

		Transplant(root, z, y);
		y->left = z->left;
		z->left->parent = y;
		y->color = z->color;
	}
	
	if(y_old_color == RBTreeColor::Black)
		RBTreeEraseFixup(x, x_parent, root);

	return z;
}


}//namespace zstl
