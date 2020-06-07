#pragma once

class IHeap {
public:
	virtual ~IHeap() {}
	virtual bool empty() const = 0;
	virtual void insert(int key) = 0;
	virtual int getMin() = 0;
	virtual int extractMin() = 0;
	virtual void meld(IHeap & h) = 0;
};
