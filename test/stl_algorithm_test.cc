#include "stl_algorithm.h"
#include "vector.h"
#include <forward_list>

#include <gtest/gtest.h>

TEST(stl_algorithm, find) {
  zstl::Vector<int> vec{1, 2, 3, 4};

  auto it1 = zstl::find(vec.begin(), vec.end(), 2);
  EXPECT_NE(it1, vec.end());
  EXPECT_EQ(*it1, 2);

  auto it2 = zstl::find(vec.begin(), vec.end(), 5);

  EXPECT_EQ(it2, vec.end());
}

TEST(stl_algorithm, find_if) {
  zstl::Vector<int> vec { 1, 3, 5, 7, 9 };

  auto it1 = zstl::find_if(
    vec.begin(), vec.end(), 
    [](int x) { return x % 2 == 0; });

  EXPECT_EQ(it1, vec.end());

  auto it2 = zstl::find_if(
    vec.begin(), vec.end(),
    [](int x) { return x % 2 != 0; });

  EXPECT_NE(it2, vec.end());
  EXPECT_EQ(*it2, 1);
}

// find_if_not like find_if, no need to test

TEST(stl_algorithm, range_check) {
  zstl::Vector<int> odds{ 1, 3, 5, 7, 9 };
  zstl::Vector<int> evens{ 2, 4, 6, 8, 10 };
  zstl::Vector<int> odds_and_evens{ 0, 1, 2, 3 };

  // Check if even
  auto pred = [](int x) { return x % 2 == 0; };

  EXPECT_TRUE(zstl::all_of(evens.begin(), evens.end(), pred));
  EXPECT_FALSE(zstl::all_of(odds.begin(), odds.end(), pred));
  EXPECT_TRUE(zstl::none_of(odds.begin(), odds.end(), pred));
  EXPECT_TRUE(zstl::any_of(
    odds_and_evens.begin(), odds_and_evens.end(), pred));
  EXPECT_FALSE(zstl::any_of(odds.begin(), odds.end(), pred));
}

TEST(stl_algorithm, count) {
  zstl::Vector<int> vec{ 1, 2, 3, 1, 1, 3, 4, 5 };
  EXPECT_EQ(zstl::count(vec.begin(), vec.end(), 1), 3);
  EXPECT_EQ(zstl::count(vec.begin(), vec.end(), 3), 2);

  EXPECT_EQ(zstl::count_if(vec.begin(), vec.end(), [](int x) {
    return x % 2 == 0;
  }), 2);

}

TEST(stl_algorithm, for_each) {
  zstl::Vector<int> vec{ 1, 2, 3, 4, 5 };
  
  zstl::for_each(vec.begin(), vec.end(), [](int& x) {
     x *= x;
  });

  for (int i = 1; i < 6; ++i) {
    EXPECT_EQ(vec[i-1], i * i);
  }
}

TEST(stl_algorithm, mismatch) {
  zstl::Vector<int> a1{ 1, 2, 3, 4, 5 };
  zstl::Vector<int> a2{ 1, 2, 4, 5, 3 };

  auto first_pair = zstl::mismatch(a1.begin(), a1.end(), a2.begin());
  EXPECT_EQ(first_pair.first, a1.begin()+2);
  EXPECT_EQ(first_pair.second, a2.begin()+2);
}

TEST(stl_algorithm, search) {
  zstl::Vector<int> set{ 1, 2, 3, 4, 5 };
  zstl::Vector<int> subset{ 2, 3, 4 };

  EXPECT_EQ(
    zstl::search(
      set.begin(), set.end(), 
      subset.begin(), subset.end()),
    set.begin()+1);
}

TEST(stl_algorithm, find_end) {
  zstl::Vector<int> set{ 1, 2, 3, 2, 3, 1, 2, 3, 5, 6, 2, 3, 4, 5 };
  auto subset = { 2, 3 };

  // Test for bidirectional iterator
  EXPECT_EQ(zstl::find_end(
    set.begin(), set.end(),
    subset.begin(), subset.end()), set.end()-4);

  // Test for forward iterator
}

TEST(stl_algorithm, transform) {

}

int main()
{
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}