#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <queue>
#include <vector>

template <bool Direction, class T>
class EdgeIterator;

class Edge {
public:

	template <bool Direction, class T>
	friend class EdgeIterator;

	Edge() :
		vertexFrom_(0),
		vertexTo_(0),
		flow_(0),
		capacity_(0)
	{}

	explicit Edge(size_t from, size_t to, long long flow, long long capacity) :
		vertexFrom_(from),
		vertexTo_(to),
		flow_(flow),
		capacity_(capacity)
	{}

	size_t from() const {
		return vertexFrom_;
	}
	size_t to() const {
		return vertexTo_;
	}

	long long flow() const {
		return flow_;
	}

	long long getResidualCapacity() const {
		return this->capacity_ - this->flow_;
	}

	bool isSaturated() const {
		return this->capacity_ <= this->flow_;
	}

private:
	size_t vertexFrom_, vertexTo_;
	long long flow_, capacity_;

	void pushFlow_(long long flow) {
		this->flow_ += flow;
	}
};

class EdgeList {
public:
	template <bool B, class T>
	friend class EdgeIterator;

	enum Direction { FORWARD = 0, BACK = 1 };

	EdgeList() = default;
	explicit EdgeList(size_t vertexNumber) :
		vertexSize_(vertexNumber)
	{
		listHead_.resize(vertexNumber, -1);
	}

	void assign(size_t size) {
		listHead_.assign(size, -1);
		listNext_.clear();
		listPrevious_.clear();
	}

	void assign(size_t vertexNumber, size_t edgeNumber) {
		listHead_.assign(vertexNumber, -1);
		listNext_.assign(edgeNumber, -1);
		listPrevious_.assign(edgeNumber, -1);
	}

	void add(size_t vertex, size_t edgeIndex) {
		setPrevious_(vertex, edgeIndex);

		listNext_[edgeIndex] = listHead_[vertex];
		listHead_[vertex] = edgeIndex;
	}

	void push_back(size_t vertex, size_t edgeIndex) {
		setPrevious_(vertex, edgeIndex);

		listNext_.push_back(listHead_[vertex]);
		listHead_[vertex] = edgeIndex;
		listPrevious_.push_back(-1);
	}

	void deleteElement(size_t indexElement, size_t indexHead) {
		int previousEdge = listPrevious_[indexElement];
		int nextEdge = listNext_[indexElement];

		if (previousEdge == -1) {
			listHead_[indexHead] = nextEdge;
		}

		if (previousEdge != -1)
			listNext_[previousEdge] = nextEdge;

		if (nextEdge != -1)
			listPrevious_[nextEdge] = previousEdge;
	}

private:
	size_t vertexSize_;
	std::vector <int> listHead_;
	std::vector <int> listNext_;
	std::vector <int> listPrevious_;

	void setPrevious_(size_t vertex, size_t edgeIndex) {
		if (listHead_[vertex] != -1) {
			listPrevious_[listHead_[vertex]] = edgeIndex;
		}
	}
};

template <bool Direction, class T>
class EdgeIterator {
public:
	EdgeIterator() = default;

	explicit EdgeIterator(size_t vertexIndex, std::vector <T> * edges, EdgeList * container) :
		edges_(edges),
		container_(container)
	{
		currentEdgeIndex_ = container_->listHead_[vertexIndex];
		validateNextStep_();
	}

	bool valid() const {
		return currentEdgeIndex_ != -1;
	}

	EdgeIterator& operator ++() {
		currentEdgeIndex_ = container_->listNext_[currentEdgeIndex_];
		validateNextStep_();
		return *this;
	}
	EdgeIterator operator ++(int) {
		EdgeIterator iterator = *this;
		++(*this);
		return iterator;
	}

	EdgeIterator& operator --() {
		currentEdgeIndex_ = container_->listPrevious_[currentEdgeIndex_];
		validateNextStep_();
		*this;
	}
	EdgeIterator operator --(int) {
		EdgeIterator iterator = *this;
		--(*this);
		return iterator;
	}

