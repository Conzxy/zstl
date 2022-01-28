#include "../include/ebco_helper.h"
#include "../include/util/macro.h"
#include <assert.h>

using namespace zstl;

EBCO(Callable)

template<typename T>
class Callable : public EBCO_Callable<T> {
public:
  template<typename... U>
  Callable(U&&... t)
    : EBCO_Callable<T>(STL_FORWARD(U, t)...)
  { }

  // using EBCO_Callable<T>::get_Callable;
};

class A {};

class B {
public:
  B(int a) 
    : a{a}
  { }
  int a;
};

template class Callable<A>;
template class Callable<B>;

int main() {
  static_assert(sizeof(Callable<A>) == 1);
  static_assert(sizeof(Callable<B>) == 4);
  
  Callable<A> a; UMI_UNUSED(a);
  Callable<B> b{1}; UMI_UNUSED(b);

  assert(b.get_Callable().a == 1);
}
