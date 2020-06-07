#include <gtest\gtest.h>
#include <ctime>
#include <cmath>
#include "XorListTest.h"
#include "StackAllocator.h"

double doubleRand() {
	static const double max = 1e5;
	return ((double)rand() / RAND_MAX)*max;
}
template <class Allocator = std::allocator<int>>
std::list<int, Allocator> initList(size_t size) {
	std::list<int, Allocator> l;
	for (size_t i = 0; i < size; ++i) {
		l.push_back(rand());
	}
	return l;
}

template <class Allocator = std::allocator<int>>
XorList<int, Allocator> initMyList(size_t size) {
	XorList<int, Allocator> ml;
	for (size_t i = 0; i < size; ++i) {
		ml.push_back(rand());
	}
	return ml;
}


TEST(listTest, int_size10_4) {
	dataTest<int>(10000, rand);
}
TEST(listTest, int_size10_5) {
	dataTest<int>(100000, rand);
}
TEST(listTest, int_size10_6) {
	dataTest<int>(1000000, rand);
}
TEST(listTest, double_size10_4) {
	dataTest<double>(10000, doubleRand);
}
TEST(listTest, double_size10_5) {
	dataTest<double>(100000, doubleRand);
}
TEST(listTest, double_size10_6) {
	dataTest<double>(1000000, doubleRand);
}

TEST(listTestAlloc, int_size10_4) {
	dataTest<int, StackAllocator<int>>(10000, rand);
}
TEST(listTestAlloc, int_size10_5) {
	dataTest<int, StackAllocator<int>>(100000, rand);
}
TEST(listTestAlloc, int_size10_6) {
	dataTest<int, StackAllocator<int>>(1000000, rand);
}
TEST(listTestAlloc, double_size10_4) {
	dataTest<double, StackAllocator<double>>(10000, doubleRand);
}
TEST(listTestAlloc, double_size10_5) {
	dataTest<double, StackAllocator<double>>(100000, doubleRand);
}
TEST(listTestAlloc, double_size10_6) {
	dataTest<double, StackAllocator<double>>(1000000, doubleRand);
}



int main(int argc, char *argv[]) {
	srand(time(0));
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	return 0;
}