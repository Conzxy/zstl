//
// Created by conzxy on 2021/3/20.
//

#ifndef TINYSTL__TOOL_H
#define TINYSTL__TOOL_H

#include <iostream>
#include <random>
#include "../include/stl_tree.h"

using namespace TinySTL;

template<typename Container>
std::ostream& PrintContainer(Container const& cont,std::string const& message="",
                              std::ostream& os=std::cout){
    os<<message;
    int cnt = 0;
    for(auto& elem : cont){
        ++cnt;
        os<<elem<<" ";
        if(cnt == 10){
            cnt = 0;
            os<<'\n';
        }
    }

    return os << "\ntotal elememts : " << cont.size() << std::endl; 
}

template<typename T>
auto GetRandom(T f,T l){
    static std::uniform_int_distribution<T> u(f,l);
    static std::default_random_engine e;
    return u(e);
}

#endif //TINYSTL__TOOL_H