	EdgeIterator getBackEdgeIterator() {
		EdgeIterator iterator = *this;

		iterator.currentEdgeIndex_ ^= 1;
		iterator.validateNextStep_();

		return iterator;
	}

	int getIndex() const {
		if (Direction) {
			return currentEdgeIndex_ ^ 1;
		}
		return currentEdgeIndex_;
	}
	int getBackIndex() const {
		if (Direction) {
			return currentEdgeIndex_;
		}
		return currentEdgeIndex_ ^ 1;
	}

	T& getBack() const {
		return (*edges_)[currentEdgeIndex_ ^ 1];
	}
	T * getBackPointer() {
		return &((*edges_)[currentEdgeIndex_ ^ 1]);
	}

	T& operator *() const {
		if (Direction) {
			return getBack();
		}
		return (*edges_)[currentEdgeIndex_];
	}
	T * operator ->() {
		if (Direction) {
			return getBackPointer();
		}
		return &((*edges_)[currentEdgeIndex_]);
	}

	void pushFlow(long long flow) {
		if (Direction) {
			flow = -flow;
		}
		(*edges_)[currentEdgeIndex_].pushFlow_(flow);
		(*edges_)[currentEdgeIndex_ ^ 1].pushFlow_(-flow);
	}

private:
	std::vector <T> * edges_;
	EdgeList * container_;

	int currentEdgeIndex_;
	int nextStep_;

	void validateNextStep_() {
		if (currentEdgeIndex_ != -1)
			nextStep_ = container_->listNext_[currentEdgeIndex_];
	}
};

class Network {
public:
	Network() :
		source_(0),
		sink_(0),
		vertexNumber_(0),
		edgeNumber_(0)
	{}

	explicit Network(size_t source, size_t sink, size_t vertexNumber) :
		source_(source),
		sink_(sink),
		vertexNumber_(vertexNumber),
		edgeNumber_(0)
	{
		edgeList_.assign(vertexNumber);
	}

	size_t sink() const {
		return sink_;
	}
	size_t source() const {
		return source_;
	}
	size_t vertexNumber() const {
		return this->vertexNumber_;
	}
	size_t edgesNumber() const {
		return edges_.size();
	}
	std::vector <Edge> edges() const {
		return edges_;
	}

	void addEdge(size_t from, size_t to, long long capacity, bool directed) {
		edges_.push_back(Edge(from, to, 0, capacity));
		edgeList_.push_back(from, edges_.size() - 1);

		long long backCapacity = directed ? 0 : capacity;
		edges_.push_back(Edge(to, from, 0, backCapacity));
		edgeList_.push_back(to, edges_.size() - 1);
	}

	void restore() {
		edgeList_.assign(vertexNumber_, edgeNumber_);
		for (size_t i = 0; i < edgeNumber_; ++i) {
			edgeList_.add(edges_[i].from(), i);
		}
	}

	const static long long INF = 1e9;

	using iterator = EdgeIterator <EdgeList::FORWARD, Edge>;
	using backIterator = EdgeIterator <EdgeList::BACK, Edge>;

	iterator begin(size_t vertexIndex) {
		return iterator(vertexIndex, &(edges_), &(edgeList_));
	}

	backIterator rbegin(size_t vertexIndex) {
		return backIterator(vertexIndex, &(edges_), &(edgeList_));
	}

protected:
	size_t source_, sink_;
	size_t vertexNumber_, edgeNumber_;

	std::vector <Edge> edges_;
	EdgeList edgeList_;
};

class LayeredNetwork {
public:
	LayeredNetwork() = default;
	explicit LayeredNetwork(Network * net)
	{
		reset(net);
	}

	size_t sink() const {
		return net_->sink();
	}
	size_t source() const {
		return net_->source();
	}
	size_t vertexNumber() const {
		return net_->vertexNumber();
	}
	size_t edgesNumber() const {
		return edges_.size();
	}

	std::vector <long long> excess() const {
		return excess_;
	}

	void addEdge(Network::iterator& it) {
		edges_.push_back(it);
		forwardEdges_.add(it->from(), edges_.size() - 1);

		edges_.push_back(it.getBackEdgeIterator());
		backEdges_.add(it->to(), edges_.size() - 1);
	}

