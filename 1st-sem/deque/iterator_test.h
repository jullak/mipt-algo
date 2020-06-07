#pragma once
#include <gtest\gtest.h>
#include "deque.h"
#include <cmath>
#include <deque>

enum iteratorComand {INC, DEC, PLUS, MINUS_INT, OPERATOR_IT, GET};

template <typename Iterator>
class TesterIt {
private:
	Iterator it;
public:
	TesterIt(){}
	TesterIt(Iterator& init) : it(init) {}

	void inc() { it++; }
	void dec() { it--; }
	void plus(int p) { it = it + p; }
	void minusInt(int p) { it = it - p; }
	int operator[] (int ind) { return it[ind]; }
	int get() { return *it; }
};

template <typename MyIt, typename It>
void itTest(Deque<int>& myD, std::deque<int>& d, MyIt& initMyD, It& initD, size_t testSize) {
	It it = initD;
	TesterIt<MyIt> testIterator(initMyD);

	int sumCmd = 0;

	for (size_t i = 0; i < testSize; ++i) {
		int cmd = rand() % 6;

		if (cmd == INC) {
			if (sumCmd < d.size()) {
				testIterator.inc();
				++it;
				sumCmd++;
			}
		}
		else if (cmd == DEC) {
			if (sumCmd > 0) {
				testIterator.dec();
				--it;
				sumCmd--;
			}
		}
		else if (cmd == PLUS) {
			int p = rand() % d.size();
			if (sumCmd + p < d.size()) {
				testIterator.plus(p);
				it += p;
				sumCmd += p;
			}
		}
		else if (cmd == MINUS_INT) {
			int p = rand() % d.size();
			if (sumCmd - p > 0) {
				testIterator.minusInt(p);
				it -= p;
				sumCmd -= p;
			}
		}
		else if (cmd == OPERATOR) {
			int p = rand() % d.size();
			if (sumCmd + p < d.size())
				EXPECT_EQ(testIterator[p], it[p]);
		}
		else if (cmd == GET) {
			if (0 <= sumCmd && sumCmd < d.size())
				EXPECT_EQ(testIterator.get(), *it);
		}
	}
}
