#ifndef _HASH_FUN_H
#define _HASH_FUN_H

#include <algorithm.h>
#include <type_traits.h>
#include <string>

namespace TinySTL{
    static const int PRIMES_NUM=28;

    static const unsigned long PRIME_LIST[PRIMES_NUM]={
        53,97,193,389,769,
        1543,3079,6151,12289,24593,
        49157,98317,196613,393241,786433,
        1572869,3145739,6291469,12582917,25165843,
        50331653,100663319,201326611,402653189,805306457,
        1610612741,3221225473ul,4294967291ul
    };

    inline unsigned long next_prime(unsigned long n){
        const unsigned long* first=PRIME_LIST;
        const unsigned long* last=PRIME_LIST+PRIMES_NUM;
        const unsigned long* pos=lower_bound(first,last,n);

        return pos == last ? *(pos-1) : *pos;
    } 

    namespace detail{
        template<typename T>
        struct Is_cstring : _false_type{};

        template<>
        struct Is_cstring<char const*> : _true_type{};

        template<>
        struct Is_cstring<char[]> : _true_type{};

        template<>
        struct Is_cstring<char*> : _true_type{};

        template<typename T>
        struct Is_string : _false_type{};
        
        template<>
        struct Is_string<std::string> : _true_type{};
    }

    template<typename T>
    constexpr bool Is_string_v=detail::Is_string<T>::value;

    template<typename T>
    constexpr bool Is_cstring_v=detail::Is_cstring<T>::value;

    //hash function
    template<typename K,typename E=TinySTL::Enable_if_t<Is_integral_v<K>,int>>//void>
    struct hash{
        size_t operator()(K i) const {
            return i;
        }
    };
    
    template<typename K>
    struct hash<K,TinySTL::Enable_if_t<TinySTL::Is_floating_point_v<K>,int>>{
        size_t operator()(K f) const {
            if(f == 0)
                return 0;
            return (unsigned)f;
        }
    };

    template<typename K>
    struct hash<K,TinySTL::Enable_if_t<Is_cstring_v<K>,int>>{ 
        size_t operator()(K str) const {
            size_t hashval=0;

            for(size_t i=0;str[i]!='\0';++i){
                hashval=37*hashval+str[i];
            }        

            return hashval;
        }
    };

    template<typename K>
    struct hash<K,TinySTL::Enable_if_t<Is_string_v<K>,int>>{
        size_t operator()(K str) const {
            size_t hashval=0;

            for(auto& ch : str){
                hashval=hashval*37+ch;
            }

            return hashval;
        }
    };
}

#endif