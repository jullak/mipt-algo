#include <iostream>
#include <vector>
#include <string>

static const int BASE = 10;

std::vector <int> stringToVector(const std::string& s);
std::vector <int> intToVector(const int& n);
std::vector <int> reverseVector(std::vector <int> & v);

class BigInteger {
public:
	BigInteger() {
		sign = true;
		number.resize(1, 0);
		size = number.size();
	}
	BigInteger(const BigInteger& num) :
		sign(num.sign),
		number(num.number),
		size(num.number.size())
	{}
	BigInteger (const int& n) :
		sign(n >= 0),
		number(intToVector(n)),
		size(number.size())
	{}
	BigInteger(const std::string& s) :
		sign(s[0] != '-'),
		number(stringToVector(s)),
		size(number.size())
	{}
	BigInteger(const std::vector <int> & v) :
		sign(true),
		number(v),
		size(number.size())
	{}

	std::string toString() const;
	
	//BigInteger& operator = (const BigInteger& num);
	//BigInteger& operator = (const int& n);
	BigInteger operator -();

	BigInteger& operator += (const BigInteger& num);
	BigInteger& operator -= (const BigInteger& num);
	BigInteger& operator *= (const BigInteger& num);
	BigInteger& operator /= (const BigInteger& num);
	BigInteger& operator %= (const BigInteger& num);
	
	BigInteger& operator ++();
	BigInteger operator ++(int);
	BigInteger& operator --();
	BigInteger operator --(int);

	explicit operator bool() const {
		if (*this == 0)
			return false;
		return true;
	}

	bool isAbsoluteMin(const BigInteger& num) const;

private:
	bool sign;
	std::vector <int> number;
	size_t size;

	BigInteger& absolutePlus(const BigInteger& num);
	BigInteger& absoluteMinus(const BigInteger& num);
	
	friend bool operator == (const BigInteger& left, const BigInteger& right);
	friend bool operator <= (const BigInteger& left, const BigInteger& right);
};

bool operator == (const BigInteger& left, const BigInteger& right) {
	if ((left.sign == right.sign) && (left.size == right.size)) {
		for (size_t i = 0; i < left.size; ++i) {
			if (left.number[i] != right.number[i])
				return false;
		}
		return true;
	}

	else if (left.size == 1 && left.number[0] == 0 &&
		right.size == 1 && right.number[0] == 0)
		return true;

	return false;
}
bool operator <= (const BigInteger& left, const BigInteger& right) {
	if (left == 0 && right == 0)
		return true;

	if (left.sign && right.sign)
		return left.isAbsoluteMin(right);
	else if (!left.sign && !right.sign)
		return right.isAbsoluteMin(left);

	else if (left.sign && !right.sign)
		return false;
	else
		return true;
}

bool operator != (const BigInteger& left, const BigInteger& right) {
	return !(left == right);
}
bool operator > (const BigInteger& left, const BigInteger& right) {
	return !(left <= right);
}
bool operator < (const BigInteger& left, const BigInteger& right) {
	return ((left <= right) && !(left == right));
}
bool operator >= (const BigInteger& left, const BigInteger& right) {
	return ((left > right) || (left == right));
}


BigInteger operator + (const BigInteger& left, const  BigInteger& right) {
	BigInteger res = left;
	res += right;
	return res;
}
BigInteger operator - (const BigInteger& left, const BigInteger& right) {
	BigInteger res = left;
	res -= right;
	return res;
}
BigInteger operator * (const BigInteger& left, const BigInteger& right) {
	BigInteger res = left;
	res *= right;
	return res;
}
BigInteger operator / (const BigInteger& left, const BigInteger& right) {
	BigInteger res = left;
	res /= right;
	return res;
}
BigInteger operator % (const BigInteger& left, const BigInteger& right) {
	BigInteger res = left;
	res %= right;
	return res;
}

