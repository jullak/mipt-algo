#pragma once
#include <memory.h>
#include <iterator>
static const size_t MIN_DEQ_SIZE = 16;

template <typename T, typename B>
class BaseIterator;

template <typename TD>
class Deque {
private:
	TD * deq_;

	int head_;
	int tail_;
	size_t size_;
	size_t maxSize_;

	void resize(size_t newSize) {
		TD * newDeq = new TD[newSize];

		for (size_t i = 0; i < size_; ++i) {
			newDeq[i] = deq_[(head_ + i) % maxSize_];
		}
		delete[] deq_;

		deq_ = newDeq;
		head_ = 0;
		tail_ = size_;
		maxSize_ = newSize;
	}
public:
	template <typename T, typename B>
	friend class BaseIterator;

	Deque() :
		head_(0),
		tail_(0),
		size_(0),
		maxSize_(MIN_DEQ_SIZE)
	{
		deq_ = new TD[maxSize_];
	}
	Deque(const Deque& d) {
			deq_ = new TD[d.maxSize_];
			memcpy(deq_, d.deq_, d.maxSize_ * sizeof(TD));
			head_ = d.head_;
			tail_ = d.tail_;
			size_ = d.size_;
			maxSize_ = d.maxSize_;
	}
	Deque & operator = (const Deque& d) {
		if (this == &d) return *this;

		delete[] deq_;
		deq_ = new TD[d.maxSize_];
		memcpy(deq_, d.deq_, d.maxSize_ * sizeof(TD));
		head_ = d.head_;
		tail_ = d.tail_;
		size_ = d.size_;
		maxSize_ = d.maxSize_;
		return *this;
	}
	~Deque() {
		delete[] deq_;
	}

	bool empty() const {
		return !(size_);
	}
	size_t size() const {
		return size_;
	}

	void push_back(const TD & element) {
		if (size_ == maxSize_)
			resize(maxSize_ * 2);

		deq_[tail_] = element;
		tail_ = (tail_ + 1) % maxSize_;
		size_++;
	}
	void push_front(const TD& element) {
		if (size_ == maxSize_)
			resize(maxSize_ * 2);

		head_ = (head_ - 1 + maxSize_) % maxSize_;
		deq_[head_] = element;
		size_++;
	}

	void pop_back() {
		tail_ = (tail_ - 1 + maxSize_) % maxSize_;
		size_--;

		if (size_ * 4 < maxSize_ && maxSize_ / 2 >= MIN_DEQ_SIZE)
			resize(maxSize_ / 2);
	}
	void pop_front() {
		head_ = (head_ + 1) % maxSize_;
		size_--;

		if (size_ * 4 < maxSize_ && maxSize_ / 2 >= MIN_DEQ_SIZE)
			resize(maxSize_ / 2);
	}

	const TD& front() const {
		return deq_[head_];
	}
	TD& front() {
		return deq_[head_];
	}
	const TD& back() const {
		return deq_[(tail_ - 1 + maxSize_) % maxSize_];
	}
	TD& back() {
		return deq_[(tail_ - 1 + maxSize_) % maxSize_];
	}
	const TD& operator [] (const int& index) const {
		return deq_[(head_ + index) % maxSize_];
	}
	TD& operator [] (const int& index) {
		return deq_[(head_ + index) % maxSize_];
	}

	typedef BaseIterator<TD, Deque<TD>*> iterator;
	typedef BaseIterator<const TD, const Deque<TD>*> const_iterator;
	typedef std::reverse_iterator<BaseIterator<TD, Deque<TD>*>> reverse_iterator;
	typedef std::reverse_iterator<BaseIterator<const TD, const Deque<TD>*>> const_reverse_iterator;


	const_iterator begin() const {
		return cbegin();
	}
	iterator begin() {
		return iterator(0, this);
	}
	const_iterator cbegin() const {
		return const_iterator(0, this);
	}

	iterator end() {
		return iterator(size_, this);
	}
	const_iterator cend() const {
		return const_iterator(size_, this);
	}
	const_iterator end() const {
		return cend();
	}

	reverse_iterator rbegin() {
		return reverse_iterator(end());
	}
	const_reverse_iterator crbegin() const {
		return const_reverse_iterator(cend());
	}
	const_reverse_iterator rbegin() const {
		return crbegin();
	}

	reverse_iterator rend() {
		return reverse_iterator(begin());
	}
	const_reverse_iterator crend() const {
		return const_reverse_iterator(cbegin());
	}
	const_reverse_iterator rend() const {
		return crend();
	}

};

template <typename T, typename B>
class BaseIterator : public std::iterator<std::random_access_iterator_tag, T>{
private:
	int pos_;
	B deq_;
public:
	BaseIterator(const int& ind, B d) :
		pos_(ind),
		deq_(d)
	{}

	BaseIterator(const BaseIterator& it) :
		pos_(it.pos_),
		deq_(it.deq_)
	{}

	BaseIterator& operator = (const BaseIterator<T,B>& it) {
		if (this == &it) return *this;
		pos_ = it.pos_;
		deq_ = it.deq_;
		return *this;
	}


	BaseIterator& operator ++() {
		++pos_;
		return *this;
	}
	BaseIterator operator ++(int) {
		BaseIterator res = *this;
		++pos_;
		return res;
	}
	BaseIterator& operator --() {
		--pos_;
		return *this;
	}
	BaseIterator operator --(int) {
		BaseIterator res = *this;
		--pos_;
		return res;
	}

	BaseIterator& operator += (const int& p) {
		pos_ += p;
		return *this;
	}
	BaseIterator& operator -= (const int& p) {
		pos_ -= p;
		return *this;
	}

	T& operator [] (const int& ind) const {
		return deq_->deq_[(deq_->head_ + ind + deq_->maxSize_) % deq_->maxSize_];
	}
	T& operator *() const{
		return deq_->deq_[(deq_->head_ + pos_ + deq_->maxSize_) % deq_->maxSize_];
	}
	T * operator->() {
		return &(deq_->deq_[(deq_->head_ + pos_ + deq_->maxSize_) % deq_->maxSize_]);
	}

	int operator-(const BaseIterator& it) const {
		return pos_ - it.pos_;
	}
	bool operator == (const BaseIterator& rit) const {
		return (rit.pos_ == pos_ && rit.deq_ == deq_);
	}
	bool operator != (const BaseIterator& rit) const {
		return !(*this == rit);
	}
	bool operator < (const BaseIterator& rit)const  {
		return (pos_ < rit.pos_);
	}
	bool operator > (const BaseIterator& rit)const {
		return (!(*this < rit) && !(*this == rit));
	}
	bool operator <= (const BaseIterator& rit)const {
		return ((*this < rit) || (*this == rit));
	}
	bool operator >= (const BaseIterator& rit)const {
		return ((*this > rit) || (*this == rit));
	}

	BaseIterator operator + (int p) const {
		BaseIterator res = *this;
		res += p;
		return res;
	}
	BaseIterator operator - (int p) const {
		BaseIterator res = *this;
		res -= p;
		return res;
	}
	
};

template <typename T, typename B>
BaseIterator<T, B> operator + (int p, const BaseIterator<T, B> & it) {
	return it + p;
}
