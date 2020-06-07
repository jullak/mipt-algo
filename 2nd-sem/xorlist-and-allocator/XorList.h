#pragma once
#include <memory>
#include <utility>
#include <iterator>


template <class T, class Alloc>
class ListIterator;

template <class TNode, class Allocator = std::allocator<TNode>>
class XorList
{
public:
	template <class T, class Alloc>
	friend class ListIterator;

	using iterator = ListIterator<TNode, Allocator>;
	using reverse_iterator = std::reverse_iterator<ListIterator<TNode, Allocator>>;

	explicit XorList(const Allocator& alloc = Allocator()) :
		head_(nullptr),
		tail_(nullptr),
		size_(0),
		alloc_(AllocatorN(alloc))
	{}
	XorList(size_t count, const TNode& value = TNode(), const Allocator& alloc = Allocator()) :
		head_(nullptr),
		tail_(nullptr),
		size_(0),
		alloc_(AllocatorN(alloc))
	{
		for (size_t i = 0; i < count; ++i) {
			push_back(value);
		}
	}
	explicit XorList(const XorList<TNode, Allocator> & xl) {
		size_ = xl.size_;
		iterator it = begin();
		iterator endit = end();
		for (it; it != endit; ++it) {
			this->push_back(*it);
		}

	}
	explicit XorList(XorList<TNode, Allocator> && xl) :
		head_(xl.head_),
		tail_(xl.tail_),
		size_(xl.size_)
	{
		xl.head_ = xl.tail_ = nullptr;
		xl.size_ = 0;
	}

	XorList<TNode, Allocator>& operator= (const XorList<TNode, Allocator>& xl) {
		if (this != &xl) {
			~this();
			size_ = xl.size_;
			iterator it = begin();
			iterator endit = end();
			for (it; it != endit; ++it) {
				this->push_back(*it);
			}
		}
		return *this;
	}
	XorList<TNode, Allocator>& operator= (XorList<TNode, Allocator>&& xl) {
		if (this != &xl) {
			this->head_ = xl.head_;
			this->tail_ = xl.tail_;
			this->size_ = xl.size_;

			xl.head_ = xl.tail_ = nullptr;
			xl.size_ = 0;
			//+ copyAlloc
		}
		return *this;
	}
	~XorList() {
		Node * currentNode = head_;
		if (currentNode != nullptr) {
			Node * nextNode = (reinterpret_cast<Node*>(currentNode->getXorAdress() ^ 0));
			while (nextNode != nullptr) {
				unsigned int currentAdr = currentNode->getIntAdress();
				destructNode_(currentNode);
				currentNode = nextNode;
				nextNode = reinterpret_cast<Node*>(currentNode->getXorAdress() ^ currentAdr);
			}
			destructNode_(currentNode);
		}
	}

	void push_back(const TNode& el) {
		insert_(tail_, constructNode_(el));
	}
	void push_front(const TNode& el) {
		insert_(head_, constructNode_(el));
	}
	void push_back(TNode&& el) {
		insert_(tail_, constructNode_(std::move(el)));
	}
	void push_front(TNode&& el) {
		insert_(head_, constructNode_(std::move(el)));
	}

	void pop_back() {
		delete_(tail_);
	};
	void pop_front() {
		delete_(head_);
	};

	void insert_before(iterator it, const TNode& el) {
		iterator lit = it;
		insert_((--lit).getNode(), it.getNode(), constructNode_(el));
	}
	void insert_before(iterator it, TNode&& el) {
		iterator lit = it;
		insert_((--lit).getNode(), it.getNode(), constructNode_(std::move(el)));
	}
	void insert_after(iterator it, const TNode& el) {
		iterator rit = it;
		insert_(it.getNode(), (++rit).getNode(), constructNode_(el));
	}
	void insert_after(iterator it, TNode&& el) {
		iterator rit = it;
		insert_(it.getNode(), (++rit).getNode(), constructNode_(std::move(el)));
	}
	void erase(iterator it) {
		iterator del = it;
		iterator lit = --it;
		iterator rit = del;
		delete_(lit.getNode(), del.getNode(), (++rit).getNode());		
	}

	size_t size() const {
		return size_;
	}

	iterator begin() {
		return iterator(head_, 0);
	}
	iterator end() {
		unsigned int prev = 0; 
		if (tail_ != nullptr) {
			prev = tail_->getXorAdress() ^ 0;
		}
		return iterator(tail_, prev);
	}
	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	reverse_iterator rend() {
		return reverse_iterator(begin());
	}

private:
	class Node {
	public:
		template <class T, class Alloc>
		friend class ListIterator;

		Node() {}
		explicit Node(const Node& n) :
			element_(n.element_),
			xorRes_(n.xorRes_)
		{}
		explicit Node(Node&& n) :
			element_(std::move(n.element_)),
			xorRes_(n.xorRes_)
		{
			n.xorRes_ = nullptr;
		}
		Node& operator= (const Node& n) {
			if (this != &n) {
				this->element_ = n.element_;
				this->xorRes_ = n.xorRes_;
			}
			return *this;
		}
		Node& operator= (Node&& n) {
			if (this != &n) {
				this->element_ = n.element_;
				this->xorRes_ = n.xorRes_;

				delete n.element_;
				n.xorRes_ = nullptr;
			}
			return *this;
		}
		explicit Node(const TNode& el) :
			element_(el),
			xorRes_(nullptr)
		{}
		explicit Node(TNode&& el) :
			element_(std::move(el)),
			xorRes_(nullptr)
		{}
		~Node() {
			xorRes_ = nullptr;
		}

