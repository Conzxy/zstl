#include "hash_aux.h"

#include <gtest/gtest.h>

using namespace zstl;
using namespace std;

TEST(hashAuxTest, hashString) {
    EXPECT_EQ(zstl::hash<std::string>{}("a"), (int)'a');
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}