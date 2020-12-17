#include <iostream>
#include <string>
#include <vector.h>
#include <Prints.h>
#include <utility>
#include <Functional.h>
#include <typeinfo>
#include <algorithm.h>
#include <Memory.h>
#include <list.h>
#include <type_traits.h>
#include <iterator>
#include <MPL.h>

using namespace TinySTL;
using namespace mpl;
using namespace TL;
using namespace VL;
using namespace std;

struct test_1 {
    test_1();
    test_1(int, int);
    test_1(const test_1&);
    test_1(test_1&&);
    test_1& operator=(int);
    test_1& operator=(const test_1&);
    test_1& operator=(test_1&&);
    ~test_1();
    void f() {}
};

struct test_2{
private:
    test_2();
    test_2(int, int);
    test_2(const test_2&);
    test_2(test_2&&);
    test_2& operator=(const test_2&);
    test_2& operator=(test_2&&);
    ~test_2();
};

struct test_3 {
    test_3() noexcept;
    test_3(int, int) noexcept;
    test_3(const test_3&) noexcept;
    test_3(test_3&&) noexcept;
    test_3& operator=(const test_3&) noexcept;
    test_3& operator=(test_3&&) noexcept;
    test_3& operator=(int)noexcept;
    ~test_3() noexcept;
};

struct empty_{};

