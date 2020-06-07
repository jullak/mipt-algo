void swap(int* l, int* r);
void reverseMassive(int* m, int start, int finish);

class Permutation {
private:
	int size = 0;
	int * permutation;

public:
	Permutation() {
		this->size = 0;
	}
	Permutation(const unsigned int& n) {
		this->size = n;
		this->permutation = new int[size];
		for (int i = 0; i < size; ++i) {
			this->permutation[i] = i;
		}
	}
	Permutation(const unsigned int& n, int* m) {
		this->size = n;
		this->permutation = new int[size];
		for (int i = 0; i < size; ++i) {
			this->permutation[i] = m[i];
		}
	}
	Permutation(const Permutation& p) {
		if (this->size != 0) {
			delete[] this->permutation;
			this->size = 0;
		}
		this->size = p.size;
		this->permutation = new int[size];
		for (int i = 0; i < size; ++i) {
			this->permutation[i] = p.permutation[i];
		}
	}
	~Permutation() {
		if (this->size != 0)
			delete[] permutation;
		size = 0;
	}

	Permutation& operator = (const Permutation& p) {
		if (this->size != 0) {
			delete[] this->permutation;
			this->size = 0;
		}
		this->size = p.size;
		this->permutation = new int[size];
		for (int i = 0; i < size; ++i) {
			this->permutation[i] = p.permutation[i];
		}

		return *this;
	}

	const int& operator[] (int index){
		return this->permutation[index];
	}
	 
	Permutation& operator *= (const Permutation& right) {
		Permutation res(size);
		for (int i = 0; i < size; ++i) {
			res.permutation[i] = this->permutation[right.permutation[i]];
		}
		*this = res;

		return *this;
	}
	Permutation operator * (const Permutation& right) {
		Permutation res(size);
		for (int i = 0; i < size; ++i) {
			res.permutation[i] = this->permutation[right.permutation[i]];
		}
		return res;
	}

	Permutation& operator ++() {
		for (int i = this->size - 1; i > 0; --i) {
			if (this->permutation[i - 1] < this->permutation[i]) {
				int minInd = this->size-1;
				bool flag = false;
				for (int j = this->size - 1; j >= i; --j) {
					if (this->permutation[i - 1] < this->permutation[j] && (this->permutation[j] <= this->permutation[minInd] || !flag)) {
						minInd = j;
						flag = true;
					}
				}
				swap(&this->permutation[i - 1], &this->permutation[minInd]);
				reverseMassive(this->permutation, i, size - 1);
				break;
			}
		}
		return *this;
	}
	Permutation operator ++(int) {
		Permutation postPer = *this;
		++*this;
		return postPer;
	}

	Permutation& operator --() {

		for (int i = this->size - 1; i > 0; --i) {
			if (this->permutation[i - 1] > this->permutation[i]) {
				int minInd = this->size - 1;
				bool flag = false;
				for (int j = this->size - 1; j >= i; --j) {
					if (this->permutation[i - 1] > this->permutation[j] && (this->permutation[j] >= this->permutation[minInd] || !flag)) {
						minInd = j;
						flag = true;
					}
				}
				swap(&this->permutation[i - 1], &this->permutation[minInd]);
				reverseMassive(this->permutation, i, size - 1);
				break;
			}
		}
		return *this;
	}
	Permutation operator --(int) {
		Permutation postPer = *this;
		--(*this);
		return postPer;
	}

	Permutation inverse();
	Permutation next();
	Permutation previous();
	void operator() (int* m);

	bool operator == (Permutation per);
	bool operator <= (Permutation per);

	bool operator != (Permutation per);
	bool operator < (Permutation per);
	bool operator > (Permutation per);
	bool operator >= (Permutation per);

	bool isOdd();
	bool isEven();
	Permutation pow(int degree);
};

bool Permutation::operator == (Permutation per) {
	for (int i = 0; i < this->size; ++i) {
		if ((*this)[i] != per[i])
			return false;
	}
	return true;
}
bool Permutation::operator <= (Permutation per) {
	for (int i = 0; i < this->size; ++i) {
		if ((*this)[i] < per[i])
			return true;
		else if ((*this)[i] > per[i]) {
			return false;
		}
	}
	return true;
}

bool Permutation::operator != (Permutation per) {
	return !(*this == per);
}
bool Permutation::operator < (Permutation per) {
	return ((*this <= per) && !((*this) == per));
}
bool Permutation::operator > (Permutation per) {
	return !(*this <= per);
}
bool Permutation::operator >= (Permutation per) {
	return (((*this) == per) || ((*this) > per));
}


Permutation Permutation::inverse() { 
	Permutation res(this->size);

	for (int i = 0; i < size; i++) {
		res.permutation[this->permutation[i]] = i;
	}

	return res;
}
Permutation Permutation::next() {
	Permutation res = *this;
	return ++res;
}
Permutation Permutation::previous() {
	Permutation res = *this;
	--res;
	return res;
}
void Permutation::operator() (int* m) {
	Permutation inversePer = this->inverse();

	int * permutationM = new int[this->size];
	for (int i = 0; i < this->size; ++i) {
		permutationM[i] = m[inversePer[i]];
	}

	for (int i = 0; i < this->size; ++i) {
		m[i] = permutationM[i];
	}
	delete[] permutationM;
}

bool Permutation::isOdd() {
	return !this->isEven();
}

bool Permutation::isEven() {
	int result = 1;

	bool * inCircle = new bool[this->size];

	for (int i = 0; i < this->size; ++i) {
		inCircle[i] = false;
	}

	for (int i = 0; i < this->size; ++i) {
		if (!inCircle[i]) {
			int ptr = this->permutation[i];
			int count = 1;
			while (ptr != i) {
				inCircle[ptr] = true;
				ptr = this->permutation[ptr];
				count++;
			}

			if ((count - 1) % 2) result *= -1;
		}
	}

	delete[] inCircle;
	return (result > 0 ? true : false);
}
Permutation Permutation::pow(int degree) {
	bool sign = false;
	Permutation res(this->size);
	if (degree == 0) {
		return res;
	}
	if (degree < 0) {
		sign = true;
		degree = -degree;
	}

	bool * inCircle = new bool[this->size];
	for (int i = 0; i < this->size; ++i) {
		inCircle[i] = false;
	}

	int * circle = new int[this->size];
	int oldPtr = 0;
	int ptr = 0;
	for (int i = 0; i < this->size; ++i) {
		if (!inCircle[i]) {
			int ptrInPer = this->permutation[i];
			int count = 1;
			circle[ptr] = ptrInPer;
			while (ptrInPer != i) {
				inCircle[ptrInPer] = true;
				ptrInPer = this->permutation[ptrInPer];

				ptr++;
				circle[ptr] = ptrInPer;
				count++;
			}

			int degreeInCircle = degree%count;
			for (int j = oldPtr; j <= ptr; ++j) {
				res.permutation[circle[j]] = circle[oldPtr + (j + degreeInCircle - oldPtr) % (ptr - oldPtr + 1)];
			}

			ptr++;
			oldPtr = ptr;
		}
	}


	delete[] inCircle;
	delete[] circle;
	if (sign) {
		return res.inverse();
	}
	return res;
}

void swap(int* l, int* r) {
	int t = *l;
	*l = *r;
	*r = t;
}
void reverseMassive(int* m, int start, int finish) {
	for (int i = start, j = finish; i <= (start + finish) / 2; ++i, --j) {
		swap(&m[i], &m[j]);
	}
}