	void setLayerNumber(Network::iterator& it) {
		layeredNumber_[it->to()] = layeredNumber_[it->from()] + 1;
	}

	bool inLayer(size_t index) const {
		return layeredNumber_[index] != -1;
	}

	bool correctEdge(Network::iterator& it) const {
		return layeredNumber_[it->from()] + 1 == layeredNumber_[it->to()];
	}

	bool sinkAvailable() const {
		return layeredNumber_[net_->sink()] != -1;
	}

	void setExcess(size_t vertexIndex, long long vertexExcess) {
		excess_[vertexIndex] = vertexExcess;
	}

	template <bool Direction, class T>
	EdgeIterator<Direction, T> eraseFromTraversal(EdgeIterator<Direction, T>& it) {
		forwardEdges_.deleteElement(it.getIndex(), (*it)->from());
		backEdges_.deleteElement(it.getBackIndex(), (*it)->to());
		++it;
		return it;
	}

	void build() {
		std::queue <size_t> queueBFS;
		queueBFS.push(net_->source());
		while (!queueBFS.empty()) {
			Network::iterator it = net_->begin(queueBFS.front());
			queueBFS.pop();

			for (; it.valid(); ++it) {
				if (!(it->isSaturated()) && !inLayer(it->to())) {
					queueBFS.push(it->to());
					setLayerNumber(it);
				}
				if (!(it->isSaturated()) && correctEdge(it)) {
					addEdge(it);
				}
			}
		}
	}

	void reset(Network * net) {
		net_ = net;
		layeredNumber_.assign(net->vertexNumber(), -1);
		layeredNumber_[net->source()] = 0;

		excess_.assign(net->vertexNumber(), 0);
		edges_.clear();

		forwardEdges_.assign(net->vertexNumber(), net->edgesNumber());
		backEdges_.assign(net->vertexNumber(), net->edgesNumber());

		build();
	}

	using iterator = EdgeIterator<EdgeList::FORWARD, Network::iterator>;
	using backIterator = EdgeIterator<EdgeList::BACK, Network::iterator>;

	iterator begin(size_t vertexIndex) {
		return iterator(vertexIndex, &edges_, &(forwardEdges_));
	}

	backIterator rbegin(size_t vertexIndex) {
		return backIterator(vertexIndex, &edges_, &(backEdges_));
	}

private:
	Network * net_;

	std::vector <Network::iterator> edges_;
	EdgeList forwardEdges_;
	EdgeList backEdges_;

	std::vector <long long> excess_;
	std::vector <int> layeredNumber_;
};

class MaxFlowProblemSolveAlgorithm {
public:
	MaxFlowProblemSolveAlgorithm() = default;
	explicit MaxFlowProblemSolveAlgorithm(Network * net) :
		network_(net)
	{}


	virtual void run() = 0;

	long long getMaxFlow() const {
		return countMaxFlow_();
	}

	void setNetwork(Network * net) {
		network_ = net;
		countMaxFlow_();
		reset_();
	}

protected:
	Network * network_;

	long long countMaxFlow_() const {
		long long maxFlow = 0;
		for (Network::iterator it = network_->begin(network_->source()); it.valid(); ++it) {
			maxFlow += it->flow();
		}
		return maxFlow;
	}

	virtual void reset_() = 0;
};

class BlockingFlowAlgorithm : public MaxFlowProblemSolveAlgorithm {
public:
	BlockingFlowAlgorithm() = default;
	explicit BlockingFlowAlgorithm(Network * net) :
		MaxFlowProblemSolveAlgorithm(net),
		layered_(new LayeredNetwork(net))
	{}

	void run() {
		buildLayered_();
		while (layered_->sinkAvailable()) {
			pushBlockingFlow_();
			buildLayered_();
		}
	}

protected:
	LayeredNetwork * layered_;

	virtual void reset_() = 0;
	virtual void pushBlockingFlow_() = 0;

	void buildLayered_() {
		layered_->reset(network_);
	}
};