void g(){}
int main() {
    std::cout<<std::boolalpha;
    /*vector<int> v{ 1,2,3,4 };
    vector<double> v1{ 1.1,2.2,3.3,4.4 };
    //auto e=find(v.begin(), v.end(), 3);
    //std::cout<<*e<<std::endl;
    //std::cout<<accumulate(v.begin(), v.end(), 0);
    std::ostream_iterator<int> oiter(std::cout, "\n");
    TinySTL::adjacent_difference(v.begin(), v.end(), oiter);
    std::cout<<TinySTL::inner_product(v.begin(), v.end(), v1.begin());
    Function<void(int*)> d=[](auto ptr) {
        if (ptr) delete ptr;
    };
    TinySTL::unique_ptr<int, Function<void(int*)> const&> p(new int{ 1 }, 
        TinySTL::move(d));
    TinySTL::unique_ptr<int> p1(new int{ 2 });
    std::cout<<*p;*/

    
    
    /*cout<<"test_1:\n"
        <<Is_default_constructible_v<test_1><<'\n'
        <<Is_constructible_v<test_1, int, int><<'\n'
        <<Is_destructible_v<test_1><<'\n'
        <<Is_copy_constructible_v<test_1><<'\n'
        <<Is_move_constructible_v<test_1><<'\n'
        <<Is_assignable_v<test_1,int><<'\n'
        <<Is_copy_assignable_v<test_1><<'\n'
        <<Is_move_assignable_v<test_1><<'\n'
        <<Is_nothrow_default_constructible_v<test_1><<'\n'
        <<Is_nothrow_constructible_v<test_1, int, int><<'\n'
        <<Is_nothrow_destructible_v<test_1><<'\n'
        <<Is_nothrow_copy_constructible_v<test_1><<'\n'
        <<Is_nothrow_move_constructible_v<test_1><<'\n'
        <<Is_nothrow_assignable_v<test_1, int><<'\n'
        <<Is_nothrow_copy_assignable_v<test_1><<'\n'
        <<Is_nothrow_move_assignable_v<test_1><<'\n';

    cout<<"test_2:\n"
        <<Is_default_constructible_v<test_2><<'\n'
        <<Is_constructible_v<test_2, int, int><<'\n'
        <<Is_destructible_v<test_2><<'\n'
        <<Is_copy_constructible_v<test_2><<'\n'
        <<Is_move_constructible_v<test_2><<'\n'
        <<Is_assignable_v<test_2, int><<'\n'
        <<Is_copy_assignable_v<test_2><<'\n'
        <<Is_move_assignable_v<test_2><<'\n'
        <<Is_nothrow_default_constructible_v<test_2><<'\n'
        <<Is_nothrow_constructible_v<test_2, int, int><<'\n'
        <<Is_nothrow_destructible_v<test_2><<'\n';

        cout<<"test_3:\n"
        <<Is_nothrow_default_constructible_v<test_3><<'\n'
        <<Is_nothrow_constructible_v<test_3, int, int><<'\n'
        <<Is_nothrow_destructible_v<test_3><<'\n'
        <<Is_nothrow_copy_constructible_v<test_3><<'\n'
        <<Is_nothrow_move_constructible_v<test_3><<'\n'
        <<Is_nothrow_assignable_v<test_3, int><<'\n'
        <<Is_nothrow_copy_assignable_v<test_3><<'\n'
        <<Is_nothrow_move_assignable_v<test_3><<'\n';
        
        auto pf=&g;
        cout<<"Is_function:\n"
            <<Is_function<Remove_pointer_t<decltype(pf)> const>::value<<'\n';

        cout<<"Is_class:\n"
            <<Is_class<test_1 const volatile >::value<<'\n';

        cout<<"Add_pointer:\n"
            <<Is_same_v<Add_pointer_t<int&>, int*><<'\n';

        cout<<"Add_lvalue_reference:\n"
            <<Is_same_v<Add_lvalue_reference_t<int()&>, int()&><<'\n';

        cout<<"Add_rvalue_reference:\n"
            <<Is_same_v<Add_rvalue_reference_t<int() const>, int()const><<'\n';

        cout<<"Is_signed:\n"
            <<Is_signed<char>::value<<'\n';

        cout<<"Is_unsigned:\n"
            <<Is_unsigned<int>::value<<'\n';

        cout<<"Make_signed:\n"
            <<Is_same_v<Make_signed<unsigned char>::type, signed char><<'\n';
        
        cout<<"Make_unsigned:\n"
            <<Is_same_v<Make_unsigned<int>::type, unsigned int><<'\n';

        cout<<"Is_swappable:\n"
            <<Is_swappable<int>::value<<'\n';
        
        cout<<"Is_empty:\n"
            <<Is_empty_v<empty_><<'\n';

        cout<<"Common_type:\n"
            <<Is_same_v<int, Common_type_t<int, const int>><<'\n';*/      
        
        using tl=Typelist<int, double,vector<int>,float>;
        Is_same_v<Type_At<tl, 2>,vector<int>>;
        Is_same_v<Type_At<tl, 3>, float>;
        static_assert(!Is_Empty<tl>, "");
        using SignedInt=Typelist<bool, char, signed char, short, int, long, long long>;
        static_assert(Is_same_v<Largest_Type<SignedInt>,long long>,"");
        using emptyList=Typelist<>;
        using tl1=Push_Back<SignedInt, unsigned int>;
        using reverL=Reverse<tl1>;
        static_assert(Is_same_v<Front<reverL>, unsigned int>, "");
        static_assert(Is_same_v<Back<Pop_Back<reverL>>,char>,"");
        static_assert(Is_same_v<Back<tl1>, unsigned int>, "");
        using constL=Transform<reverL, Add_const>;
        static_assert(Is_same_v<Front<constL>,const unsigned int>,"");
        TL_PRINTS<constL>(std::cout);
        static_assert(Length<reverL> ==8,"");
        static_assert(Is_same_v<Accumulate<SignedInt, Larger_Type, char>, long long>,"");
        TL_PRINTS<Sort<tl1, Larger_pred>>(std::cout);
        
        using vl=Valuelist<int,1, 2,3,6, 5,4>;
        VL::Length<vl>;
        using evl=Valuelist<int>;
        Front<vl>;
        static_assert(Is_Empty<evl>, "");
        TL_PRINTS<typename Push_FrontT<vl, CValueT<int,5>>::type>(std::cout);
        TL_PRINTS<Push_Back<vl, CValueT<int,99>>>(std::cout);
        TL_PRINTS<Sort<vl, Larger_pred_v>>(std::cout);
        using cl=Cons<int, Cons<double, Cons<char, Cons<unsigned int>>>>;
        Front<cl>;
        Length<cl>;
        Is_same_v<Type_At<cl, 3>, unsigned int>;
        static_assert(!Is_Empty<cl>, "");
        TL_PRINTS<Push_Back<cl,int>>(std::cout);
        using ttt=typename FrontT<vl>::type;
        cout<<"hello";
}