		unsigned int getIntAdress() {
			return reinterpret_cast<unsigned int>(this);
		}
		unsigned int getXorAdress() {
			return reinterpret_cast<unsigned int>(xorRes_);
		}
		void setXorAdress(unsigned int adr) {
			xorRes_ = reinterpret_cast<Node*>(adr);
		}
	private:
		TNode element_;
		Node* xorRes_;
	};

	using AllocatorN = typename Allocator::template rebind<Node>::other;
	using AllocTraits = typename std::allocator_traits<AllocatorN>;

	Node* head_;
	Node* tail_;
	size_t size_;
	AllocatorN alloc_;

	Node * constructNode_(const TNode& el) {
		Node * res = alloc_.allocate(1);
		AllocTraits::construct(alloc_, res, Node(el));
		return res;
	}
	Node * constructNode_(TNode&& el) {
		Node * res = alloc_.allocate(1);
		AllocTraits::construct(alloc_, res, Node(std::move(el)));
		return res;
	}

	void destructNode_(Node*& n) {
		AllocTraits::destroy(alloc_, n);
		alloc_.deallocate(n, 1);
	}

	void insert_(Node*& to, Node * newNode) {
		if (to == nullptr) {
			head_ = tail_ = newNode;
			++size_;
			return;
		}

		unsigned int adressTo = to->getIntAdress();
		newNode->setXorAdress(adressTo ^ 0);
		to->setXorAdress(0 ^ (to->getXorAdress()) ^ (newNode->getIntAdress()));
		to = newNode;
		++size_;
	}

	void insert_(Node*& l, Node*& r, Node * newNode) {
		if (l == nullptr) {
			insert_(r, newNode);
		}
		else if (r == nullptr) {
			insert_(l, newNode);
		}
		unsigned int lAdr = l->getIntAdress();
		unsigned int rAdr = r->getIntAdress();
		newNode->setXorAdress(lAdr^rAdr);
		l->setXorAdress(l->getXorAdress() ^ rAdr ^ newNode->getIntAdress());
		r->setXorAdress(r->getXorAdress() ^ lAdr ^ newNode->getIntAdress());

		++size_;
	}

	void delete_(Node*& to) {
		if (size_ == 1) {
			destructNode_(head_);
			--size_;
			head_ = tail_ = nullptr;
			return;
		}
		unsigned int adressXorTo = to->getXorAdress();
		Node * newTo = reinterpret_cast<Node*>(adressXorTo ^ 0);

		unsigned int adressPrev = newTo->getXorAdress() ^ to->getIntAdress();
		newTo->setXorAdress(adressPrev ^ 0);

		destructNode_(to);
		to = newTo;
		--size_;
	}

	void delete_(Node*& l, Node*& del, Node*& r) {
		if (l == nullptr || r == nullptr) {
			delete_(del);
		}

		unsigned int delAdress = del->getIntAdress();
		l->setXorAdress(delAdress ^ l->getXorAdress() ^ r->getIntAdress());
		r->setXorAdress(delAdress ^ r->getXorAdress() ^ l->getIntAdress());

		destructNode_(del);
		--size_;
	}
};

template <class T, class Alloc>
class ListIterator : std::iterator<std::bidirectional_iterator_tag, T> {
public:
	using ListNode = typename XorList<T, Alloc>::Node;

	ListIterator(ListNode * start, unsigned int adr) :
		currentNode_(start),
		prevNodeAdr_(adr)
	{}
	ListIterator(const ListIterator<T, Alloc>& l) :
		currentNode_(l.currentNode_),
		prevNodeAdr_(l.prevNodeAdr_)
	{}
	ListIterator& operator= (const ListIterator<T, Alloc>& l)
	{
		currentNode_ = l.currentNode_;
		prevNodeAdr_ = l.prevNodeAdr_;
	}
	~ListIterator() {
		currentNode_ = nullptr;
	}

	ListIterator& operator ++() {
		if (currentNode_ == nullptr) return *this;
		unsigned int currXorAdress = currentNode_->getXorAdress();
		unsigned int newPrew = currentNode_->getIntAdress();

		currentNode_ = reinterpret_cast<ListNode*>(currXorAdress ^ prevNodeAdr_);
		prevNodeAdr_ = newPrew;
		return *this;
	}
	ListIterator operator++ (int) {
		ListIterator res = *this;
		++(*this);
		return res;
	}
	ListIterator& operator-- () {
		if (currentNode_ == nullptr) return *this;
		if (prevNodeAdr_ == 0) {
			currentNode_ = nullptr;
			return *this;
		}
		ListNode * prevNode = reinterpret_cast<ListNode*>(prevNodeAdr_);
		unsigned int newPrew = prevNode->getXorAdress() ^ currentNode_->getIntAdress();

		currentNode_ = prevNode;
		prevNodeAdr_ = newPrew;
		return *this;
	}
	ListIterator operator-- (int) {
		ListIterator res = *this;
		--(*this);
		return res;
	}

	T& operator* () const {
		return currentNode_->element_;
	}
	T * operator-> () {
		return &(currentNode_->element_);
	}

	bool operator == (const ListIterator& rit) const {
		return (rit.currentNode_ == this->currentNode_ && rit.prevNodeAdr_ == this->prevNodeAdr_);
	}
	bool operator != (const ListIterator& rit) const {
		return !(*this == rit);
	}

	ListNode*& getNode() { return currentNode_; }

private:
	ListNode * currentNode_;
	unsigned int prevNodeAdr_;
};