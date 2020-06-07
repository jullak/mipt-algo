#include <vector>
#include <algorithm>
#include "IHeap.h"


class TestHeap : public IHeap {
private:
	std::vector <int> Keys;

public:
	TestHeap() {}
	explicit TestHeap(int key)
	{
		Keys.push_back(key);
	}

	bool empty() const {
		return !Keys.size();
	}

	void insert(int key) {
		Keys.push_back(key);
	}

	void makeHeap() {
		std::make_heap(Keys.begin(), Keys.end());
		std::sort_heap(Keys.begin(), Keys.end());
	}

	int getMin() {
		makeHeap();
		return Keys.front();
	}

	int extractMin() {
		makeHeap();
		int res = Keys.front();
		Keys.erase(Keys.begin());
		return res;
	}

	void meld(IHeap & h) {
		TestHeap & th = dynamic_cast<TestHeap&>(h);
		Keys.insert(Keys.end(), th.Keys.begin(), th.Keys.end());
		th.Keys.clear();
	}
};