#include "IHeap.h"

template <class Node, class Merger>
class LeftSkewHeap : public IHeap {
private:
	Node * root_;

	void destruct(Node * deleted) {
		if (deleted == nullptr) return;
		destruct(deleted->left);
		destruct(deleted->right);
		delete deleted;
	}
public:
	LeftSkewHeap() {
		root_ = nullptr;
	}
	explicit LeftSkewHeap(int key) {
		Node * n = new Node(key);
		root_ = n;
	}
	explicit LeftSkewHeap(Node * n) {
		root_ = n;
	}
	~LeftSkewHeap() {
		destruct(root_);
	}

	bool empty() const {
		return !(root_);
	}

	void insert(int key) {
		LeftSkewHeap inHeap(key);
		this->meld(inHeap);
	}

	int getMin() {
		return root_->key;
	}

	int extractMin() {
		int res = root_->key;

		Node * deleted = root_;
		if (root_->left == nullptr && root_->right == nullptr) root_ = nullptr;
		else if (root_->left == nullptr) root_ = root_->right;
		else if (root_->right == nullptr) root_ = root_->left;
		else {
			LeftSkewHeap lh(root_->right);
			root_ = root_->left;
			meld(lh);

		}
		deleted->right = nullptr;
		deleted->left = nullptr;
		destruct(deleted);

		return res;
	}

	void meld(IHeap & h) {
		LeftSkewHeap & lh = dynamic_cast<LeftSkewHeap&>(h);
		Node * currRoot = lh.root_;
		lh.root_ = nullptr;
		root_ = Merger().merge(root_, currRoot);
	}

};
