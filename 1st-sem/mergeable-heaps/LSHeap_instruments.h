#include <algorithm>

struct LNode {
	int key;
	LNode * left;
	LNode * right;
	int range;

	LNode() {
		key = 0;
		range = 0;
		left = right = nullptr;
	}

	explicit LNode(int k) {
		key = k;
		range = 0;
		left = right = nullptr;
	}
};

struct SNode {
	int key;
	SNode * left;
	SNode * right;

	SNode() {
		key = 0;
		left = right = nullptr;
	}
	SNode(int k) {
		key = k;
		left = right = nullptr;
	}
};

struct LMerger {
	LNode * merge(LNode * l, LNode * r) {
		if (l == nullptr) {
			return  r;
		}
		if (r == nullptr) {
			return  l;
		}
		if (l->key > r->key) {
			LNode * tmp = l;
			l = r;
			r = tmp;
		}

		l->right = merge(l->right, r);
		if (l->right != nullptr && l->left != nullptr && l->right->range > l->left->range
			|| l->left == nullptr && l->right != nullptr) {
			LNode * tmp = l->left;
			l->left = l->right;
			l->right = tmp;
		}

		if (l->left == nullptr && l->right == nullptr)
			l->range = 1;
		else if (l->left == nullptr)
			l->range = l->right->range + 1;
		else if (l->right == nullptr)
			l->range = l->left->range + 1;
		else
			l->range = std::min(l->left->range, l->right->range) + 1;


		return  l;
	}
};

struct SMerger {
	SNode * merge(SNode * l, SNode * r) {
		if (l == nullptr) {
			return  r;
		}
		if (r == nullptr) {
			return  l;
		}
		if (l->key > r->key) {
			SNode * tmp = l;
			l = r;
			r = tmp;
		}

		l->right = merge(l->right, r);

		SNode * tmp = l->left;
		l->left = l->right;
		l->right = tmp;

		SNode * nL = l;
		return  nL;
	}
};
