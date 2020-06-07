#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <cstdio>
#include <algorithm>

// Interface

template<typename T>
void serialize(T value, std::ostream& out) {
	out.write(reinterpret_cast<char*>(&value), sizeof(T));
}

template<typename T>
T deserialize(std::istream& in) {
	T value;
	in.read((char*)&value, sizeof(T));
	return value;
}

template<typename T>
class SerializeIterator {
public:
	typedef void value_type;
	typedef void difference_type;
	typedef void pointer;
	typedef void reference;
	typedef std::output_iterator_tag iterator_category;


	explicit SerializeIterator(std::ostream& stream) :
		stream_(&stream)
	{}

	SerializeIterator& operator=(const T& value) {
		serialize(value, *stream_);
		return *this;
	}
	SerializeIterator& operator*() { return *this; } // does nothing
	SerializeIterator& operator++() { return *this; } // does nothing
	SerializeIterator& operator++(int) { return *this; } // does nothing

private:
	std::ostream* stream_;
};

template<typename T>
class DeserializeIterator {
public:
	typedef T value_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T& reference;
	typedef std::input_iterator_tag iterator_category;

	DeserializeIterator() {
		isEnd = true;
	}
	explicit DeserializeIterator(std::istream& stream) :
		stream_(&stream),
		isEnd_(false)
	{
		value_ = deserialize<value_type>(*stream_);
	}

	DeserializeIterator(const DeserializeIterator& deserializer) :
		stream_(deserializer.stream_),
		isEnd_(deserializer.isEnd_),
		value_(deserializer.value_)
	{}

	const T& operator*() const {
		return value_;
	}
	const T& operator->() const {
		return value_;
	}


	DeserializeIterator& operator++() {
		value_ = deserialize<value_type>(*stream_);
		if (stream_->eof()) {
			isEnd_ = true;
		}
		return *this;
	}

	DeserializeIterator operator++(int) {
		DeserializeIterator<T> copy = *this;
		++(*this);
		return copy;
	}


	bool isEnd() const {
		return isEnd_;
	}

private:
	std::istream* stream_;
	T value_;
	bool isEnd_;
};


template<typename T>
bool operator==(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second) {
	if (first->isEnd_ == second->isEnd_
		&& first->stream_ == second->stream_
		&& first->value_ == second->value_) return true;
	return false;
}

template<typename T>
bool operator!=(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second) {
	return !(first == second);
}

std::string tempFilename();

template<typename InputIter, typename OutputIter, typename Merger>
class ExternalAlgoritm {
public:
	typedef typename std::iterator_traits<InputIter>::value_type value_type;

	ExternalAlgoritm() {}

	ExternalAlgoritm(InputIter begin, InputIter end,
		size_t size, size_t maxObjectsInMemory,
		OutputIter out) :
		begin_(begin),
		end_(end),
		size_(size),
		maxObjectsInMemory_(maxObjectsInMemory),
		out_(out)
	{
		fstreams_ = new std::fstream();
	}

	~ExternalAlgoritm() {
		delete fstreams_;
		for (size_t i = 0; i < filenames_.size(); ++i)
			remove(filenames_[i].c_str());
	}

	void run() {
		std::vector <value_type> data;

		for (size_t i = 0; i <= size_ / maxObjectsInMemory_ && begin_ != end_; ++i) {
			int count = 0;
			while (begin_ != end_ && count < maxObjectsInMemory_) {
				data.push_back(*begin_);
				++begin_;
				count++;
			}
			this->prepare(data);
			filenames_.push_back(tempFilename());
			fstreams_->open(filenames_.back(), std::fstream::out | std::ios::binary);

			SerializeIterator<value_type> serializeIt(*fstreams_);
			for (size_t i = 0; i < data.size(); ++i) {
				serializeIt = data[i];
			}

			data.clear();
			data.resize(0);
			fstreams_->close();
		}

		std::vector<DeserializeIterator<value_type> > deserializers;
		std::vector<std::ifstream> openedStreams(filenames_.size());

		for (size_t i = 0; i < filenames_.size(); ++i) {
			openedStreams[i].open(filenames_[i], std::ifstream::in | std::ifstream::binary);
			deserializers.push_back(DeserializeIterator<value_type>(openedStreams[i]));
		}

		Merger merger(deserializers);
		while (merger.hasNext()) {
			out_ = merger.next();
		}

		for (size_t i = 0; i < openedStreams.size(); ++i) {
			openedStreams[i].close();
		}
	}

private:
	virtual void prepare(std::vector<value_type>& data) = 0;

