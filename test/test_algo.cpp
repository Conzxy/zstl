#include <algorithm.h>
#include <vector.h>
#include <tool.h>
#include <algorithm>
using namespace TinySTL;

class test_algo{
private:
    vector<int> vec;
    
public:
    void caseBegin(){ 
        for(int i=0;i < 100;++i){
            vec.emplace_back(getRandom(0,10000));
        }
        std::sort(std::begin(vec),std::end(vec));
        print_container(vec);
    }
    void caseEnd(){ vec.clear(); }

    bool test_lower_bound(int val){
       try{
        caseBegin();
        std::cout<<*lower_bound(vec.begin(),vec.end(),val)<<'\n';
        std::cout<<*std::lower_bound(vec.begin(),vec.end(),val);
        caseEnd();
       }catch(...){
           return false;
       }
       return true;
    }
};

int main(){
    test_algo ta;
    assert(ta.test_lower_bound(2088));
    
}