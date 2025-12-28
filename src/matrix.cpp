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
	my_vector v1(_mat[0][0], _mat[0][1], _mat[0][2]);
	my_vector v2(_mat[1][0], _mat[1][1], _mat[1][2]);
	my_vector v3(_mat[2][0], _mat[2][1], _mat[2][2]);

	return my_vector((v * v1 + _mat[0][3]), (v * v2 + _mat[1][3]), (v * v3 + _mat[2][3]));
}

void matrix::prep_gen_mat(const attrib& a)
{
	unit dx = a.get_deg_x();
	unit dy = a.get_deg_y();
	unit dz = a.get_deg_z();
	unit ox = a.get_off_x();
	unit oy = a.get_off_y();
	unit oz = a.get_off_z();
	unit zm = a.get_zoom();
	unit sdx = sin(dx);
	unit sdy = sin(dy);
	unit sdz = sin(dz);
	unit cdx = cos(dx);
	unit cdy = cos(dy);
	unit cdz = cos(dz);

	_mat[0][0] = (zm * cdy * cdz);
	_mat[0][1] = (zm * -cdy * sdz);
	_mat[0][2] = (zm * -sdy);
	_mat[0][3] = ox;
	_mat[1][0] = (zm * (cdx * sdz - sdx * sdy * cdz));
	_mat[1][1] = (zm * (cdx * cdz + sdx * sdy * sdz));
	_mat[1][2] = (zm * -sdx * cdy);
	_mat[1][3] = oy;
	_mat[2][0] = (zm * (sdx * sdz + cdx * sdy * cdz));
	_mat[2][1] = (zm * (sdx * cdz - cdx * sdy * sdz));
	_mat[2][2] = (zm * cdx * cdy);
	_mat[2][3] = oz;
	_mat[3][0] = ZERO;
	_mat[3][1] = ZERO;
	_mat[3][2] = ZERO;
	_mat[3][3] = UNIT;
}

void matrix::prep_rot_mat(const attrib& a)
{
	unit dx = a.get_deg_x();
	unit dy = a.get_deg_y();
	unit dz = a.get_deg_z();
	unit ox = a.get_off_x();
	unit oy = a.get_off_y();
	unit oz = a.get_off_z();
	unit zm = a.get_zoom();
	unit sdx = sin(dx);
	unit sdy = sin(dy);
	unit sdz = sin(dz);
	unit cdx = cos(dx);
	unit cdy = cos(dy);
	unit cdz = cos(dz);

	_mat[0][0] = (cdy * cdy);
	_mat[0][1] = (-cdy * sdz);
	_mat[0][2] = (-sdy);
	_mat[0][3] = ZERO;
	_mat[1][0] = (cdx * sdz - sdx * sdy * cdy);
	_mat[1][1] = (cdx * cdy + sdx * sdy * sdz);
	_mat[1][2] = (-sdx * cdy);
	_mat[1][3] = ZERO;
	_mat[2][0] = (sdx * sdz + cdx * sdy * cdy);
	_mat[2][1] = (sdx * cdy - cdx * sdy * sdz);
	_mat[2][2] = (cdx * cdy);
	_mat[2][3] = ZERO;
	_mat[3][0] = ZERO;
	_mat[3][1] = ZERO;
	_mat[3][2] = ZERO;
	_mat[3][3] = UNIT;
}

matrix get_unit_mat()
{
	return matrix(unt);
}

} // namespace matrix_ns
