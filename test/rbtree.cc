#include <fstream>
#include <iterator>
#define RBTREE_DEBUG
#include "../include/stl_tree.h"
#include "../include/functional.h"
#include "tool.h"
#include "stl_utility.h"


#define ROOT(rbtree) \
	static_cast<RBTreeNode<typename rbtree::key_type>*>(rbtree.Root())

using namespace TinySTL;

template<typename T>
using Set = RBTree<T, T, identity<T>, less<T>>;
template<typename K, typename V>
using Map = RBTree<K, V, get_first<K, V>, less<K>>;

template<typename T>
class RBTreeTest{
public:
    RBTreeTest() = default;

    void InsertOfSet(int num){
        for(int i = 0; i != num; ++i){
			auto num = GetRandom<T>(0, 10000);
            set_.InsertEqual(num);
		}
			//PrintContainer(set_);
			//printf("Header()->right:%d\n", Set<T>::Value(set_.Header()->right));
			//printf("Header()->right:%d\n", Set<T>::Value(set_.Header()->left));
			//if(set_.Root()->parent == set_.Header() && set_.Header()->parent == set_.Root())
				//printf("Header() and Root() required\n");
			//Test_RB_PROPERTY(set_);		
			set_.Print();
    }

	void Erase(){
		set_.Print();
	}

	void Copy() {
		InsertOfSet(30);
		auto rb1 = set_;
		if(rb1 == set_) printf("copy construct right\n");
		InsertOfSet(60);
		rb1 = set_;
		if(rb1 == set_) printf("Copy assignment right\n");
		set_.Print();
		rb1.Print();
		TEST_RB_PROPERTY(set_);
		set_.erase(set_.begin(), Prev_Iter(set_.end()));
		set_.Print();
	}
private:
    Set<T> set_;
};

int main(){
    RBTreeTest<int> test;
	printf("size of RBTree: %d\n", sizeof(Set<int>));
	test.Copy();
	
}