BigInteger BigInteger::operator-() {
	BigInteger minus = *this;
	minus.sign = !minus.sign;
	return minus;
}
BigInteger& BigInteger::operator += (const BigInteger& num) {
	if (this->sign && num.sign)
		this->absolutePlus(num);
	else if (!this->sign && !num.sign)
		*this = -(this->absolutePlus(num));

	else if (this->sign && !num.sign) {
		if (this->isAbsoluteMin(num)) {
			BigInteger copyR = num;
			*this = -copyR.absoluteMinus(*this);
		}
		else
			this->absoluteMinus(num);
	}

	else if (!this->sign && num.sign) {
		if (this->isAbsoluteMin(num)) {
			BigInteger copyR = num;
			*this = copyR.absoluteMinus(*this);
		}
		else
			*this = -(this->absoluteMinus(num));
	}

	this->size = this->number.size();
	if (this->size == 1 && this->number[0] == 0)
		this->sign = true;
	
	return *this;
}
BigInteger& BigInteger::operator -= (const BigInteger& num) {
	if (this->sign && num.sign) {
		if (*this >= num)
			this->absoluteMinus(num);
		else {
			BigInteger copyR = num;
			*this = -copyR.absoluteMinus(*this);
		}
	}

	else if (!this->sign && !num.sign) {
		if (this->isAbsoluteMin(num)) {
			BigInteger copyR = num;
			*this = copyR.absoluteMinus(*this);
		}
		else
			*this = -(this->absoluteMinus(num));
	}

	else if (this->sign && !num.sign)
		this->absolutePlus(num);
	else if (!this->sign && num.sign)
		*this = -(this->absolutePlus(num));

	this->size = this->number.size();

	if (this->size == 1 && this->number[0] == 0)
		this->sign = true;

	return *this;
}
BigInteger& BigInteger::operator *= (const BigInteger& num) {
	int execess = 0;
	int intermedRes = 0;
	BigInteger res;

	for (size_t i = 0; i < num.size; ++i) {
		for (size_t j = 0; j < this->size; ++j) {
			int multiply = this->number[j] * num.number[i];
			intermedRes = multiply + execess;

			if (j + i >= res.number.size())
				res.number.push_back(intermedRes%BASE);
			else {
				intermedRes += res.number[j + i];
				res.number[j + i] = intermedRes%BASE;
			}
			execess = intermedRes / BASE;
		}

		if (execess) {
			if (this->size + i >= res.number.size())
				res.number.push_back(execess%BASE);
			else
				res.number[this->size + i] += execess%BASE;
			execess /= BASE;
		}
	}

	while (res.number.size() > 1 && res.number.back() == 0) {
		res.number.pop_back();
	}

	if ((this->sign && !num.sign) || (!this->sign && num.sign))
		res = -(res);
	res.size = res.number.size();
	*this = res;

	return *this;
}
BigInteger& BigInteger::operator /= (const BigInteger& num) {
	bool finalSign = true;
	if ((!this->sign && num.sign) || (this->sign && !num.sign))
		finalSign = false;

	std::vector <int> result(1, 0);
	if (this->size < num.size) {
		*this = reverseVector(result);
		return *this;
	}

	BigInteger leftVal(this->number.back());
	leftVal.size = leftVal.number.size();
	BigInteger rightVal = num;
	if (!num.sign)
		rightVal = -rightVal;
	int ptr = this->size - 2;

	while (true) {
		bool flag = false;
		while (leftVal < rightVal && ptr >= 0) {
			if (flag) {
				result.push_back(0);
				flag = false;
			}
			leftVal.number.insert(leftVal.number.begin(), this->number[ptr]);
			leftVal.size = leftVal.number.size();
			flag = true;
			ptr--;
		}

		if (leftVal < rightVal && ptr < 0) {
			if (flag) {
				result.push_back(0);
				flag = false;
			}
			break;
		}
		flag = false;

		while (leftVal.number.size() > 1 && leftVal.number.back() == 0) {
			leftVal.number.pop_back();
		}
		leftVal.size = leftVal.number.size();

		int decRes = 0;
		for (int i = 1; i <= BASE; ++i) {
			if (i*rightVal > leftVal) {
				decRes = i - 1;
				break;
			}
		}

		leftVal.absoluteMinus(decRes*rightVal);
		result.push_back(decRes);
	}

	result = reverseVector(result);
	while (result.size() > 1 && result.back() == 0) {
		result.pop_back();
	}

	*this = result;
	this->sign = finalSign;

	return *this;
}
BigInteger& BigInteger::operator %= (const BigInteger& num) {
	*this = (*this - ((*this / num)*num));
	return *this;
}

