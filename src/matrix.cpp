#include <string.h>

#include "matrix.h"

namespace matrix_ns
{

// clang-format off
unit unt[dim][dim] = {
	{ UNIT, ZERO, ZERO, ZERO },
	{ ZERO, UNIT, ZERO, ZERO },
	{ ZERO, ZERO, UNIT, ZERO },
	{ ZERO, ZERO, ZERO, UNIT }
};
// clang-format on

matrix::matrix()
{
	explicit_bzero(_mat, sizeof(unt));
}

matrix::matrix(unit n[dim][dim])
{
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			_mat[i][j] = n[i][j];
		}
	}
}

matrix matrix::operator*(const matrix& m)
{
	matrix temp;

	// clang-format off
	temp._mat[0][0] = _mat[0][0] * m._mat[0][0] + _mat[0][1] * m._mat[1][0] +
			  _mat[0][2] * m._mat[2][0] + _mat[0][3] * m._mat[3][0];
	temp._mat[0][1] = _mat[0][0] * m._mat[0][1] + _mat[0][1] * m._mat[1][1] +
			  _mat[0][2] * m._mat[2][1] + _mat[0][3] * m._mat[3][1];
	temp._mat[0][2] = _mat[0][0] * m._mat[0][2] + _mat[0][1] * m._mat[1][2] +
			  _mat[0][2] * m._mat[2][2] + _mat[0][3] * m._mat[3][2];
	temp._mat[0][3] = _mat[0][0] * m._mat[0][3] + _mat[0][1] * m._mat[1][3] +
			  _mat[0][2] * m._mat[2][3] + _mat[0][3] * m._mat[3][3];
	temp._mat[1][0] = _mat[1][0] * m._mat[0][0] + _mat[1][1] * m._mat[1][0] +
			  _mat[1][2] * m._mat[2][0] + _mat[1][3] * m._mat[3][0];
	temp._mat[1][1] = _mat[1][0] * m._mat[0][1] + _mat[1][1] * m._mat[1][1] +
			  _mat[1][2] * m._mat[2][1] + _mat[1][3] * m._mat[3][1];
	temp._mat[1][2] = _mat[1][0] * m._mat[0][2] + _mat[1][1] * m._mat[1][2] +
			  _mat[1][2] * m._mat[2][2] + _mat[1][3] * m._mat[3][2];
	temp._mat[1][3] = _mat[1][0] * m._mat[0][3] + _mat[1][1] * m._mat[1][3] +
			  _mat[1][2] * m._mat[2][3] + _mat[1][3] * m._mat[3][3];
	temp._mat[2][0] = _mat[2][0] * m._mat[0][0] + _mat[2][1] * m._mat[1][0] +
			  _mat[2][2] * m._mat[2][0] + _mat[2][3] * m._mat[3][0];
	temp._mat[2][1] = _mat[2][0] * m._mat[0][1] + _mat[2][1] * m._mat[1][1] +
			  _mat[2][2] * m._mat[2][1] + _mat[2][3] * m._mat[3][1];
	temp._mat[2][2] = _mat[2][0] * m._mat[0][2] + _mat[2][1] * m._mat[1][2] +
			  _mat[2][2] * m._mat[2][2] + _mat[2][3] * m._mat[3][2];
	temp._mat[2][3] = _mat[2][0] * m._mat[0][3] + _mat[2][1] * m._mat[1][3] +
			  _mat[2][2] * m._mat[2][3] + _mat[2][3] * m._mat[3][3];
	temp._mat[3][0] = _mat[3][0] * m._mat[0][0] + _mat[3][1] * m._mat[1][0] +
			  _mat[3][2] * m._mat[2][0] + _mat[3][3] * m._mat[3][0];
	temp._mat[3][1] = _mat[3][0] * m._mat[0][1] + _mat[3][1] * m._mat[1][1] +
			  _mat[3][2] * m._mat[2][1] + _mat[3][3] * m._mat[3][1];
	temp._mat[3][2] = _mat[3][0] * m._mat[0][2] + _mat[3][1] * m._mat[1][2] +
			  _mat[3][2] * m._mat[2][2] + _mat[3][3] * m._mat[3][2];
	temp._mat[3][3] = _mat[3][0] * m._mat[0][3] + _mat[3][1] * m._mat[1][3] +
			  _mat[3][2] * m._mat[2][3] + _mat[3][3] * m._mat[3][3];
	// clang-format on

	return temp;
}

matrix& matrix::operator*=(const matrix& m)
{
	*this = *this * m;
	return *this;
}

