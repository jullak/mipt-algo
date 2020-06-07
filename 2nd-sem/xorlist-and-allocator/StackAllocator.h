#pragma once

template <typename T>
class StackAllocator
{
public:
	using value_type = T;

	StackAllocator() noexcept : 
		isAvialable_(0),
		currentAdress_(0),
		currentBlock_(-1) 
	{
		blocks_ = new T*[MAX_BLOCKS];
	}

	template <typename B>
	StackAllocator(const StackAllocator<B>& sa) noexcept : StackAllocator<T>() {}
	StackAllocator(const StackAllocator& sa) noexcept : StackAllocator<T>() {}

	~StackAllocator() noexcept {
		for (int i = 0; i < currentBlock_; ++i) {
			delete[] blocks_[i];
		}
		delete[] blocks_;
	}

	T* allocate(size_t size) {

		if (size > isAvialable_) {
			
			++currentBlock_;
			blocks_[currentBlock_] = new T[BLOCK_SIZE];
			currentAdress_ = 0;
			isAvialable_ = BLOCK_SIZE;
		}

		T* ptr = blocks_[currentBlock_] + currentAdress_;
		isAvialable_ -= size;
		currentAdress_ += size;

		return ptr;
	}

	void deallocate(T * obj, size_t size) {
		//do nothing
	}


	template <typename B> 
	struct  rebind
	{
		typedef StackAllocator<B> other;
	};

private:
	static const size_t BLOCKS_BYTE = 2048;
	static const size_t MAX_BLOCKS = 1024 * 32;
	static const size_t BLOCK_SIZE = BLOCKS_BYTE/sizeof(T);
	size_t isAvialable_;
	size_t currentAdress_;
	int currentBlock_;
	T** blocks_;
};

template <class T, class B>
bool operator== (const StackAllocator<T> & l, const StackAllocator<B> & r) noexcept
{
	return true;
}

template <class T, class B>
bool operator!= (const StackAllocator<T> & l, const StackAllocator<B> & r) noexcept
{
	return false;
}