#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>

class Treap {
private:
	class Node {
	public:
		long long element;

		bool reversed;
		long long sum;
		long long add;
		long long addFlag;
		long long assign; 
		long long assignFlag;

		bool isIncreasing;
		bool isDecreasing;
		long long leftmost;
		long long rightmost;

		long long weight;
		long long priority;

		Node * left;
		Node * right;

		Node(int x) : element(x), reversed(false), sum(x), add(0), addFlag(-1), assign(0), assignFlag(-1),
			isIncreasing(true), isDecreasing(true), leftmost(x), rightmost(x), weight(1), priority(rand()),
			left(nullptr), right(nullptr) {}
		~Node() {
			left = right = nullptr;
		}


		void updateSuffixInf() {
			if (this == nullptr) return;

			if (left == nullptr && right == nullptr) {
				isDecreasing = true;
				isIncreasing = true;
				leftmost = rightmost = element;
				return;
			}

			if (left == nullptr) {
				if (right->isDecreasing && element >= right->leftmost) isDecreasing = true;
				else isDecreasing = false;

				if (right->isIncreasing && element <= right->leftmost) isIncreasing = true;
				else isIncreasing = false;

				leftmost = element;
				rightmost = right->rightmost;
			}
			else if (right == nullptr) {
				if (left->isDecreasing && element <= left->rightmost) isDecreasing = true;
				else isDecreasing = false;

				if (left->isIncreasing && element >= left->rightmost) isIncreasing = true;
				else isIncreasing = false;

				leftmost = left->leftmost;
				rightmost = element;
			}
			else {
				if (left->isDecreasing && right->isDecreasing && left->rightmost >= element && element >= right->leftmost) isDecreasing = true;
				else isDecreasing = false;
				if (left->isIncreasing && right->isIncreasing && left->rightmost <= element && element <= right->leftmost) isIncreasing = true;
				else isIncreasing = false;

				leftmost = left->leftmost;
				rightmost = right->rightmost;
			}

		}


		long long getWeight() const {
			if (this == nullptr) return 0;
			else return this->weight;
		}
		void updateWeight() {
			weight = left->getWeight() + right->getWeight() + 1;
		}

		long long getSum() const {
			if (this == nullptr) return 0;
			return sum; //+ add * this->getWeight();
		}
		void updateSum() {
			sum = element + left->getSum() + right->getSum();
		}

		void pushAssign() {
			if (this == nullptr) return;
			if (assignFlag == -1) return;
			if (addFlag != -1 && addFlag < assignFlag) this->pushAdd();

			element = assign;
			sum = assign * this->getWeight();
			leftmost = rightmost = assign;
			isIncreasing = isDecreasing = true;

			if (left != nullptr) { 
				left->assign = assign;
				left->assignFlag = assignFlag;
			}
			if (right != nullptr) {
				right->assign = assign;
				right->assignFlag = assignFlag;
			}

			assignFlag = -1;
		}

		void pushAdd() {
			if (this == nullptr) return;
			if (addFlag == -1) return;
			if (assignFlag != -1 && assignFlag < addFlag) this->pushAssign();

			element += add;
			leftmost += add;
			rightmost += add;
			sum += add * this->getWeight();

			if (left != nullptr) {
				if (addFlag > left->assignFlag && left->assignFlag > left->addFlag) left->add = add;
				else left->add += add;
				left->addFlag = addFlag;
			}
			if (right != nullptr) {
				if (addFlag > right->assignFlag && right->assignFlag > right->addFlag) right->add = add;
				else right->add += add;
				right->addFlag = addFlag;
			}

			add = 0;
			addFlag = -1;
		}
		void pushReversed() {
			if (this == nullptr) return;
			if (!reversed) return;

			std::swap(isDecreasing, isIncreasing);
			std::swap(leftmost, rightmost);
			std::swap(left, right);

			reversed = false;
			if (left != nullptr) left->reversed ^= true;
			if (right != nullptr) right->reversed ^= true;
		}
	};

	//ind + 1
	long long findIndForNext(Node * n) {
		if (n == nullptr) return 0;
		globalPush(n);

		if (n->right != nullptr && !n->right->isDecreasing) {
			return n->left->getWeight() + 1 + findIndForNext(n->right);
		}
		if (n->right != nullptr && n->element < n->right->leftmost) {
			return n->left->getWeight() + 1;
		}
		if (n->left != nullptr && n->element > n->left->rightmost) {
			return n->left->getWeight();
		}
		return findIndForNext(n->left);
	}
	long long findIndForPrev(Node * n) {
		if (n == nullptr) return 0;
		globalPush(n);

		if (n->right != nullptr && !n->right->isIncreasing) {
			return n->left->getWeight() + 1 + findIndForPrev(n->right);
		}
		if (n->right != nullptr && n->element > n->right->leftmost) {
			return n->left->getWeight() + 1;
		}
		if (n->left != nullptr && n->element < n->left->rightmost) {
			return n->left->getWeight();
		}
		return findIndForPrev(n->left);
	}


