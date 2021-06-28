#include <hash_table_impl.h>
#include <functional.h>
#include <tool.h>
#include <mystring.h>
#include <unordered_set>
using namespace TinySTL;

class TestHashtable{
public:
    using HT = HashTable<int,int,hash<int>,identity<int>,equal_to<int>>;
    HT* ht_;
public:
    void caseBegin(){ ht_=new HT(); }
    void caseEnd(){ delete ht_; }

    void test_insert_unique(size_t n){
        caseBegin();
        for(size_t i=0;i != n;++i){
            ht_->insert_unique(i);
        }
        print_container(*ht_); 
        caseEnd();
    }

    void test_erase(){
        caseBegin();
        for(size_t i=0;i < 100;++i){
            ht_->insert_unique(i);
        }
        ht_->erase(ht_->begin());
        
        
        print_container(*ht_);
        //printf("load factor:%f\n",ht_->load_factor());
        
    }
};

int main(){
    TestHashtable test;

    //test.test_insert_unique(100000);
    //test.test_erase();
    
}
