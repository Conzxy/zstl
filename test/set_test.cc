#define RBTREE_DEBUG

#include "../include/set.h"
#include "../include/vector.h"

#include "tool.h"

#include <algorithm>
#include <gtest/gtest.h>

#define N 1000000

using namespace zstl;
using namespace std;

TEST(MySet, insert) {
	Set<int> s;
  
  std::initializer_list<int> il{ 999, 2, 1, 10 };
  
  s.insert(il.begin(), il.end());
  auto res = s.insert(1);
  
  EXPECT_FALSE(res.second);
  EXPECT_EQ(*res.first, 1);

  EXPECT_TRUE(is_sorted(s.begin(), s.end()));
}

TEST(MySet, insert_hint) {
  Set<int> s;

  s.insert(s.end(), 91);
  s.insert(s.begin(), 2);
  s.insert(++s.begin(), 3);
  
  s.insert(s.end(), 1);
  auto res = s.insert(s.end(), 1);
  //PrintContainer(s);
  EXPECT_TRUE(is_sorted(s.begin(), s.end()));
  EXPECT_FALSE(res.second);
  EXPECT_EQ(*res.first, 1);
}

TEST(MySet, find) {
  Set<int> s;

  s.insert(2);
  s.insert(10);
  s.insert(100);
 
  auto f1 = s.find(2); 
  EXPECT_NE(f1, s.end());
  EXPECT_EQ(*f1, 2);

  f1 = s.find(10);
  EXPECT_NE(f1, s.end());
  EXPECT_EQ(*f1, 10);

  f1 = s.find(9);
  EXPECT_EQ(f1, s.end());
}

TEST(MySet, erase) {
  Set<int> s;

  for (int i = 0; i < 1000; ++i) {
    s.insert(i);
  }

  for (int i = 0; i < 1000; ++i) {
    s.erase(s.begin());
  }

  EXPECT_TRUE(s.empty());
}

TEST(MySet, copy) {
  Set<int> s;

  for (int i = 0; i < 1000; ++i) {
    s.insert(i);
  }

  // Test1: size less than other tree
  Set<int> s1;
  s1 = s;

  EXPECT_EQ(s1.size(), 1000);

  // Test2: size more than other tree
  
  Set<int> s2;

  for (int i = 0; i < 1500; ++i) {
    s2.insert(i);
  }

  s2 = s;
  EXPECT_EQ(s2.size(), 1000);


  Set<int> empty;

  s2 = empty;

  EXPECT_TRUE(s2.empty()); 
}

int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
