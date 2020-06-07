#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace Geometry {
	class Point {
	private:
		long long x_, y_;

	public:
		Point() = default;
		explicit Point (long long x, long long y) :
			x_(x),
			y_(y)
		{}

		long long getX () const {
			return x_;
		}

		long long getY () const {
			return y_;
		}


		long long operator * (const Point & p) const {
			return this->x_ * p.x_ + this->y_ * p.y_;
		}

		long long operator ^ (const Point & p) const {
			return this->x_ * p.y_ - this->y_ * p.x_;
		}

		bool operator == (const Point & p) const {
			return (this->x_ == p.x_ && this->y_ == p.y_);
		}

		bool operator != (const Point & p) const {
			return !(*this == p);
		}

		static bool lexicographicalyCompare (const Point & p1, const Point & p2) {
			return ((p1.x_ != p2.x_) ? (p1.x_ < p2.x_) : (p1.y_ < p2.y_));
		}

		Point& operator += (const Point & p) {
			this->x_ += p.x_;
			this->y_ += p.y_;
			return *this;
		}
		Point& operator -= (const Point & p) {
			this->x_ -= p.x_;
			this->y_ -= p.y_;
			return *this;
		}

		Point operator + (const Point & p) const {
			Point res = *this;
			res += p;
			return res;
		}
		Point operator - (const Point & p) const {
			Point res = *this;
			res -= p;
			return res;
		}
	};

	std::istream& operator >> (std::istream& is, Point & p) {
		long long x, y;
		is >> x >> y;
		p = Point(x, y);

		return is;
	}


	class OrientedSegment {
	public:
		OrientedSegment() = default;
		explicit OrientedSegment (const Point & A, const Point & B) :
			A_(A),
			B_(B)
		{}
		explicit OrientedSegment (const Point & A) :
			A_(A),
			B_(A)
		{}

		Point getA () const {
			return A_;
		}

		Point getB () const {
			return B_;
		}

		bool isOn (const Point & p) const {
			return (((this->A_ - p)^(this->B_ - this->A_)) == 0) && (((this->B_ - p)^(this->B_ - this->A_)) == 0)
				&& ((((this->A_ - p)*(this->B_ - p)) < 0) || this->B_ == p || this->A_ == p);
		}


		bool isPointInXProjection(const Point & p) const {
			Point Q = this->A_, S = this->B_;
			if (Geometry::Point::lexicographicalyCompare(S, Q)) {
				std::swap(S, Q);
			}
			return (Q.getX() <= p.getX() && p.getX() <= S.getX());
		}


		bool isPositivelyOriented () const {
			return (( (this->B_ - this->A_) * (Point(1, 0)) ) > 0);
		}

	private:
		Point A_, B_;
	};

	class Line {
	public:
		enum PointPositionWithRespectOnLine { ABOVE, BELOW, ON };

		Line () = default;
		explicit Line (long long A, long long B, long long C) :
			A_(A),
			B_(B),
			C_(C)
		{}

		explicit Line (const Point & p1, const Point & p2) {
			A_ = p1.getY() -  p2.getY();
			B_ = p2.getX() - p1.getX();
			C_ = -A_*p1.getX() - B_*p1.getY();
		}

		long long getA () const {
			return A_;
		}
		long long getB () const {
			return B_;
		}
		long long getC () const {
			return C_;
		}

		PointPositionWithRespectOnLine pointPosition (const Point & p) const {
			long long result = A_*p.getX() + B_*p.getY() + C_;
			if (result > 0) {
				return ABOVE;
			}
			return (result < 0 ? BELOW : ON);
		}

	private:
		long long A_, B_, C_;
	};


	class Polygon {
	public:
		Polygon () = default;
		explicit Polygon (const std::vector <Point> & points) {
			polygonPoints_ = points;
			if (polygonPoints_[0] != polygonPoints_.back()) {
				polygonPoints_.push_back(polygonPoints_[0]);
			}
		}

		size_t getPointNumbers () const {
			return polygonPoints_.size();
		}

		std::vector <Point> getPoints () const {
			return polygonPoints_;
		}

		bool isCounterClockwise () const {
			return (calculateOrientedArea() > 0);
		}

		long double calculateOrientedArea () const {
			long double square = 0;
			for (size_t i = 1; i < polygonPoints_.size(); ++i) {
				square += ((polygonPoints_[i-1] - polygonPoints_[0]) ^ (polygonPoints_[i] - polygonPoints_[0]));
			}

			return (square / 2);
		}

		void switchOrientation () {
			std::reverse(polygonPoints_.begin(), polygonPoints_.end());
		}

		std::vector<OrientedSegment> generateSegments () {
			std::vector<OrientedSegment> polygonSegments;
			for (size_t i = 0; i + 1 < polygonPoints_.size(); ++i) {
				if (polygonPoints_[i] == polygonPoints_[i+1]) {
					continue;
				}
				polygonSegments.push_back(OrientedSegment(polygonPoints_[i], polygonPoints_[i+1]));
			}
			return polygonSegments;
		}

	private:
		std::vector <Point> polygonPoints_;

	};
};



