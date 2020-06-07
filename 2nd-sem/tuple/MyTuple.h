#include <utility>

template <class... TArgs>
class MyTuple;

template<>
class MyTuple<> {

protected:
	MyTuple& getThis() {
		return *this;
	}

public:
	MyTuple() {}
	MyTuple(const MyTuple& t) {}
	MyTuple(MyTuple&& t) {}

	MyTuple& operator= (const MyTuple& t) {
		return *this;
	}

	
	MyTuple& operator= (MyTuple&& t) {
		return *this;
	}

	~MyTuple() = default;

	void swap(MyTuple&) {}

	template <class...UArgs>
	bool equal(const MyTuple<UArgs...>& t) const {
		return true;
	}
	template <class...UArgs>
	bool less(const MyTuple<UArgs...>& t) const {
		return false;
	}


	bool isEmpty = false;
};

template <class FArg, class... TailArgs>
class MyTuple<FArg, TailArgs...> : public MyTuple<TailArgs...> {

protected:
	using super = MyTuple<TailArgs...>;

	template <class Tuple1, class Tuple2>
	friend decltype(auto) twoCat(Tuple1&& t1, Tuple2&& t2);

	template <class UFArg, class... UTuples>
	explicit MyTuple(FArg&& val, UTuples&&... t) :
		super(std::forward<UTuples>(t)...),
		value_(std::forward<FArg>(val))
	{}

	MyTuple& getThis() {
		return *this;
	}

public:
	MyTuple() :
		super(),
		value_()
	{}
	
	explicit MyTuple(const FArg& val, const TailArgs&... tVal) :
		super(tVal...),
		value_(val)
	{}
	template <class ...UTailArgs>
	explicit MyTuple(const FArg& val, UTailArgs&&... tVal) :
		super(std::forward<UTailArgs>(tVal)...),
		value_(val)
	{}
	
	explicit MyTuple(FArg&& val, TailArgs&&... tVal) :
		super(std::move(tVal)...),
		value_(std::move(val))
	{}
	template <class ...UTailArgs>
	explicit MyTuple(FArg&& val, UTailArgs&&... tVal) :
		super(std::forward<UTailArgs>(tVal)...),
		value_(std::move(val))
	{}


	MyTuple(const MyTuple& t) :
		super(t),
		value_(t.value_)
	{}

	MyTuple(MyTuple&& t) :
		super(std::move(t)),
		value_(std::move(t.value_))
	{}
	
	MyTuple& operator= (const MyTuple& t) {
		if (this != &t) {
			value_ = t.value_;
			super::getThis() = t;
		}
		return *this;
	}
	MyTuple& operator= (MyTuple&& t) {
		if (this != &t) {
			value_ = std::move(t.value_);
			super::getThis()= std::move(t);
		}
		return *this;
	}

	~MyTuple() = default;

	template <class UTuple>
	void swap(UTuple&& t) {
		std::swap(value_, t.value_);
		super::swap(t);
	}

	template <class...UArgs>
	bool equal(const MyTuple<UArgs...>& t) const {
		return (value_ == t.value_ && super::equal(t));
	}
	template <class...UArgs>
	bool less(const MyTuple<UArgs...>& t) const {
		return (value_ < t.value_ || super::less(t));
	}
	
	super& getSuper(){
		return *(static_cast<super>(*this));
	}

	bool isEmpty = false;

	FArg value_;
};

template <class... Types>
MyTuple<typename std::decay_t<Types>...> makeTuple(Types&&... args) {
	return MyTuple<std::decay_t<Types>...>(std::forward<Types>(args)...);
}

/*---------------get by Ind----------------*/
template <int Ind, class... Types>
struct getIndStruct;

template <int Ind, class FArg, class ...TailArgs>
struct getIndStruct <Ind, FArg, TailArgs...> : public getIndStruct<Ind - 1, TailArgs...> {};

template <class FArg, class ...TailArgs> 
struct getIndStruct <0, FArg, TailArgs...> {
	using tupleType = MyTuple<FArg, TailArgs...>;
	using elementType = std::decay_t<FArg>;
};

