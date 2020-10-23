#ifndef TINYSTL_PRINTS_H
#define TINYSTL_PRINTS_H

#include <iostream>
#include "vector.h"
#include <string>
template<typename C>
void PRINTS(C const& x,std::string const& s){
    std::cout<<s<<" ";
    for(auto &e:x){
        std::cout<<e<<" ";
    }
    std::cout<<std::endl;
}
#endif //TINYSTL_PRINTS_H
