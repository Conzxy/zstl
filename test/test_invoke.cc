#include "../include/func/invoke.h"
#include "../include/func/reference_wrapper.h"
#include <stdio.h>

template<typename T>
class B{
public:
    enum E {e1 = 3, e2};
    virtual void zero(E e = e1){ printf("B:zero\n"); }
    virtual void one(E&){}
};

template<typename T>
class D : public B<T>{
public:
    void f(B<T>* p) {
        typename B<T>::E e;
        p->zero();
        this->one(e);
    }

    void zero(typename B<T>::E e){
        printf("D:zero\n");
    }
};

template<typename T>
class A {
 public:
  template<int N>
  class B {
   public:
    template<int M>
    class C {
     public:
      virtual void f(){
          printf("C:f()\n");
      }
    };
  };
};

template<typename T, int N>
struct CC : public A<T>::template B<N>::template C<N>{
    public:
    void f() override {
        printf("CC:f()\n");
    }
};

template<typename T, int N>
class X {
    using CT = typename A<T>::template B<N>::template C<N>;
 public:
  void case1(typename A<T>::template B<N>::template C<N>* p)
  {
    p->f(); // 不调用虚函数
  }
  void case2(typename A<T>::template B<N>::template C<N>& p)
  {
    p->CT::f(); // 不调用虚函数
  }
};

template<typename T>
class C {
  using Type = T;

  struct I {
    C* c; // C是当前实例化
    C<Type>* c2; // C<Type>是当前实例化
    I* i; // I是当前实例化
  };

  struct J {
    C* c; // C是当前实例化
    C<Type>* c2; // C<Type>是当前实例化
    I* i; // I是未知特化
    J* j; // J是当前实例化
  };
};

template<>
class C<int>{

};

template<typename T>
struct incomplete;

struct incomplete2;

template<typename T>
constexpr auto value = incomplete<T>{};

template<typename T>
void test(incomplete<T> a, incomplete2){}

template<typename T>
struct incomplete{

};

struct incomplete2{};

void gg(){}
void ggg();

int main(){
    printf("main:%p\n", main);
    printf("test<int>: %p\n", test<int>);
    printf("gg:%p\n", gg);
    printf("ggg:%p\n", ggg);

    D<int>* p = new D<int>();
    p->f(p);
    test(incomplete<int>{}, incomplete2{});
    C<int> c;
    A<int>::B<1>::C<1>* p2 = new CC<int, 1>();
    X<int, 1>().case1(p2);
    p2->f();

}

void ggg(){}
