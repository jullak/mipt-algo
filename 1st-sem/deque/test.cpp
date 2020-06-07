#include <gtest\gtest.h>
#include <ctime>
#include <cmath>
#include "iterator_test.h"
#include "deq_test.h"

double doubleRand() {
	static const double max = 1e5;
	return ((double)rand() / RAND_MAX)*max;
}

std::deque<int> initDeq(size_t size) {
	std::deque<int> d;
	for (size_t i = 0; i < size; ++i) {
		d.push_back(i);
	}
	return d;
}
Deque<int> initMyDeq(size_t size) {
	Deque<int> md;
	for (size_t i = 0; i < size; ++i) {
		md.push_back(i);
	}
	return md;
}

TEST(deqTest, int_size10_4) {
	dataTest<int>(10000, rand);
}
TEST(deqTest, int_size10_5) {
	dataTest<int>(100000, rand);
}
TEST(deqTest, int_size10_6) {
	dataTest<int>(1000000, rand);
}
TEST(deqTest, double_size10_4) {
	dataTest<double>(10000, doubleRand);
}
TEST(deqTest, double_size10_5) {
	dataTest<double>(100000, doubleRand);
}
TEST(deqTest, double_size10_6) {
	dataTest<double>(1000000, doubleRand);
}


TEST(iteratorTest, it_50_10_4) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::iterator, std::deque<int>::iterator>(md, d, md.begin(), d.begin(), 10000);
}
TEST(iteratorTest, it_50_10_5) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::iterator, std::deque<int>::iterator>(md, d, md.begin(), d.begin(), 100000);
}
TEST(iteratorTest, cit_50_10_4) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::const_iterator, std::deque<int>::const_iterator>(md, d, md.cbegin(), d.cbegin(), 10000);
}
TEST(iteratorTest, cit_50_10_5) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::const_iterator, std::deque<int>::const_iterator>(md, d, md.cbegin(), d.cbegin(), 100000);
}

TEST(iteratorTest, rit_50_10_4) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::reverse_iterator, std::deque<int>::reverse_iterator>(md, d, md.rbegin(), d.rbegin(), 10000);
}
TEST(iteratorTest, rit_50_10_5) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::reverse_iterator, std::deque<int>::reverse_iterator>(md, d, md.rbegin(), d.rbegin(), 100000);
}
TEST(iteratorTest, crit_50_10_4) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::const_reverse_iterator, std::deque<int>::const_reverse_iterator>(md, d, md.crbegin(), d.crbegin(), 10000);
}
TEST(iteratorTest, crit_50_10_5) {
	std::deque<int> d = initDeq(50);
	Deque<int> md = initMyDeq(50);
	itTest<Deque<int>::const_reverse_iterator, std::deque<int>::const_reverse_iterator>(md, d, md.crbegin(), d.crbegin(), 100000);
}

int main(int argc, char *argv[]) {
	srand(time(0));
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	return 0;
}