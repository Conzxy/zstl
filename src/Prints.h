#ifndef TINYSTL_PRINTS_H
#define TINYSTL_PRINTS_H

#include <iostream>
#include <vector.h>
#include <string>
#include <type_list.h>
#include <type_traits.h>

template<typename C>
void PRINTS(C const& x,std::string const& s){
    std::cout<<s<<" ";
    for(auto &e:x){
        std::cout<<e<<" ";
    }
    std::cout<<std::endl;
}

/*
* below C++ standard 17
template<typename TL,bool =Is_Empty<TL>>
class TL_PRINTS_helper {
public:
    static void print(std::ostream& ostr) {
        ostr<<typeid(Front<TL>).name()<<" ";
        TL_PRINTS_helper<Pop_Front<TL>>::print(ostr);
    }
};

template<typename TL>
class TL_PRINTS_helper <TL,true>{
public:
    static void print(std::ostream& ostr) {}
};

template<typename TL>
void TL_PRINTS(std::ostream& ostr) {
    TL_PRINTS_helper<TL>::print(ostr);
}*/


// C++ standard 17
/*template<typename TL>
void TL_PRINTS(std::ostream& ostr) {
    if constexpr (!Is_Empty<TL>) {
        ostr<<typeid(Front<TL>).name()<<" ";
        TL_PRINTS<Pop_Front<TL>>(ostr);
    }
}*/

template<typename TL>
void TL_PRINTS(std::ostream& os) {
    os<<typeid(TL).name()<<'\n';
}

template<typename T,typename U>
struct Larger_Type {
    using type=TinySTL::Conditional_t<(sizeof(T)<sizeof(U)),U,T>;
};

template<typename T,typename U,bool =(sizeof(T) > sizeof(U))>
struct Larger_pred {
    static constexpr bool value=true;
};

template<typename T,typename U>
struct Larger_pred<T, U, false> {
    static constexpr bool value=false;
};

#endif //TINYSTL_PRINTS_H