	void recalc(Node * n) {
		if (n == nullptr) return;

		n->updateWeight();
		n->updateSum();
		n->updateSuffixInf();
	}

	void pushAll(Node * n) {
		if (n == nullptr) return;

		n->pushReversed();
		n->pushAdd();
		n->pushAssign();

	}
	void globalPush(Node * n) {
		if (n == nullptr) return;
		pushAll(n);
		pushAll(n->left);
		pushAll(n->right);
	}


	//[key)
	std::pair <Node *, Node *> splitByKey(Node * node, int key) {
		globalPush(node);

		std::pair <Node *, Node *> result;

		if (node == nullptr) {
			result = std::make_pair(nullptr, nullptr);
		}

		else if (node->element <= key) {
			std::pair <Node *, Node *> splitR = splitByKey(node->right, key);
			node->right = splitR.first;
			recalc(node);
			result = std::make_pair(node, splitR.second);
		}
		else {
			std::pair <Node *, Node *> splitL = splitByKey(node->left, key);
			node->left = splitL.second;
			recalc(node);
			result = std::make_pair(splitL.first, node);
		}

		return result;
	}

	//[)key
	std::pair <Node *, Node *> split(Node * node, int key) {
		globalPush(node);

  		std::pair<Node*, Node*> result;

		if (node == nullptr) {
			result = std::make_pair(nullptr, nullptr);
		}

		else if (node->left->getWeight() + 1 <= key) {
			std::pair <Node *, Node *> splitR = split(node->right, key - node->left->getWeight() - 1);
			node->right = splitR.first;
			recalc(node);
			result = std::make_pair(node, splitR.second);
		}
		else {
			std::pair <Node *, Node *> splitL = split(node->left, key);
			node->left = splitL.second;
			recalc(node);
			result = std::make_pair(splitL.first, node);
		}
		
		return result;
	}
	Node * merge(Node * l, Node * r) {
		

		if (l == nullptr) return r;
		if (r == nullptr) return l;
		
		globalPush(l);
		globalPush(r);

		if (l->priority < r->priority) {
			r->left = merge(l, r->left);
			recalc(r);
			return r;
		}
		else {
			l->right = merge(l->right, r);
			recalc(l);
			return l;
		}
	}

	//обход
	void inOrder(Node * n) {
		if (n == nullptr) return;

		globalPush(n);

		inOrder(n->left);
		std::cout << n->element << " ";
		inOrder(n->right);
	}
	//удаление
	void postOrder(Node * n) {
		if (n == nullptr) return;

		postOrder(n->left);
		postOrder(n->right);
		delete n;
	}


	Node * root;
	long long comandNum;

public:
	Treap() : root(nullptr), comandNum(0) {}
	~Treap() {
		postOrder(root);
	}

	void printTreap() {
		inOrder(root);
	}

	void insertOnPos(long long ind, long long x) {
		Node * newNode = new Node(x);
		std::pair <Node *, Node *> spl = split(root, ind);
		root = merge(merge(spl.first, newNode), spl.second);
	}

	long long getSumOnSubsegment(long long lInd, long long rInd) {
		long long ans;
		std::pair <Node *, Node *> rightPart = split(root, rInd + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart.first, lInd);
		ans = leftPart.second->getSum();
		root = merge(merge(leftPart.first, leftPart.second), rightPart.second);

		return ans;
	}

	void deleteOnPos(long long ind) {
		std::pair <Node *, Node *> rightPart = split(root, ind + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart. first, ind);
		delete leftPart.second;
		root = merge(leftPart.first, rightPart.second);
	}

	void assignOnSubsegment(long long x, long long lInd, long long rInd) {
		std::pair <Node *, Node *> rightPart = split(root, rInd + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart.first, lInd);

		globalPush(leftPart.second);
 
		leftPart.second->assign = x;
		leftPart.second->assignFlag = comandNum;
		leftPart.second->pushAssign();

		root = merge(merge(leftPart.first, leftPart.second), rightPart.second);

		comandNum++;
	}

