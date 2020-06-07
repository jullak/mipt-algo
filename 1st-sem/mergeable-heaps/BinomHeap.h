#include "IHeap.h"

class BinomHeap : public IHeap {
private:

	struct Node {
		int key;
		int degree;

		Node * leftBroth;
		Node * rightBroth;
		Node * leftChild;
		Node * rightChild;

		Node() {
			key = 0;
			degree = 0;
			leftBroth = leftChild = rightBroth = rightChild = nullptr;
		}

		explicit Node(int k) {
			key = k;
			degree = 1;
			leftBroth = leftChild = rightBroth = rightChild = nullptr;
		}
	};

	void swapNode(Node* l, Node * r) {
		if (l->leftBroth != nullptr) {
			l->leftBroth->rightBroth = r;
		}
		r->leftBroth = l->leftBroth;
		if (r->rightBroth != nullptr) {
			r->rightBroth->leftBroth = l;
		}
		l->rightBroth = r->rightBroth;
		l->leftBroth = r;
		r->rightBroth = l;
	}
	void destruct(Node * deleted) {
		if (deleted == nullptr) return;
		destruct(deleted->rightBroth);
		destruct(deleted->leftChild);
		delete deleted;
	}
	Node * root;

public:

	BinomHeap() {
		root = nullptr;
	}
	explicit BinomHeap(int key) {
		Node * n = new Node(key);
		root = n;
	}
	explicit BinomHeap(Node * n) {
		this->root = n;
	}
	~BinomHeap() {
		destruct(root);
	}

	bool empty() const {
		return !(root);
	}

	void insert(int key) {
		BinomHeap inHeap(key);
		this->meld(inHeap);
	}

	Node * searchMin() {
		Node * currentMin = this->root;
		Node * currentNode = this->root;
		while (currentNode->rightBroth != nullptr) {
			currentNode = currentNode->rightBroth;
			if (currentNode->key < currentMin->key) {
				currentMin = currentNode;
			}
		}
		return currentMin;
	}

	int getMin() {
		return searchMin()->key;
	}
	int extractMin() {
		Node * deleted = searchMin();

		int res = deleted->key;

		if (deleted->leftBroth != nullptr)
			deleted->leftBroth->rightBroth = deleted->rightBroth;

		if (deleted->rightBroth != nullptr)
			deleted->rightBroth->leftBroth = deleted->leftBroth;


		if (root == deleted) root = deleted->rightBroth;
		if (deleted->leftChild != nullptr) {
			BinomHeap inHeap(deleted->leftChild);
			this->meld(inHeap);
		}

		deleted->leftChild = nullptr;
		deleted->rightChild = nullptr;
		deleted->leftBroth = nullptr;
		deleted->rightBroth = nullptr;

		destruct(deleted);

		return res;
	}

	void meld(IHeap& h) {
		BinomHeap & bh = dynamic_cast<BinomHeap&>(h);

		if (bh.empty()) return;
		if (this->empty()) {
			this->root = bh.root;
			bh.root = nullptr;
			return;
		}

		Node * currL = this->root;
		Node * currR = bh.root;
		this->root = nullptr;
		bh.root = nullptr;

		Node * startRes;
		Node * res;
		if (currL->degree < currR->degree) {
			res = currL;
			currL = currL->rightBroth;
		}
		else {
			res = currR;
			currR = currR->rightBroth;
		}
		startRes = res;

		while (currL != nullptr && currR != nullptr) {
			if (currL->degree < currR->degree) {
				res->rightBroth = currL;
				res->rightBroth->leftBroth = res;
				res = res->rightBroth;
				currL = currL->rightBroth;
			}
			else {
				res->rightBroth = currR;
				res->rightBroth->leftBroth = res;
				res = res->rightBroth;
				currR = currR->rightBroth;
			}
		}
		while (currL != nullptr) {
			res->rightBroth = currL;
			res->rightBroth->leftBroth = res;
			res = res->rightBroth;
			currL = currL->rightBroth;
		}
		while (currR != nullptr) {
			res->rightBroth = currR;
			res->rightBroth->leftBroth = res;
			res = res->rightBroth;
			currR = currR->rightBroth;
		}

		Node * currRoot = startRes;
		while (currRoot != nullptr && currRoot->rightBroth != nullptr) {
			if (currRoot->degree == currRoot->rightBroth->degree) {
				if (currRoot->key < currRoot->rightBroth->key) {
					swapNode(currRoot, currRoot->rightBroth);
					currRoot = currRoot->leftBroth;
				}

				if (currRoot->leftBroth != nullptr)
					currRoot->leftBroth->rightBroth = currRoot->rightBroth;
				currRoot->rightBroth->leftBroth = currRoot->leftBroth;
				currRoot->leftBroth = nullptr;

				Node * rB = currRoot->rightBroth;
				currRoot->rightBroth = nullptr;
				if (rB->rightChild != nullptr) {
					rB->rightChild->rightBroth = currRoot;
					rB->rightChild->rightBroth->leftBroth = rB->rightChild;
					rB->rightChild = rB->rightChild->rightBroth;
				}
				else {
					rB->leftChild = currRoot;
					rB->rightChild = currRoot;
				}

				rB->degree *= 2;

				if (rB->leftBroth == nullptr) {
					startRes = rB;
				}
				currRoot = rB;

				if (currRoot->rightBroth != nullptr && currRoot->degree > currRoot->rightBroth->degree) {
					swapNode(currRoot, currRoot->rightBroth);
					currRoot = currRoot->leftBroth;
				}
				if (currRoot->rightBroth != nullptr && currRoot->degree == currRoot->rightBroth->degree) {
					continue;
				}
			}
			if (currRoot->rightBroth != nullptr) {
				currRoot = currRoot->rightBroth;
			}
		}
		while (currRoot->leftBroth != nullptr) {
			currRoot = currRoot->leftBroth;
		}
		root = currRoot;
	}
}; 
