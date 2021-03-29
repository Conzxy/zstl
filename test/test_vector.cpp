//
// Created by conzxy on 2021/3/20.
//

#include <vector.h>
#include <tool.h>
using namespace TinySTL;

class test_vector{
protected:
    vector<int>* vec;
    using size_type = typename vector<int>::size_type;

public:
    void caseBegin(){ vec=new vector<int>(); }
    void caseEnd(){ delete vec; }

    //test part
    void test_push_back(size_type const sz){
        caseBegin();
        std::cout<<"test push_back of vector<int>:"<<std::endl;
        for(int i=0;i<=sz;++i){
            vec->push_back(i);
        }
        print_container(*vec);
        caseEnd();
    }

};


int main(){
    test_vector t_vec;
    t_vec.test_push_back(1000);
}
