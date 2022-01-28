#include "vector.h"
#include "tool.h"

#include <vector>
#include <gtest/gtest.h>

using namespace std;
using namespace zstl;

#define M 100
#define N 5000000

int i = 0;

struct A {
	A() {
	}

	~A() {
		++i;
		printf("~A(): %d\n", i);
	}

	//static int i;
};

// TEST(stlVec, emplace_back) {
// 	vector<int> vec;
// 	for(unsigned i = 0; i != N; ++i)
// 		vec.emplace_back(i);
// }

TEST(MyVecBehaviour, push_back) {
	Vector<int> vec;
	for(unsigned i = 0; i != M; ++i)
		vec.emplace_back(i);
	
	PrintContainer(vec);
}

TEST(MyVecBehaviour, copy_ctor) {
}

TEST(MyVecBehaviour, insert_single) {
	Vector<int> vec;
	for (int i = 0; i != 10; ++i)
		vec.emplace_back(i);

	auto it5 = vec.begin() + 5;
	vec.insert(it5, 100);

	EXPECT_EQ(vec[5], 100);
}

TEST(MyVecBehaviour, insert_fill_n) {
	Vector<int> vec;
	
	for (int i = 0; i != 10; ++i)
		vec.emplace_back(i);

	vec.insert(vec.begin() + 5, 10, 1);

	auto it5 = vec.begin() + 5;	

	Vector<int> test_vec(10, 1);
	EXPECT_EQ(zstl::lexicographical_compare(it5, it5 + 10, test_vec.begin(), test_vec.end()), 0);
}

TEST(MyVecBehaviour, insert_range) {
	Vector<int> vec;
	for (int i = 0; i != 10; ++i)
		vec.emplace_back(i);

	Vector<int> data_vec;
	for (int i = 100; i != 110; ++i)
		data_vec.emplace_back(i);

	auto it5 = vec.begin() + 5;
	vec.insert(it5, data_vec.begin(), data_vec.end());

	auto new_it5 = vec.begin() + 5;
	EXPECT_EQ(zstl::lexicographical_compare(new_it5, new_it5 + 10, data_vec.begin(), data_vec.end()), 0);
}

TEST(MyVecBehaviour, erase_single) {
	Vector<int> vec;
	for (int i = 0; i != 10; ++i)
		vec.emplace_back(i);

	auto it5 = vec.begin() + 5;
	vec.erase(it5);

	EXPECT_EQ(vec[5], 6);
}

TEST(MyVecBehaviour, erase_range) {
	Vector<int> vec;
	for (int i = 0; i != 10; ++i)
		vec.emplace_back(i);

	auto it5 = vec.begin() + 5;
	auto it8 = vec.begin() + 8;
	vec.erase(it5, it8);

	auto il = { 0, 1, 2, 3, 4, 8, 9 };
	EXPECT_EQ(zstl::lexicographical_compare(vec.begin(), vec.end(), il.begin(), il.end()), 0);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