BigInteger& BigInteger::operator ++() {
	return (*this += 1);
}
BigInteger BigInteger::operator ++(int) {
	BigInteger postNum = *this;
	++*this;
	return postNum;
}
BigInteger& BigInteger::operator --() {
	return (*this -= 1);
}
BigInteger BigInteger::operator --(int) {
	BigInteger postNum = *this;
	--*this;
	return postNum;
}


std::string BigInteger::toString() const {
	std::string result = "";
	if (!this->sign && *this != 0)
		result += "-";

	for (size_t i = this->size; i > 0; --i) {
		result += ('0' + this->number[i - 1]);
	}
	return result;
}
std::vector <int> stringToVector(const std::string& s) {
	std::vector <int> result;

	size_t fin = 0;
	if (s[0] == '-')
		fin++;

	for (size_t i = s.length(); i > fin; --i) {
		result.push_back(s[i - 1] - '0');
	}

	return result;
}
std::vector <int> intToVector(const int& n) {
	std::vector <int> result;

	int copyN = n;
	if (copyN == 0)
		result.push_back(0);
	else if (copyN < 0)
		copyN = -copyN;
	while (copyN > 0) {
		result.push_back(copyN % BASE);
		copyN /= BASE;
	}

	return result;
}
std::vector <int> reverseVector(std::vector <int> & v) {
	std::vector <int> result(v.size());

	for (size_t i = 0; i < v.size(); ++i) {
		result[v.size() - 1 - i] = v[i];
	}

	return result;
}

BigInteger& BigInteger::absolutePlus(const BigInteger& num) {
	size_t minSize = this->size < num.size ? this->size : num.size;

	int execess = 0;
	for (size_t i = 0; i < minSize; ++i) {
		this->number[i] += num.number[i] + execess;
		execess = this->number[i] / BASE;
		this->number[i] %= BASE;
	}

	for (size_t i = 0; i < (this->size + num.size - 2 * minSize); ++i) {
		if (this->size < num.size)
			this->number.push_back(num.number[minSize + i] + execess);
		else
			this->number[minSize + i] += execess;

		execess = this->number[minSize + i] / BASE;
		this->number[minSize + i] %= BASE;
	}

	while (execess > 0) {
		this->number.push_back(execess%BASE);
		execess /= BASE;
	}

	if (!this->sign)
		*this = -(*this);

	this->size = this->number.size();

	return *this;
}
BigInteger& BigInteger::absoluteMinus(const BigInteger& num) {
	int borrow = 0;
	for (size_t i = 0; i < this->size; ++i) {
		if (this->number[i] - borrow < (i < num.size ? num.number[i] : 0)) {
			this->number[i] = BASE + this->number[i] - borrow - (i < num.size ? num.number[i] : 0);
			borrow = 1;
		}
		else {
			this->number[i] = this->number[i] - borrow - (i < num.size ? num.number[i] : 0);
			borrow = 0;
		}
	}

	while (this->number.size() > 1 && this->number.back() == 0) {
		this->number.pop_back();
	}

	if (!this->sign)
		*this = - (*this);

	this->size = this->number.size();

	return *this;
}

bool BigInteger::isAbsoluteMin(const BigInteger& num) const {
	if (this->size > num.size)
		return false;

	else if (this->size == num.size) {
		for (size_t i = this->size; i > 0; --i) {
			if (this->number[i-1] < num.number[i-1])
				return true;
			if (this->number[i-1] > num.number[i-1])
				return false;
		}
	}

	return true;
}

std::istream& operator >> (std::istream& is, BigInteger& num) {
	std::string s;
	is >> s;
	num = s;
	return is;
}

std::ostream& operator << (std::ostream& of, const BigInteger& num) {
	of << num.toString();
	return of;
}
