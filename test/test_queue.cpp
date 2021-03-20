//
// Created by conzxy on 2021/3/20.
//

#include <iostream>
#include <queue.h>
#include <tool.h>

using namespace TinySTL;

class test_queue{
protected:
    queue<int>* intq;
    using size_type=typename queue<int>::size_type;
public:
    void caseBegin(){intq=new queue<int>();}
    void caseEnd(){delete intq;}

    void testPush(size_type const sz){
        caseBegin();
        std::cout<<"test push operation of queue<int>:\n";
        for(int i=0;i != sz;++i){
            intq->push(i);
            std::cout<<intq->back()<<" ";
        }
        std::cout<<std::endl;
        caseEnd();
    }

    void testPop(size_type const sz){
        caseBegin();
        for(int i=0;i != sz;++i){
            intq->push(i);
        }
        std::cout<<"test pop operation of queue<int>:\n";
        for(int i=0;i != sz;++i){
           std::cout<<intq->front()<<" ";
           intq->pop();
        }
        std::cout<<std::endl;
        caseEnd();
    }

    //由于 queue本质是依赖底层容器的适配器，因此该测试取决于底层容器的对应操作
};


int main(){
    test_queue t;
    t.testPush(1000);
    t.testPop(1000);
}