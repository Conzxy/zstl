#ifndef _HASH_FUN_H
#define _HASH_FUN_H

#include <algorithm.h>

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

    constexpr unsigned long next_prime(unsigned long n){
        const unsigned long* first=PRIME_LIST;
        const unsigned long* last=PRIME_LIST+PRIMES_NUM;
        const unsigned long* pos=lower_bound(first,last,n);

        return pos == last ? *(pos-1) : *pos;
    } 

}

#endif