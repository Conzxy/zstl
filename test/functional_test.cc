#include "include/functional.h"

using namespace zstl;

int main()
{
  static_assert(Is_UnaryPredicate<int>([](int)->bool { return false; }), "");
  static_assert(Is_UnaryPredicate<int>([](double) {}), "");
}