class MalhotraKumarMaheshwariBlockingFlow : public BlockingFlowAlgorithm {
public:
	MalhotraKumarMaheshwariBlockingFlow() = default;
	explicit MalhotraKumarMaheshwariBlockingFlow(Network * net) :
		BlockingFlowAlgorithm(net)
	{}

private:
	std::vector <long long> potentialIn_;
	std::vector <long long> potentialOut_;
	std::vector <size_t> notAvailable_;

	void reset_() {
		notAvailable_.assign(network_->vertexNumber(), 0);
		potentialIn_.assign(network_->vertexNumber(), 0);
		potentialOut_.assign(network_->vertexNumber(), 0);
	}

	void pushBlockingFlow_() {
		reset_();
		countPotential_();

		while (!notAvailable_[layered_->sink()] && !notAvailable_[layered_->source()]) {
			deleteNullPotential_();

			int vertexWithMinPotential = findMinPotentialVertex_();
			if (vertexWithMinPotential == -1)
				break;

			long long pushingFlow = resultPotential_(vertexWithMinPotential);

			//push right
			layered_->setExcess(vertexWithMinPotential, pushingFlow);
			pushFromVertex_<EdgeList::FORWARD, LayeredNetwork::iterator>(vertexWithMinPotential, &LayeredNetwork::begin);


			//push left
			layered_->setExcess(vertexWithMinPotential, pushingFlow);
			std::swap(potentialIn_, potentialOut_);
			pushFromVertex_<EdgeList::BACK, LayeredNetwork::backIterator>(vertexWithMinPotential, &LayeredNetwork::rbegin);
			std::swap(potentialIn_, potentialOut_);
		}
	}


	long long resultPotential_(size_t index) {
		return std::min(potentialIn_[index], potentialOut_[index]);
	}

	void countPotential_() {
		for (size_t i = 0; i < layered_->vertexNumber(); ++i) {
			for (LayeredNetwork::iterator it = layered_->begin(i); it.valid(); ++it) {
				potentialOut_[(*it)->from()] += (*it)->getResidualCapacity();
				potentialIn_[(*it)->to()] += (*it)->getResidualCapacity();
			}
		}

		potentialIn_[layered_->source()] = potentialOut_[layered_->sink()] = Network::INF*Network::INF;
	}

	int findMinPotentialVertex_() {
		int index = -1;
		for (size_t i = 0; i < layered_->vertexNumber(); ++i) {
			if (resultPotential_(i) > 0 &&
				(index == -1 || resultPotential_(i) < resultPotential_(index))) {
				index = i;
			}
		}

		return index;
	}

	void deleteNullPotential_() {
		std::queue <size_t> deleteQueue;

		for (size_t i = 0; i < layered_->vertexNumber(); ++i) {
			if (resultPotential_(i) <= 0 && layered_->inLayer(i)) {
				deleteQueue.push(i);
			}
		}

		while (!deleteQueue.empty()) {
			changeAvailableVertex_(deleteQueue.front());
			deleteQueue.pop();
		}
	}

	void changeAvailableVertex_(size_t index) {
		if (notAvailable_[index]) return;

		notAvailable_[index]++;

		std::queue <size_t> deleteQueue;

		LayeredNetwork::iterator it = layered_->begin(index);
		deleteAllEdges_<LayeredNetwork::iterator&, EdgeList::FORWARD>(it, potentialIn_, deleteQueue);

		LayeredNetwork::backIterator itBack = layered_->rbegin(index);
		deleteAllEdges_<LayeredNetwork::backIterator&, EdgeList::BACK>(itBack, potentialOut_, deleteQueue);


		while (!deleteQueue.empty()) {
			changeAvailableVertex_(deleteQueue.front());
			deleteQueue.pop();
		}
	}

	template <class IteratorType, bool Direction>
	void deleteAllEdges_(IteratorType it, std::vector <long long> & potential, std::queue<size_t> & deleteQueue) {
		for (; it.valid(); layered_->eraseFromTraversal(it)) {
			size_t vertex = Direction ? ((*it)->from()) : ((*it)->to());

			potential[vertex] -= (*it)->getResidualCapacity();
			if (resultPotential_(vertex) <= 0) {
				deleteQueue.push(vertex);
			}
		}
	}