class FindInsidePointsAlgorithm {
public:

	enum PointPosition { NOANSWER, OUTSIDE, INSIDE, BORDER };

	FindInsidePointsAlgorithm () = default;

	void build (const Geometry::Polygon & polygon, const std::vector <Geometry::Point> & problems) {
		reset_();

		problemPoints_ = problems;
		polygon_ = polygon;
		generateAntiClockwisePolygon_();
		generateSegmentsPolygon_();

		generateEvents_();
		sortEvents_();
		answers_.assign(problemPoints_.size(), NOANSWER);
	}

	void run () {
		for (auto event : events_) {
			EventType_ eventType = event.getType();
			switch (eventType) {
				case FIRST_CHECK:
				case SECOND_CHECK: {
					auto it = sweepLine_.lower_bound(Geometry::OrientedSegment(event.getPoint()));
					if (it != sweepLine_.end())
						setPointPositionOn(*it, event);
					else if (answers_[event.getId()] == NOANSWER)
						answers_[event.getId()] = OUTSIDE;
					break;
                }
				case CLOSE:
					sweepLine_.erase(segmentsPolygon_[event.getId()]);
					break;
				case OPEN:
					sweepLine_.insert(segmentsPolygon_[event.getId()]);
					break;
				case VERTICAL_CLOSE:
					verticalSweepLine_ = false;
					break;
				case VERTICAL_OPEN:
					verticalSweepLine_ = true;
					break;
				case VERTICAL_CHECK:
					if (verticalSweepLine_)
						answers_[event.getId()] = BORDER;
					break;
			}
		}
	}

	std::vector <PointPosition> getAnswer () const {
		return answers_;
	}

	static std::string positionToString (PointPosition position) {
		switch (position) {
			case OUTSIDE:
				return "OUTSIDE";
			case INSIDE:
				return "INSIDE";
			case BORDER:
				return "BORDER";
		}
	}

private:
	enum EventType_ { FIRST_CHECK, CLOSE, OPEN, SECOND_CHECK, VERTICAL_CLOSE, VERTICAL_OPEN, VERTICAL_CHECK };

	class Event_ {
	public:
		Event_ () = default;
		explicit Event_ (const Geometry::Point & p, EventType_ type, size_t id) :
			point_(p),
			type_(type),
			id_(id)
		{}

		Geometry::Point getPoint () const {
			return point_;
		}

		EventType_ getType () const {
			return type_;
		}

		size_t getId () const {
			return id_;
		}

		bool operator < (const Event_ & e) const {
			if (this->type_ >= VERTICAL_CLOSE && e.type_ >= VERTICAL_CLOSE) {
				return (Geometry::Point::lexicographicalyCompare(this->point_, e.point_) || (this->point_ == e.point_ && this->type_ < e.type_));
			}
			return ((this->point_.getX() < e.point_.getX())|| (this->point_.getX() == e.point_.getX() && this->type_ < e.type_));
		}

	private:
		Geometry::Point point_;
		EventType_ type_;
		size_t id_;
	};



	struct isSegmentAboveCompare {
		void unpack(Geometry::Point & p1, Geometry::Point & p2, const Geometry::OrientedSegment & s) {
			p1 = s.getA();
			p2 = s.getB();
			if (Geometry::Point::lexicographicalyCompare(p2, p1)) {
				std::swap(p2, p1);
			}
		}

		bool operator () (const Geometry::OrientedSegment & s1, const Geometry::OrientedSegment & s2) {
			Geometry::Point P1, Q1, P2, Q2;
			unpack(P1, Q1, s1);
			unpack(P2, Q2, s2);

			if (P1 == P2 || Q1 == Q2) {
				Geometry::Line line(P2, Q2);
				return ((P1 == P2) ? (line.pointPosition(Q1) == Geometry::Line::ABOVE) : (line.pointPosition(P1) == Geometry::Line::ABOVE));
			}
			else if (P2 == Q2 || (s1.isPointInXProjection(P2) || s1.isPointInXProjection(Q2))) {
				Geometry::Line line(P1, Q1);
				return ((s1.isPointInXProjection(Q2) && line.pointPosition(Q2) == Geometry::Line::BELOW)
					|| (s1.isPointInXProjection(P2) && line.pointPosition(P2) == Geometry::Line::BELOW));
			}
			else {
				Geometry::Line line(P2, Q2);
				return ((s2.isPointInXProjection(Q1) && line.pointPosition(Q1) == Geometry::Line::ABOVE)
					|| (s2.isPointInXProjection(P1) && line.pointPosition(P1) == Geometry::Line::ABOVE));
			}
		}
	};


