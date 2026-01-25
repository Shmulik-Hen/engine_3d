#pragma once

#include "common.h"
#include "attrib.h"
#include "vector_3.h"

namespace matrix_ns
{

using namespace attrib_ns;
using namespace vector_3_ns;
// using unit_ns::unit;
// using unit_ns::UNIT;
// using unit_ns::ZERO;

const int dim = O_ + 1;

class matrix
{
	class vector_4
	{
	public:

		unit _coords[dim];

		vector_4();
		vector_4(const vector_4&);
		vector_4(const unit[dim]);
		vector_4(const unit, const unit, const unit, const unit);
		~vector_4() {}

		vector_4 operator+(const vector_4&) const;
		vector_4 operator-(const vector_4&) const;
		vector_4& operator=(const vector_4&);
		vector_4& operator+=(const vector_4&);
		vector_4& operator-=(const vector_4&);
		unit operator*(const vector_4&) const;
	};

	unit _mat[dim][dim];
	vector_4 _rows[dim];
	vector_4 _cols[dim];

	void prep_row(int);
	void prep_cols();

public:

	matrix();
	matrix(const matrix&);
	matrix(unit[dim][dim]);
	~matrix() {}

	void print() const;

	matrix operator*(const matrix&) const;
	matrix& operator=(const matrix& m);
	matrix& operator*=(const matrix&);
	vector_3 operator*(const vector_3&) const;

	void prep_gen_mat(const attrib&);
	void prep_rot_mat(const attrib&);
};

matrix get_unit_mat();

} // namespace matrix_ns
