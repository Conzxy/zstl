//
// Created by conzxy on 2021/3/20.
//

#ifndef TINYSTL__TOOL_H
#define TINYSTL__TOOL_H

#include <iostream>

template<typename Container>
std::ostream& print_container(Container const& cont,std::string const& message="",
                              std::ostream& os=std::cout){
    os<<message;
    for(auto& elem : cont)
        os<<elem<<" ";
    os<<std::endl;
    return os;
}
#endif //TINYSTL__TOOL_H
