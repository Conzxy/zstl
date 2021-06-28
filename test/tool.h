//
// Created by conzxy on 2021/3/20.
//

#ifndef TINYSTL__TOOL_H
#define TINYSTL__TOOL_H

#include <iostream>
#include <random>
#include "../include/stl_tree.h"

using namespace TinySTL;

template<typename Container>
std::ostream& PrintContainer(Container const& cont,std::string const& message="",
                              std::ostream& os=std::cout){
    os<<message;
    int cnt = 0;
    for(auto& elem : cont){
        ++cnt;
        os<<elem<<" ";
        if(cnt == 10){
            cnt = 0;
            os<<'\n';
        }
    }

    return os << "\ntotal elememts : " << cont.size() << std::endl; 
}

auto GetRandom(long long f,long long l){
    static std::uniform_int_distribution<long long> u(f,l);
    static std::default_random_engine e;
    return u(e);
}

template<typename T>
void PrintRoot(RBTreeNode<T>* root, std::ostream& os){
	os << root->val << ((root->color == RBTreeColor::Red) ? "(Red)" : "(Black)") << '\n';
}

template<typename V>
void PrintSubTree(RBTreeNode<V>* root, std::string const& prefix
                           , std::ostream& os){
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
            PrintSubTree(root->Right(), prefix + "|   ", os);
        else
            PrintSubTree(root->Right(), prefix + "    ", os);
    }

    if(has_left){
        os << ((has_right) ? prefix : "") << "└── ";
		PrintRoot(root->Left(), os);
        PrintSubTree(root->Left(), prefix + "    ", os); 
    }
}

template<typename V>
void PrintDirTree(RBTreeNode<V>* root, std::ostream& os = std::cout){
    if(! root) return ;
	
	PrintRoot(root, os);

    PrintSubTree(root, "", os);
}



#endif //TINYSTL__TOOL_H