	InputIter begin_;
	InputIter end_;
	size_t size_;
	size_t maxObjectsInMemory_;
	OutputIter out_;

	size_t countOfFiles_;
	std::fstream* fstreams_;
	std::vector<std::string> filenames_;
};


template<class T>
struct DeserializerCompare {
	bool operator()(const DeserializeIterator<T>& first, const DeserializeIterator<T>& second)
	{
		return *first > *second;
	}
};

template<class T>
class SortMerger {
public:
	explicit SortMerger(const std::vector<DeserializeIterator<T> >& deserializers) :
		deserializers_(deserializers),
		hasNext_(true)
	{
		std::make_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());
	}

	bool hasNext() const {
		return hasNext_;
	}

	T next() {
		T value;

		std::pop_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());
		value = *deserializers_.back();
		++deserializers_[deserializers_.size() - 1];

		if (deserializers_[deserializers_.size() - 1].isEnd()) 
			deserializers_.pop_back();
		else
			std::push_heap(deserializers_.begin(), deserializers_.end(), DeserializerCompare<T>());

		if (deserializers_.empty()) hasNext_ = false;

		return value;
	}

private:
	std::vector<DeserializeIterator<T> > deserializers_;
	bool hasNext_;
};

template<typename InputIter, typename OutputIter>
class ExternalSort : public ExternalAlgoritm<
	InputIter, OutputIter, SortMerger<typename std::iterator_traits<InputIter>::value_type> > {
public:
	typedef ExternalAlgoritm<
		InputIter, OutputIter, SortMerger<typename std::iterator_traits<InputIter>::value_type> > Base;

	ExternalSort(InputIter begin, InputIter end,
		size_t size, size_t maxObjectsInMemory,
		OutputIter out) : Base(begin, end, size, maxObjectsInMemory, out)
	{}

private:
	virtual void prepare(std::vector<typename Base::value_type>& container) {
		std::make_heap(container.begin(), container.end());
		std::sort_heap(container.begin(), container.end());
	}
};


template<class T>
class ReverseMerger {
public:
	explicit ReverseMerger(const std::vector<DeserializeIterator<T> >& deserializers) :
		deserializers_(deserializers),
		hasNext_(true),
		index(0)
	{
		std::reverse(deserializers_.begin(), deserializers_.end());
	}

	bool hasNext() const {
		return hasNext_;
	}

	T next() {
		T value;
		value = *(deserializers_[index]);
		++(deserializers_[index]);
		if (deserializers_[index].isEnd()) {
			index++;
		}

		if (index >= deserializers_.size()) {
			hasNext_ = false;
		}

		return value;
	}

private:
	std::vector<DeserializeIterator<T> > deserializers_;
	bool hasNext_;
	int index;
};

template<typename InputIter, typename OutputIter>
class ExternalReverse : public ExternalAlgoritm<
	InputIter, OutputIter, ReverseMerger<typename std::iterator_traits<InputIter>::value_type> > {
public:
	typedef ExternalAlgoritm<
		InputIter, OutputIter, ReverseMerger<typename std::iterator_traits<InputIter>::value_type> > Base;


	ExternalReverse(InputIter begin, InputIter end,
		size_t size, size_t maxObjectsInMemory,
		OutputIter out) : Base(begin, end, size, maxObjectsInMemory, out)
	{}

private:
	virtual void prepare(std::vector<typename Base::value_type>& container) {
		std::reverse(container.begin(), container.end());
	}

};

// Implementation

std::string tempFilename() {
	static int fileNumber = 0;
	std::string fileName = "file" + std::to_string(fileNumber) + ".txt";
	fileNumber++;
	return fileName;
}

int main()
{
	std::ifstream ifs("input.txt");
	std::ofstream ofs("output.txt");
	size_t type, count, max;
	ifs >> type >> count >> max;

	if (type == 1) {
		ExternalSort<
			std::istream_iterator<int>,
			std::ostream_iterator<int>
		> alg(
			std::istream_iterator<int>(ifs), std::istream_iterator<int>(),
			count, max,
			std::ostream_iterator<int>(ofs, " "));
		alg.run();
	}
	else {
		ExternalReverse<
			std::istream_iterator<int>,
			std::ostream_iterator<int>
		> alg(
			std::istream_iterator<int>(ifs), std::istream_iterator<int>(),
			count, max,
			std::ostream_iterator<int>(ofs, " "));
		alg.run();
	}
	return 0;
}
