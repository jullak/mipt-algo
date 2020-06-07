#pragma once
#include <gtest\gtest.h>
#include "deque.h"
#include <deque>
#include <ctime>
#include <cmath>
#include <iostream>

enum deqComand {PUSH_B, PUSH_F, POP_B, POP_F, FRONT, BACK, OPERATOR};

template <typename P>
class TesterDeq {
private:
	Deque<P> testDeq;
	clock_t resTime;
public:
	TesterDeq() : resTime(0) {}
	bool empty() const {
		return testDeq.empty();
	}
	size_t size() const {
		return testDeq.size();
	}
	clock_t time() const {
		return resTime;
	}

	void push_back(P x) {
		clock_t time = clock();
		testDeq.push_back(x);
		resTime += clock() - time;
	}
	void push_front(P x) {
		clock_t time = clock();
		testDeq.push_front(x);
		resTime += clock() - time;
	}
	void pop_back() {
		clock_t time = clock();
		testDeq.pop_back();
		resTime += clock() - time;
	}
	void pop_front() {
		clock_t time = clock();
		testDeq.pop_front();
		resTime += clock() - time;
	}

	P front() {
		clock_t time = clock();
		P x = testDeq.front();
		resTime += clock() - time;
		return x;
	}
	P back() {
		clock_t time = clock();
		P x = testDeq.back();
		resTime += clock() - time;
		return x;
	}
	P operator [] (int index) {
		clock_t time = clock();
		P x = testDeq[index];
		resTime += clock() - time;
		return x;
	}
};

template <typename B>
void dataTest(size_t testSize, B rnd()) {
	TesterDeq<B> tester;
	std::deque<B> d;

	for (size_t i = 0; i < testSize; ++i) {
		int cmd = rand() % 7;

		if (cmd == PUSH_B) {
			B x = rnd();
			tester.push_back(x);
			d.push_back(x);
		}
		else if (cmd == PUSH_F) {
			B x = rnd();
			tester.push_front(x);
			d.push_front(x);
		}
		else if (cmd == POP_B) {
			EXPECT_EQ(tester.empty(), d.empty());
			if (!tester.empty() && !d.empty()) {
				tester.pop_back();
				d.pop_back();
			}	
		}
		else if (cmd == POP_F) {
			EXPECT_EQ(tester.empty(), d.empty());
			if (!tester.empty() && !d.empty()) {
				tester.pop_front();
				d.pop_front();
			}
		}
		else if (cmd == FRONT) {
			EXPECT_EQ(tester.empty(), d.empty());
			if (!tester.empty() && !d.empty())
				EXPECT_EQ(tester.front(), d.front());
		}
		else if (cmd == BACK) {
			EXPECT_EQ(tester.empty(), d.empty());
			if (!tester.empty() && !d.empty())
				EXPECT_EQ(tester.back(), d.back());
		}
		else if (cmd == OPERATOR) {
			EXPECT_EQ(tester.size(), d.size());
			if (tester.size() && d.size()) {
				int ind = rand() % tester.size();
				EXPECT_EQ(tester[ind], d[ind]);
			}
		}
	}

	std::cout << "Pure time : " << ((float)tester.time()) / CLOCKS_PER_SEC << " sec" << std::endl;
}