	template <bool Direction, class IteratorType>
	void pushFromVertex_(size_t vertex, IteratorType(LayeredNetwork::*initialize)(size_t)) {
		size_t finishVertex = Direction ? layered_->source() : layered_->sink();

		std::queue <size_t> queueBFS;
		queueBFS.push(vertex);

		while (!queueBFS.empty()) {
			size_t fromVertex = queueBFS.front();
			queueBFS.pop();
			if (fromVertex == finishVertex) return;

			size_t toVertex;
			for (IteratorType it = (layered_->*initialize)(fromVertex); it.valid() && layered_->excess()[fromVertex];
				(*it)->isSaturated() ? layered_->eraseFromTraversal(it) : ++it) {

				toVertex = Direction ? (*it)->from() : (*it)->to();

				long long possibleFlow = (*it)->getResidualCapacity();
				long long maxPushingFlow = std::min(layered_->excess()[fromVertex], possibleFlow);

				pushFlow_<IteratorType>(maxPushingFlow, fromVertex, toVertex, it);

				queueBFS.push(toVertex);
			}
		}
	}

	template <class IteratorType>
	void pushFlow_(long long maxPushingFlow, size_t vFrom, size_t vTo, IteratorType & it) {
		(*it).pushFlow(maxPushingFlow);

		layered_->setExcess(vFrom, layered_->excess()[vFrom] - maxPushingFlow);
		layered_->setExcess(vTo, layered_->excess()[vTo] + maxPushingFlow);

		potentialOut_[vFrom] -= maxPushingFlow;
		potentialIn_[vTo] -= maxPushingFlow;
	}
};

class PushRelabelAlgorithm : public MaxFlowProblemSolveAlgorithm {
public:
	PushRelabelAlgorithm() = default;
	explicit PushRelabelAlgorithm(Network * net) :
		MaxFlowProblemSolveAlgorithm(net)
	{
		reset_();
	}

	void run() {
		reset_();
		initializePreflow_();
		pushPreflow_();
	};

protected:
	long long getExcess_(size_t index) const {
		return excess_[index];
	}

	size_t getHeight_(size_t index) const {
		return height_[index];
	}

	Network::iterator getCurrentVertexIterator_(size_t index) const {
		return currentVertexIterator_[index];
	}

	void setCurrentVertexIterator_(size_t index, Network::iterator & it) {
		currentVertexIterator_[index] = it;
	}

	void push_(Network::iterator & it) {
		long long pushFlow = std::min(excess_[it->from()], it->getResidualCapacity());
		it.pushFlow(pushFlow);
		excess_[it->from()] -= pushFlow;
		excess_[it->to()] += pushFlow;
	}

	void relabel_(size_t vertex) {
		int vertexWithMinHeight = -1;
		for (Network::iterator it = network_->begin(vertex); it.valid(); ++it) {
			if (!it->isSaturated() && (vertexWithMinHeight == -1 || height_[it->to()] < height_[vertexWithMinHeight])) {
				vertexWithMinHeight = it->to();
			}
		}
		if (vertexWithMinHeight != -1)
			height_[vertex] = height_[vertexWithMinHeight] + 1;
	}

private:
	std::vector <long long> excess_;
	std::vector <size_t> height_;
	std::vector <Network::iterator> currentVertexIterator_;

	virtual void pushPreflow_() = 0;

	void reset_() {
		excess_.assign(network_->vertexNumber(), 0);
		height_.assign(network_->vertexNumber(), 0);
		currentVertexIterator_.resize(network_->vertexNumber());

		for (size_t i = 0; i < network_->vertexNumber(); ++i) {
			currentVertexIterator_[i] = network_->begin(i);
		}
	}

	void initializePreflow_() {
		for (Network::iterator it = network_->begin(network_->source()); it.valid(); ++it) {
			excess_[it->from()] -= it->getResidualCapacity();
			excess_[it->to()] += it->getResidualCapacity();
			it.pushFlow(it->getResidualCapacity());
		}
		height_[network_->source()] = network_->vertexNumber();
	}
};

