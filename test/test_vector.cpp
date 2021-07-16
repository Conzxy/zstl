//
// Created by conzxy on 2021/3/20.
//

#include "vector.h"
#include "tool.h"
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
        PrintContainer(*vec);
        caseEnd();
    }

    void test_insert(size_type const num){
        caseBegin();
        std::cout<<"test insert of vector<int>:\n";
        for(int i=0;i <= num;++i)
            vec->insert(vec->begin(),i);
        PrintContainer(*vec);
        caseEnd();
    }
    
    template<typename II,TinySTL::Enable_if_t<TinySTL::is_input_iterator<II>,int> =0>
    void test_insert(II first,II last){
        caseBegin();
        std::cout<<"test insert of vector<int>(iterator):\n";
        vec->insert(vec->begin(),first,last);
        //vec->erase(vec->begin(),vec->end());
        //vec->erase(vec->begin());
        PrintContainer(*vec);
    }

    void test_copy(){
        caseBegin();
        std::cout<<"test copy of vector<int>:"<<std::endl;
        for(int i=0;i!=1000;++i)
            vec->emplace_back(i);
        auto v=*vec;
        PrintContainer(v);
        std::cout<<"capacity of copy: "<<v.capacity()
                 <<"\ncapacity of origin: "<<vec->capacity();
        std::cout<<"after reserve\n";
        v.reserve(vec->capacity());
        
        std::cout<<"capacity of copy: "<<v.capacity()
                 <<"\ncapacity of origin: "<<vec->capacity()<<std::endl;
        assert(v.capacity() == vec->capacity());
        assert(v.size() == vec->size());
        try{
            std::cout<<v.at(1000);
        }catch(std::range_error const& e){
            std::cerr<<e.what();
        }
        caseEnd();
    }
};


int main(){
    int arr[10000];
    for(int i=0;i!=10000;++i)
        arr[i]=i;
    int* parr=arr;
    test_vector t_vec;
    t_vec.test_push_back(1000);
    //t_vec.test_insert(parr,parr+1000);
    //t_vec.test_copy();

}