	Geometry::Polygon polygon_;
	std::vector <Geometry::OrientedSegment> segmentsPolygon_;
	std::vector <Geometry::Point> problemPoints_;
	std::vector <Event_> events_;
	std::set <Geometry::OrientedSegment, FindInsidePointsAlgorithm::isSegmentAboveCompare> sweepLine_;
	std::vector <PointPosition> answers_;
	bool verticalSweepLine_;

	void reset_ () {
		problemPoints_.clear();
		events_.clear();
		sweepLine_.clear();
		answers_.clear();
		verticalSweepLine_ = false;
	}

	void generateAntiClockwisePolygon_ () {
		if (!polygon_.isCounterClockwise()) {
			polygon_.switchOrientation();
		}
	}


	void generateSegmentsPolygon_() {
		segmentsPolygon_ = polygon_.generateSegments();
	}

	void generateEvents_ () {
		Geometry::Point P, Q;
		for (size_t i = 0; i < segmentsPolygon_.size(); ++i) {
			P = segmentsPolygon_[i].getA();
			Q = segmentsPolygon_[i].getB();
			if (P.getX() != Q.getX()) {
				if (P.getX() >= Q.getX()) {
					std::swap(P, Q);
				}
				events_.push_back(Event_(P, OPEN, i));
				events_.push_back(Event_(Q, CLOSE, i));
			}
			else {
				if (P.getY() >= Q.getY()) {
					std::swap(P, Q);
				}
				events_.push_back(Event_(P, VERTICAL_OPEN, i));
				events_.push_back(Event_(Q, VERTICAL_CLOSE, i));
			}
		}

		for (size_t i = 0; i < problemPoints_.size(); ++i) {
			events_.push_back(Event_(problemPoints_[i], FIRST_CHECK, i));
			events_.push_back(Event_(problemPoints_[i], SECOND_CHECK, i));
			events_.push_back(Event_(problemPoints_[i], VERTICAL_CHECK, i));
		}

	}

	void sortEvents_ () {
		std::sort(events_.begin(), events_.end());
	}

	void setPointPositionOn (const Geometry::OrientedSegment & s, const Event_ & event) {
		if (answers_[event.getId()] == BORDER) {
			return;
		}

		if (s.isOn(event.getPoint())) {
			answers_[event.getId()] = BORDER;
		}
		else if (s.isPositivelyOriented()) {
			answers_[event.getId()] = INSIDE;
		}
		else if (answers_[event.getId()] == NOANSWER) {
			answers_[event.getId()] = OUTSIDE;
		}
	}
};

std::vector<Geometry::Point> readPointsVector (std::istream& in, size_t size) {
	std::vector<Geometry::Point> vector(size);
	for (size_t i = 0; i < vector.size(); ++i) {
		in >> vector[i];
	}
	return vector;
}

struct Test {
	size_t polygonSize, pointsNumber;
	Geometry::Polygon polygon;
	std::vector <Geometry::Point> points;
	std::vector <FindInsidePointsAlgorithm::PointPosition> result;

	Test () = default;

	void solve () {
		FindInsidePointsAlgorithm algorithm;
		algorithm.build(polygon, points);
		algorithm.run();
		result = algorithm.getAnswer();
	}

	void printAnswer (std::ostream & out) const {
		for (size_t i = 0; i < pointsNumber; ++i) {
			out << FindInsidePointsAlgorithm::positionToString(result[i]) << std::endl;
		}
	}
};

std::istream& operator >> (std::istream& is, Test & t) {
	is >> t.polygonSize;
	std::vector <Geometry::Point> points = readPointsVector(is, t.polygonSize);
	t.polygon = Geometry::Polygon(points);

	is >> t.pointsNumber;
	t.points = readPointsVector(is, t.pointsNumber);

	return is;
}

struct Data {
	size_t size;
	std::vector <Test> tests;

	Data () = default;
	explicit Data (size_t s) :
		size(s)
	{
		tests.resize(size);
	}

	void readAllTests (std::istream& in) {
		for (size_t i = 0; i < size; ++i) {
			in >> tests[i];
		}
	}

	void solveAll () {
		for (size_t i = 0; i < size; ++i) {
			tests[i].solve();
		}
	}

	void printAllAnswers (std::ostream& out) const {
		for (size_t i = 0; i < size; ++i) {
			tests[i].printAnswer(out);
			out << std::endl;
		}
	}
};

void run(std::istream& in, std::ostream& out) {
	size_t testNumber;
	in >> testNumber;

	Data allData(testNumber);
	allData.readAllTests(in);
	allData.solveAll();
	allData.printAllAnswers(out);
}

int main() {
	run(std::cin, std::cout);
	return 0;
}
