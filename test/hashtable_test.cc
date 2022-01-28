#define HASH_DEBUG

#include "hash_table.h"
#include "functional.h"
#include "tool.h"

#include <iostream>
#include <string.h>
#include <unordered_set>
#include <gtest/gtest.h>

using namespace std;

template<typename K>
using HashSet 
= zstl::HashTable<
    K, K, zstl::hash<K>, zstl::identity<K>, zstl::equal_to<K>>;

#define N 5000

TEST(MyHashTest, insertUnique) {
    HashSet<std::string> hashSet;
    for (int i = 0; i != N; ++i)
        //hashSet.insertUnique(getRandomString(i % 10 + 1));
        hashSet.insertUnique(std::to_string(i));

    std::cout << "my load_factor: " << hashSet.load_factor() << "\n";
    //PrintContainer(hashSet);
    //EXPECT_EQ(hashSet.size(), N);
}

TEST(STLHashTest, insert) {
    std::unordered_set<std::string> hashSet;
    for (int i = 0; i != N; ++i)
        hashSet.insert(std::to_string(i));

    std::cout << "load_factor: " << hashSet.load_factor() << "\n";
}

int main(int argc, char* argv[]) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}