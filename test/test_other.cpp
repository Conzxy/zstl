#include "fmt/format.h"
#include "fmt/printf.h"
#include <functional.h>
#include <memory>
#include "vector.h"
#include "algorithm.h"
#include <type_traits>
#include <typeinfo>

using namespace std;
using namespace fmt;
using namespace fmt::literals;


struct FunObj
{
    void f(int x) const { print("member function called for {}\n",x); }
};

using fm=void(FunObj::*)(int) const;

template<typename T>
struct TT
{
    TT(T x) : data_{x}{}

    T data_;
};
int& f(int x, int y) {
  // ...
  
}
int main()
{
    TinySTL::vector<int> primes{2,3,5,7,11,13,17,19};
    TinySTL::vector intv(10,2);
    fm ff=&FunObj::f;
    FunObj fo{};
    TT x{11};
    std::
    print("{}",x.data_);
    ::invoke(ff,&fo,2);
    
}