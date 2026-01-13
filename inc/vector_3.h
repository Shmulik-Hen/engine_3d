#pragma once

#include "common.h"
#include "unit.h"

namespace vector_3_ns
{

using std::ifstream;
using std::istream;
using std::ostream;
using unit_ns::unit;
using unit_ns::UNIT;
using unit_ns::ZERO;

const int dim = Z_ + 1;

class vector_3
{
	unit _coords[dim];

public:

	vector_3();
	vector_3(const vector_3&);
	vector_3(const unit[dim]);
	vector_3(const unit, const unit, const unit);
	~vector_3() {}

	bool read(ifstream&);
	void print() const;
	unit get(int) const;
	vector_3 project(const vector_3&, const vector_3&);
	void normalize(vector_3&);

	vector_3& operator=(const vector_3&) = default;
	vector_3 operator+(const vector_3&) const;
	vector_3 operator-(const vector_3&) const;
	vector_3& operator+=(const vector_3&);
	vector_3& operator-=(const vector_3&);
	const unit operator*(const vector_3&) const;

	friend ostream& operator<<(ostream&, const vector_3&);
	friend istream& operator>>(istream&, vector_3&);
};

} // namespace vector_3_ns
