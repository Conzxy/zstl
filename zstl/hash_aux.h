#ifndef ZSTL_HASH_AUX_H
#define ZSTL_HASH_AUX_H

#include "stl_algorithm.h"
#include "type_traits.h"
#include <stdint.h>
#include <string>

namespace zstl {

#define PRIMES_NUM 28

extern const unsigned long PRIME_LIST[PRIMES_NUM];

inline unsigned long nextPrime(unsigned long n)
{
    const auto first = PRIME_LIST;
    const auto last = PRIME_LIST + PRIMES_NUM;
    const auto pos = lower_bound(first, last, n);

    return pos == last ? *(pos - 1) : *pos;
}

namespace detail {

template <typename T>
struct Is_cstring : _false_type
{ };

template <>
struct Is_cstring<char const *> : _true_type
{ };

template <>
struct Is_cstring<char[]> : _true_type
{ };

template <>
struct Is_cstring<char *> : _true_type
{ };

template <typename T>
struct Is_string : _false_type
{ };

template <>
struct Is_string<std::string> : _true_type
{ };

} // namespace detail

// hash function
template<typename K, typename = void>
struct hash
{ };

template<typename K>
struct hash <K, zstl::Enable_if_t<Is_integral_v<K>>>
{
    size_t operator()(K i) const
    {
        return i;
    }
};

template <typename K>
struct hash<K, zstl::Enable_if_t<zstl::Is_floating_point_v<K>>>
{
    size_t operator()(K f) const
    {
        if (f == 0)
            return 0;
        return (unsigned)f;
    }
};

template <typename K>
struct hash<K, zstl::Enable_if_t<detail::Is_cstring<K>::value>>
{
    size_t operator()(K str) const
    {
        size_t hashval = 0;

        for (size_t i = 0; str[i] != '\0'; ++i)
        {
            hashval = 37 * hashval + str[i];
        }

        return hashval;
    }
};

template <typename K>
struct hash<K, zstl::Enable_if_t<detail::Is_string<K>::value>>
{
    size_t operator()(K str) const
    {
        size_t hashval = 0;

        for (auto &ch : str)
        {
            hashval = hashval * 37 + ch;
        }

        return hashval;
    }
};

/**
 * @fn hashDivision
 * @brief 
 * h(k) = k mod m
 * key value denoted k
 * slots size denoted m
 * @see <<Introduction to algorithms>> 11.3. The division method
 */
ZSTL_CONSTEXPR uint64_t hashDivision(uint64_t key, uint64_t slotsNum) {
    return key % slotsNum;
}

} // namespace zstl

#endif