my_vector matrix::operator*(const my_vector& v)
{
	my_vector temp;

	temp.get_coord(coord::X) = _mat[0][0] * v.get_coord(coord::X) +
	                           _mat[0][1] * v.get_coord(coord::Y) +
	                           _mat[0][2] * v.get_coord(coord::Z) +
	                           _mat[0][3];
	temp.get_coord(coord::Y) = _mat[1][0] * v.get_coord(coord::X) +
	                           _mat[1][1] * v.get_coord(coord::Y) +
	                           _mat[1][2] * v.get_coord(coord::Z) +
	                           _mat[1][3];
	temp.get_coord(coord::Z) = _mat[2][0] * v.get_coord(coord::X) +
	                           _mat[2][1] * v.get_coord(coord::Y) +
	                           _mat[2][2] * v.get_coord(coord::Z) +
	                           _mat[2][3];

	return temp;
}

void matrix::prep_gen_mat(const attrib& a)
{
	// clang-format off
	_mat[0][0] = a.get_zoom() * cos(a.get_deg_y()) * cos(a.get_deg_z());
	_mat[0][1] = a.get_zoom() * -cos(a.get_deg_y()) * sin(a.get_deg_z());
	_mat[0][2] = a.get_zoom() * -sin(a.get_deg_y());
	_mat[0][3] = a.get_off_x();
	_mat[1][0] = a.get_zoom() * (cos(a.get_deg_x()) * sin(a.get_deg_z()) -
		     sin(a.get_deg_x()) * sin(a.get_deg_y()) * cos(a.get_deg_z()));
	_mat[1][1] = a.get_zoom() * (cos(a.get_deg_x()) * cos(a.get_deg_z()) +
		     sin(a.get_deg_x()) * sin(a.get_deg_y()) * sin(a.get_deg_z()));
	_mat[1][2] = a.get_zoom() * -sin(a.get_deg_x()) * cos(a.get_deg_y());
	_mat[1][3] = a.get_off_y();
	_mat[2][0] = a.get_zoom() * (sin(a.get_deg_x()) * sin(a.get_deg_z()) +
		     cos(a.get_deg_x()) * sin(a.get_deg_y()) * cos(a.get_deg_z()));
	_mat[2][1] = a.get_zoom() * (sin(a.get_deg_x()) * cos(a.get_deg_z()) -
		     cos(a.get_deg_x()) * sin(a.get_deg_y()) * sin(a.get_deg_z()));
	_mat[2][2] = a.get_zoom() * cos(a.get_deg_x()) * cos(a.get_deg_y());
	_mat[2][3] = a.get_off_z();
	_mat[3][0] = 0;
	_mat[3][1] = 0;
	_mat[3][2] = 0;
	_mat[3][3] = UNIT;
	// clang-format on
}

void matrix::prep_rot_mat(const attrib& a)
{
	_mat[0][0] = cos(a.get_deg_y()) * cos(a.get_deg_z());
	_mat[0][1] = -cos(a.get_deg_y()) * sin(a.get_deg_z());
	_mat[0][2] = -sin(a.get_deg_y());
	_mat[0][3] = 0;
	_mat[1][0] = cos(a.get_deg_x()) * sin(a.get_deg_z()) -
	             sin(a.get_deg_x()) * sin(a.get_deg_y()) * cos(a.get_deg_z());
	_mat[1][1] = cos(a.get_deg_x()) * cos(a.get_deg_z()) +
	             sin(a.get_deg_x()) * sin(a.get_deg_y()) * sin(a.get_deg_z());
	_mat[1][2] = -sin(a.get_deg_x()) * cos(a.get_deg_y());
	_mat[1][3] = 0;
	_mat[2][0] = sin(a.get_deg_x()) * sin(a.get_deg_z()) +
	             cos(a.get_deg_x()) * sin(a.get_deg_y()) * cos(a.get_deg_z());
	_mat[2][1] = sin(a.get_deg_x()) * cos(a.get_deg_z()) -
	             cos(a.get_deg_x()) * sin(a.get_deg_y()) * sin(a.get_deg_z());
	_mat[2][2] = cos(a.get_deg_x()) * cos(a.get_deg_y());
	_mat[2][3] = 0;
	_mat[3][0] = 0;
	_mat[3][1] = 0;
	_mat[3][2] = 0;
	_mat[3][3] = UNIT;
}

matrix get_unit_mat()
{
	return matrix(unt);
}

} // namespace matrix_ns
