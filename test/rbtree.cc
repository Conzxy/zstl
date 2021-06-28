#define RBTREE_DEBUG
#include "../include/stl_tree.h"
#include "../include/functional.h"
#include "tool.h"

using namespace TinySTL;

template<typename T>
using Set = RBTree<T, T, identity<T>, less<T>>;
template<typename K, typename V>
using Map = RBTree<K, V, get_first<K, V>, less<K>>;

template<typename T>
class RBTreeTest{
public:
    RBTreeTest() = default;

    void InsertOfSet(){
        for(int i = 0; i != 100; ++i)
            set_.InsertEqual(GetRandom(0, 10000));

        PrintContainer(set_);
    }

    void PrintDirTree(){
        ::PrintDirTree((RBTreeNode<T>*)set_.Root());
    }
private:
    Set<T> set_;
};

int main(){
    RBTreeTest<int> test;
    test.InsertOfSet();
    test.PrintDirTree();    
}