class PushRelabelToFront : public PushRelabelAlgorithm {
public:
	PushRelabelToFront() = default;
	explicit PushRelabelToFront(Network * net) :
		PushRelabelAlgorithm(net)
	{}

private:
	void pushPreflow_() {
		std::list <size_t> vertexList;
		for (size_t i = 0; i < network_->vertexNumber(); ++i) {
			if (i != network_->source() && i != network_->sink()) {
				vertexList.push_back(i);
			}
		}

		size_t vertex;
		size_t oldHeight;
		for (auto it = vertexList.begin(); it != vertexList.end(); ++it) {
			vertex = *it;
			oldHeight = getHeight_(vertex);
			discharge_(vertex);
			if (getHeight_(vertex) > oldHeight) {
				vertexList.erase(it);
				vertexList.push_front(vertex);
				it = vertexList.begin();
			}
		}
	}

	void discharge_(size_t vertex) {
		while (getExcess_(vertex) > 0) {
			auto it = getCurrentVertexIterator_(vertex);
			if (!it.valid()) {
				relabel_(vertex);
				auto it = network_->begin(vertex);
				setCurrentVertexIterator_(vertex, it);
			}
			else {
				if (!it->isSaturated()
					&& getHeight_(it->from()) == getHeight_(it->to()) + 1) {
					push_(it);
				}
				else {
					auto it = getCurrentVertexIterator_(vertex);
					setCurrentVertexIterator_(vertex, ++it);
				}
			}
		}
	}
};



struct Data {
	size_t themesNumber;
	std::vector <long long> themesCost;
	std::vector <std::vector <int>> dependencies;

	Data() = default;
	explicit Data(size_t themes) :
		themesNumber(themes)
	{
		themesCost.resize(themes, 0);
		dependencies.resize(themes);
	}
};

template <typename T>
std::vector<T> readVector(std::istream& in, size_t size) {
	std::vector<T> vector(size);
	for (size_t i = 0; i < vector.size(); ++i) {
		in >> vector[i];
	}
	return vector;
}

Data readData(std::istream& in) {
	size_t n;
	in >> n;
	Data data(n);

	data.themesCost = readVector<long long>(in, data.themesNumber);

	size_t dependenciesNumber;
	for (size_t i = 0; i < data.themesNumber; ++i) {
		in >> dependenciesNumber;
		data.dependencies[i] = readVector<int>(in, dependenciesNumber);
	}

	return data;
}

Network buildNetwork(const Data & data) {
	Network network(0, data.themesNumber + 1, data.themesNumber + 2);

	for (size_t i = 0; i < data.themesNumber; ++i) {
		if (data.themesCost[i] > 0) {
			network.addEdge(network.source(), i + 1, data.themesCost[i], true);
		}
		else {
			network.addEdge(i + 1, network.sink(), -data.themesCost[i], true);
		}
	}

	for (size_t i = 0; i < data.themesNumber; ++i) {
		for (size_t j = 0; j < data.dependencies[i].size(); ++j) {
			network.addEdge(i + 1, data.dependencies[i][j], Network::INF, true);
		}
	}

	return network;
}

template <class Algorithm>
long long solve(const Data & data) {
	long long maxCost = 0;
	for (size_t i = 0; i < data.themesNumber; ++i) {
		if (data.themesCost[i] > 0) maxCost += data.themesCost[i];
	}

	Network network = buildNetwork(data);

	Algorithm maxFlowAlgorithm(&network);
	maxFlowAlgorithm.run();

	long long minPenalty = maxFlowAlgorithm.getMaxFlow();
	return maxCost - minPenalty;
}

template <class Algorithm>
void run(std::istream& in, std::ostream& out) {
	Data data = readData(in);
	out << solve<Algorithm>(data) << std::endl;
}

int main() {
	run<MalhotraKumarMaheshwariBlockingFlow>(std::cin, std::cout);
	//run<PushRelabelToFront>(std::cin, std::cout);
	return 0;
}