	void addOnSubSegment(long long x, long long lInd, long long rInd) {
		std::pair <Node *, Node *> rightPart = split(root, rInd + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart.first, lInd);

		globalPush(leftPart.second);


		if (comandNum > leftPart.second->assignFlag && leftPart.second->assignFlag > leftPart.second->addFlag) leftPart.second->add = x;
		else leftPart.second->add += x;
		leftPart.second->addFlag = comandNum;
		leftPart.second->pushAdd();

		globalPush(leftPart.second);

		root = merge(merge(leftPart.first, leftPart.second), rightPart.second);

		comandNum++;
	}

	void nextPermutation(long long lInd, long long rInd) {

		std::pair <Node *, Node *> rightPart = split(root, rInd + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart.first, lInd); //needs - leftPart.second
		globalPush(leftPart.second); 
		long long startInd = findIndForNext(leftPart.second); //max left

		if (startInd == 0) {
			leftPart.second->reversed = true;
			leftPart.second->pushReversed();

			globalPush(leftPart.second);

			root = merge(merge(leftPart.first, leftPart.second), rightPart.second);
			return;
		}

		std::pair <Node *, Node *> suffix = split(leftPart.second, startInd);
		std::pair <Node *, Node *> swapElement = split(suffix.first, suffix.first->getWeight() - 1);

		suffix.second->reversed = true;
		suffix.second->pushReversed();
		suffix.second->left->pushReversed();
		suffix.second->right->pushReversed();

		std::pair <Node *, Node *> nextMore = splitByKey(suffix.second, swapElement.second->element);
		std::pair <Node *, Node *> swapElementSuf = split(nextMore.second, 1);

		root = merge(merge(leftPart.first, merge(merge(swapElement.first, swapElementSuf.first), merge(nextMore.first, merge(swapElement.second, swapElementSuf.second)))), rightPart.second);
	};

	
	void prevPermutation(long long lInd, long long rInd) {

		std::pair <Node *, Node *> rightPart = split(root, rInd + 1);
		std::pair <Node *, Node *> leftPart = split(rightPart.first, lInd); //needs - leftPart.second
		globalPush(leftPart.second);

		long long startInd = findIndForPrev(leftPart.second);

		if (startInd == 0) {
			leftPart.second->reversed = true;
			leftPart.second->pushReversed();

			globalPush(leftPart.second);

			root = merge(merge(leftPart.first, leftPart.second), rightPart.second);
			return;
		}

		std::pair <Node *, Node *> suffix = split(leftPart.second, startInd);
		std::pair <Node *, Node *> swapElement = split(suffix.first, suffix.first->getWeight() - 1);

		std::pair <Node *, Node *> nextLess = splitByKey(suffix.second, swapElement.second->element - 1);
		std::pair <Node *, Node *> swapElementSuf = split(nextLess.first, nextLess.first->getWeight() - 1);

		suffix.second = merge(merge(swapElementSuf.first, swapElement.second), nextLess.second);

		suffix.second->reversed = true;
		suffix.second->pushReversed();
		suffix.second->left->pushReversed();
		suffix.second->right->pushReversed();

		root = merge(merge(leftPart.first, merge(merge(swapElement.first, swapElementSuf.second), suffix.second)), rightPart.second);
	}
	
};


int main() {
	srand(6);

	long long n, q;
	long long x, l, r;
	long long comand, pos;
	std::cin >> n;
	Treap treap;
	
	//start tree
	for (long long i = 0; i < n; ++i) {
		std::cin >> x;
		treap.insertOnPos(i, x);
	}


	std::cin >> q;
	for (long long i = 0; i < q; ++i) {
		std::cin >> comand;
		if (comand == 1) {
			std::cin >> l >> r;
			std::cout << treap.getSumOnSubsegment(l, r) << std::endl;
		}
		else if (comand == 2) {
			std::cin >> x >> pos;
			treap.insertOnPos(pos, x);
			//treap.printTreap();
		}
		else if (comand == 3) {
			std::cin >> pos;
			treap.deleteOnPos(pos);
			//treap.printTreap();
		}
		else if (comand == 4) {
			std::cin >> x >> l >> r;
			treap.assignOnSubsegment(x, l, r);
			//treap.printTreap();
		}
		else if (comand == 5) {
			std::cin >> x >> l >> r;
 			treap.addOnSubSegment(x, l, r);
			//treap.printTreap();
		}
		else if (comand == 6) {
			std::cin >> l >> r;
			treap.nextPermutation(l, r);
			//treap.printTreap();
			//std::cout << std::endl;
		}
		else if (comand == 7) {
			std::cin >> l >> r;
			treap.prevPermutation(l, r);
			//treap.printTreap();
			//std::cout << std::endl;
		}
	}
 	treap.printTreap();
	return 0;
}
