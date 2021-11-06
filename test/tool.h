#ifndef TINYSTL__TOOL_H
#define TINYSTL__TOOL_H

#include <iostream>
#include <random>

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
T GetRandom(T f,T l){
    static std::uniform_int_distribution<T> u(f,l);
    static std::default_random_engine e;
    return u(e);
}

std::string getRandomString(uint64_t len) {
    static char const alphabetAndNumber[] = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    static uint8_t index = sizeof alphabetAndNumber;

    std::string str;
    str.reserve(len);

    while (len) {
        str += alphabetAndNumber[GetRandom<int>(0, index)];
        --len;
    }

    return str;
}

template<typename T, typename V>
void emplace_backLoop(T& cont, unsigned n, V const& v) {
	for (unsigned i = 0; i != n; ++i)
		cont.emplace_back(v);
}

#endif //TINYSTL__TOOL_H
