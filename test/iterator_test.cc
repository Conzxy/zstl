#include "stl_iterator.h"

#include <list>

namespace std {
  void f() {}
}

using namespace zstl;

//struct ForwardIterator : zstl::iterator<Forward_iterator_tag, int> {};

int main()
{
  std::list<int> s;
  std::f();
}