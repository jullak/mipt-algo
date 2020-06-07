#include "IHeap.h"
#include "BinomHeap.h"
#include "LeftSkewHeap.h"
#include "TestHeap.h"
#include "LSHeap_instruments.h"
#include <vector>
#include <gtest\gtest.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

enum testMethod { ADD, INSERT, GET_MIN, EXTRACT_MIN, MELD };
enum heapType { BINOM, LEFT, SKEW };
class Tester {
private:
	std::vector <IHeap *> heaps;
	clock_t resTime;
public:
	Tester() : resTime(0) {}
	~Tester() {
		for (size_t i = 0; i < heaps.size(); ++i) {
			delete heaps[i];
		}
	}
	size_t size() const {
		return heaps.size();
	}
	clock_t time() const {
		return resTime;
	}
	void addHeap(int k, heapType heapT) {
		IHeap * h;
		if (heapT == BINOM)
			h = dynamic_cast<IHeap*>(new BinomHeap(k));
		else if (heapT == LEFT)
			h = dynamic_cast<IHeap*>(new LeftSkewHeap<LNode, LMerger>(k));
		else if (heapT == SKEW)
			h = dynamic_cast<IHeap*>(new LeftSkewHeap<SNode, SMerger>(k));

		heaps.push_back(h);
	}
	void insert(int index, int k) {
		clock_t time = clock();
		heaps[index]->insert(k);
		resTime += clock() - time;
	}
	void meld(int index1, int index2) {
		if (index1 != index2) {
			clock_t time = clock();
			heaps[index1]->meld(*heaps[index2]);
			resTime += clock() - time;
		}
	}

	int getMin(int index) {
		if (!heaps[index]->empty()) {
			clock_t time = clock();
			int x = heaps[index]->getMin();
			resTime += clock() - time;
			return x;
		}
	}
	int extractMin(int index) {
		if (!heaps[index]->empty()) {
			clock_t time = clock();
			int x =  heaps[index]->extractMin();
			resTime += clock() - time;
			return x;
		}
	}

	bool empty(int ind) {
		return heaps[ind]->empty();
	}
};

void allTest(heapType heapType, size_t testSize) {
	Tester tester;
	std::vector <TestHeap> testHeaps;

	for (size_t i = 0; i < testSize; ++i) {
		int method = rand() % 5;

		if (method == ADD) {
			int x = rand();
			tester.addHeap(x, heapType);
			testHeaps.push_back(TestHeap(x));
		}
		else if (method == INSERT && tester.size()) {
			int ind = rand() % tester.size();
			int x = rand();
			tester.insert(ind, x);
			testHeaps[ind].insert(x);
		}
		else if (method == MELD && tester.size()) {
			int ind1 = rand() % tester.size();
			int ind2 = rand() % tester.size();
			if (ind1 != ind2) {
				testHeaps[ind1].meld(testHeaps[ind2]);
				tester.meld(ind1, ind2);
			}
		}
		else if (method == GET_MIN && tester.size()) {
			int ind = rand() % tester.size();
			EXPECT_EQ(tester.empty(ind), testHeaps[ind].empty());
			if (!testHeaps[ind].empty())
				EXPECT_EQ(tester.getMin(ind), testHeaps[ind].getMin());
		}
		else if (method == EXTRACT_MIN && tester.size()) {
			int ind = rand() % tester.size();
			EXPECT_EQ(tester.empty(ind), testHeaps[ind].empty());
			if (!testHeaps[ind].empty())
				EXPECT_EQ(tester.extractMin(ind), testHeaps[ind].extractMin());
		}
	}
	
	std::cout << "Pure time : " << ((float)tester.time()) / CLOCKS_PER_SEC << " sec" << std::endl;
}


TEST(dataTest, BinomHeap10_4) {
	allTest(BINOM, 10000);
}
TEST(dataTest, BinomHeap10_5) {
	allTest(BINOM, 100000);
}
TEST(dataTest, BinomHeap10_6) {
	allTest(BINOM, 1000000);
}
TEST(dataTest, LeftHeap10_4) {
	allTest(LEFT, 10000);
}
TEST(dataTest, LeftHeap10_5) {
	allTest(LEFT, 100000);
}
TEST(dataTest, LeftHeap10_6) {
	allTest(LEFT, 1000000);
}
TEST(dataTest, SkewHeap10_4) {
	allTest(SKEW, 10000);
}
TEST(dataTest, SkewHeap10_5) {
	allTest(SKEW, 100000);
}
TEST(dataTest, SkewHeap10_6) {
	allTest(SKEW, 1000000);
}


int main(int argc, char *argv[]) {
	srand(time(0));
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
 	return 0;
}