template<int Ind, class... Types>
typename getIndStruct<Ind, Types...>::elementType&
get(MyTuple<Types...> & t) {
	using tupleType = typename getIndStruct<Ind, Types...>::tupleType;
	return (static_cast<tupleType&>(t)).value_;
}
template<int Ind, class... Types>
typename getIndStruct<Ind, Types...>::elementType const &
get(const MyTuple<Types...> & t) {
	using tupleType = typename getIndStruct<Ind, Types...>::tupleType;
	return (static_cast<tupleType&>(t)).value_;
}
template<int Ind, class... Types>
typename getIndStruct<Ind, Types...>::elementType&&
get(MyTuple<Types...> && t) {
	using tupleType = typename getIndStruct<Ind, Types...>::tupleType;
	//using typeElement = typename getIndStruct<Ind, Types...>::elementType&&;
	return std::move((static_cast<tupleType&>(t)).value_);
}
/*---------------end get by Ind----------------*/


/*---------------get by Type----------------*/
template <class Find, class... Types>
struct getTypeStruct;

template <class Find, class FArg, class ...TailArgs>
struct getTypeStruct <Find, FArg, TailArgs...> : public getTypeStruct<Find, TailArgs...> {};

template <class FArg, class ...TailArgs>
struct getTypeStruct <FArg, FArg, TailArgs...> {
	using tupleType = MyTuple<FArg, TailArgs...>;
	using elementType = std::decay_t<FArg>;
};

template<class Find, class... Types>
typename getTypeStruct<Find, Types...>::elementType&
get(MyTuple<Types...> & t) {
	using tupleType = typename getTypeStruct<Find, Types...>::tupleType;
	return (static_cast<tupleType&>(t)).value_;
}
template<class Find, class... Types>
typename getTypeStruct<Find, Types...>::elementType const&
get(const MyTuple<Types...> & t) {
	using tupleType = typename getTypeStruct<Find, Types...>::tupleType;
	return (static_cast<tupleType&>(t)).value_;
}
template<class Find, class... Types>
typename getTypeStruct<Find, Types...>::elementType&&
get(MyTuple<Types...> && t) {
	using tupleType = typename getTypeStruct<Find, Types...>::tupleType;
	//using typeElement = typename getTypeStruct<Find, Types...>::elementType&&;
	return std::move((static_cast<tupleType&>(t)).value_);
}
/*---------------end get by Type----------------*/


/*----------------operators-------------*/
template<class... TArgs, class... UArgs>
bool operator==(const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return l.equal(r);
}
template<class... TArgs, class... UArgs>
bool operator!=(const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return !(l.equal(r));
}

template<class... TArgs, class... UArgs>
bool operator<(const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return l.less(r);
}
template<class... TArgs, class... UArgs>
bool operator<=(const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return (l.less(r) || l.equal(r));
}
template<class... TArgs, class... UArgs>
bool operator > (const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return !(l <= r);
}
template<class... TArgs, class... UArgs>
bool operator >= (const MyTuple<TArgs...>& l, const MyTuple<UArgs...>& r) {
	return (l > r || l == r);
}
/*----------------end/operators-------------*/


/*---------------tuple concat----------------*/
/*template<class Tuple1, class Tuple2>
struct concatType {
	using type = void;
};

template <class...TailArgs1, class...TailArgs2> 
struct concatType<MyTuple<TailArgs1...>, MyTuple<TailArgs2...>>{
	using type = MyTuple<TailArgs1..., TailArgs2...>;
};

template<class Tuple>
struct cutTuple;

template <class FArg, class...TailArgs>
struct cutTuple<MyTuple<FArg, TailArgs...>> {
	typedef FArg firstType;
	typedef MyTuple<TailArgs...> type;
};*/



template <class Tuple1, class Tuple2>
decltype(auto) twoCat(Tuple1&& t1, Tuple2&& t2) {
	if (t1.isEmpty) {
		return std::forward<Tuple2>(t2);
	}
	//using resultTuple = typename concatType<Tuple1, Tuple2>::type;
	return decltype(auto)(std::forward< decltype(t1.value_) >(t1.value_),
		twoCat(t1.getSuper(), std::forward< Tuple2 >(t2)));
}
 


template <class FirstTuple, class... TailTuples>
decltype(auto) tupleCat(FirstTuple&& t1, TailTuples &&... ts) {
	return twoCat(std::forward<FirstTuple>(t1), tupleCat(std::forward<TailTuples>(ts)...));
}

template <class FirstTuple>
decltype(auto) tupleCat(FirstTuple&& t1) {
	return std::forward<FirstTuple>(t1);
}
/*---------------end tuple concat----------------*/