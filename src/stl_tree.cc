/*
 * @file stl_tree.cc
 * implemetation of Algorithm of RBTree
 * @author Conzxy
 * @date 28-6-2021
 */

#include "../include/stl_tree.h"

namespace TinySTL{

RBTreeBaseNode const* RBTreeIncrement(RBTreeBaseNode const* node) noexcept {
    //if right subtree is not empty
    //search the minimum of right subtree
    //otherwise, search the least ancestor of given node whose 
    //left child is also an ancestor
    if(node->right != nullptr){
        return RBTreeBaseNode::Minimum(node->right);
    }else{
        auto p = node->parent;
        while(p->right == node){
            node = p;
            p = node->parent;
        }

        //if node = root, p = header
        //now, root is maximum, node = header
        if(node->right == p)
            return node;
        
        return p;
    }
}

RBTreeBaseNode* RBTreeIncrement(RBTreeBaseNode* node) noexcept {
    return const_cast<RBTreeBaseNode*>(
        RBTreeIncrement(static_cast<RBTreeBaseNode const*>(node))
    );
}

RBTreeBaseNode const* RBTreeDecrement(RBTreeBaseNode const* node) noexcept {
    //if node is header, should return leftmost
    //because node->parent->parent = node not only header but also root
    //set header.color to red is a implemetation trick
    if(node->parent->parent == node && node->color == RBTreeColor::Red)
        return node->right;
    
    if(node->left != nullptr)
        return RBTreeBaseNode::Maximum(node->left);
    else{
        auto p = node->parent;
        while(p->left == node){
            node = p;
            p = node->parent;
        }

        //althrough node = root => p = root
        //we don't want return header that make begin() and begin() be a loop
        //so trivially return p
        return p;
    }
}

RBTreeBaseNode* RBTreeDecrement(RBTreeBaseNode* node) noexcept {
    return const_cast<RBTreeBaseNode*>(
        RBTreeDecrement(static_cast<RBTreeBaseNode const*>(node))
    );
}

/*
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
    if(x->parent == root->parent){
        root->parent->parent = y;
        root = y;
    }
    else if(x->parent->right == x)
        x->parent->right = y;
    else //x->parent->left == x
        x->parent->left = y;
    
    //x and y link
    x->parent = y;
    y->left = x;
}

/*
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
    if(y->parent == root->parent){
        root->parent->parent = x;
        root = x;
    }

    else if(y->parent->left == y)
        y->parent->left = x;
    else
        y->parent->right = x;

    y->parent = x;
    x->right = y;
}

void RBTreeInsertFixup(RBTreeBaseNode* new_node, RBTreeBaseNode*& root){
    while(new_node->parent->color == RBTreeColor::Red 
        && new_node != root){
        auto parent = new_node->parent;
        auto grandpa = parent->parent;

        if(grandpa->left == parent){
            auto uncle = grandpa->right;
            //CASE1 : uncle's color is red
            //recolor uncle and parent, then new_node up by 2 level(grandpa)
            if(uncle->color == RBTreeColor::Red){
                parent->color = RBTreeColor::Red;
                uncle->color = RBTreeColor::Red;
                new_node = grandpa;
            }
            else{ //uncle's color is BLACK
                //CASE2: parent right is new_node
                //now, grandpa, parent and new_node are not in one line
                //so left rotate parent make them in one change to CASE3
                if(parent->right == new_node){
                    LeftRotation(root, parent);
                    new_node = new_node->left;
                }

                //CASE3: parent left is new_node
                //just right rotate the grandpa, and recolor
                //that rebalance the RBTree
                grandpa->color = RBTreeColor::Red;
                parent->color = RBTreeColor::Black;
                RightRotation(root, grandpa);
            }
        }
        else{//symmetric cases
            auto uncle = grandpa->left;
            if(uncle->color == RBTreeColor::Red){
                parent->color = RBTreeColor::Red;
                grandpa->color = RBTreeColor::Red;
                new_node = grandpa;
            }           
            else{
                if(parent->left == new_node){
                    RightRotation(root, parent);
                    new_node = new_node->right;
                }
                grandpa->color = RBTreeColor::Red;
                parent->color = RBTreeColor::Black;
                LeftRotation(root, grandpa);
            }
        }//if(grandpa->left == parent)
    }//while
    root->color = RBTreeColor::Black;
}

}//namespace TinySTL