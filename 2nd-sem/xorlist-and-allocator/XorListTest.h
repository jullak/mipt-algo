#pragma once
#include <gtest\gtest.h>
#include "XorList.h"
#include <list>
#include <ctime>
#include <cmath>
#include <iostream>
#include <memory>

enum listComand { PUSH_B, PUSH_F, POP_B, POP_F };
enum iteratorComand { INS_BEF, INS_AFT, ERASE, INCR, DECR, GET };


template <class P, class Allocator = std::allocator<P>>
class TesterList {
private:
	XorList<P, Allocator> testList_;
	clock_t resTime_;
public:
	TesterList() : resTime_(0) {}
	size_t size() const {
		return testList_.size();
	}
	clock_t time() const {
		return resTime_;
	}

	void push_back(P x) {
		clock_t time = clock();
		testList_.push_back(x);
		resTime_ += clock() - time;
	}
	void push_front(P x) {
		clock_t time = clock();
		testList_.push_front(x);
		resTime_ += clock() - time;
	}
	void pop_back() {
		clock_t time = clock();
		testList_.pop_back();
		resTime_ += clock() - time;
	}
	void pop_front() {
		clock_t time = clock();
		testList_.pop_front();
		resTime_ += clock() - time;
	}
};

template <class B, class Allocator = std::allocator<B>>
void dataTest(size_t testSize, B rnd()) {
	TesterList<B, Allocator> tester;
	std::list<B, Allocator> l;
	clock_t listTime = 0;

	for (size_t i = 0; i < testSize; ++i) {
		int cmd = rand() % 4;

		if (cmd == PUSH_B) {
			B x = rnd();
			tester.push_back(x); 
			clock_t time = clock();
			l.push_back(x);
			listTime += clock() - time;
		}
		else if (cmd == PUSH_F) {
			B x = rnd();
			tester.push_front(x); 
			clock_t time = clock();
			l.push_front(x);
			listTime += clock() - time;
		}
		else if (cmd == POP_B) {
			EXPECT_EQ(tester.size(), l.size());
			if (tester.size() != 0 && l.size() != 0) {
				tester.pop_back();
				clock_t time = clock();
				l.pop_back();
				listTime += clock() - time;
			}
		}
		else if (cmd == POP_F) {
			EXPECT_EQ(tester.size(), l.size());
			if (tester.size() != 0 && l.size() != 0) {
				tester.pop_front();
				clock_t time = clock();
				l.pop_front();
 				listTime += clock() - time;
			}
		}
	}

	std::cout << "Pure XorList time : " << ((float)tester.time()) / CLOCKS_PER_SEC << " sec" << std::endl;
	std::cout << "Pure standartList time : " << ((float)listTime) / CLOCKS_PER_SEC << " sec" << std::endl;
}