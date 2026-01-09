// #define DEBUG_PRINTS
#include "matrix.h"

namespace matrix_ns
{

matrix::vector_4::vector_4()
{
	for (int i = X_; i <= O_; i++) {
		_coords[i] = ZERO;
	}
}

matrix::vector_4::vector_4(const vector_4& v)
{
	for (int i = X_; i <= O_; i++) {
		_coords[i] = v._coords[i];
	}
}

matrix::vector_4::vector_4(const unit v[dim])
{
	for (int i = X_; i <= O_; i++) {
		_coords[i] = v[i];
	}
}

matrix::vector_4::vector_4(const unit x, const unit y, const unit z, const unit o)
{
	_coords[X_] = x;
	_coords[Y_] = y;
	_coords[Z_] = z;
	_coords[O_] = o;
}

matrix::vector_4& matrix::vector_4::operator=(const vector_4& v)
{
	if (this != &v) {
		for (int i = X_; i <= O_; i++) {
			_coords[i] = v._coords[i];
		}
	}
	return *this;
}

matrix::vector_4 matrix::vector_4::operator+(const vector_4& v) const
{
	return vector_4(_coords[X_] + v._coords[X_],
	                _coords[Y_] + v._coords[Y_],
	                _coords[Z_] + v._coords[Z_],
	                _coords[O_] + v._coords[O_]);
}

matrix::vector_4 matrix::vector_4::operator-(const vector_4& v) const
{
	return vector_4(_coords[X_] - v._coords[X_],
	                _coords[Y_] - v._coords[Y_],
	                _coords[Z_] - v._coords[Z_],
	                _coords[O_] - v._coords[O_]);
}

matrix::vector_4& matrix::vector_4::operator+=(const vector_4& v)
{
	_coords[X_] += v._coords[X_];
	_coords[Y_] += v._coords[Y_];
	_coords[Z_] += v._coords[Z_];
	_coords[O_] += v._coords[O_];

	return *this;
}

matrix::vector_4& matrix::vector_4::operator-=(const vector_4& v)
{
	_coords[X_] -= v._coords[X_];
	_coords[Y_] -= v._coords[Y_];
	_coords[Z_] -= v._coords[Z_];
	_coords[O_] -= v._coords[O_];

	return *this;
}

const unit matrix::vector_4::operator*(const vector_4& v) const
{
	return unit(_coords[X_] * v._coords[X_] +
	            _coords[Y_] * v._coords[Y_] +
	            _coords[Z_] * v._coords[Z_] +
	            _coords[O_] * v._coords[O_]);
}

// clang-format off
unit unt[dim][dim] = {
	{ UNIT, ZERO, ZERO, ZERO },
	{ ZERO, UNIT, ZERO, ZERO },
	{ ZERO, ZERO, UNIT, ZERO },
	{ ZERO, ZERO, ZERO, UNIT }
};
// clang-format on

void matrix::prep_row(int r)
{
	_rows[r] = _mat[r];
}

void matrix::prep_cols()
{
	for (int i = X_; i <= O_; i++) {
		_cols[i] = {_rows[X_]._coords[i],
		            _rows[Y_]._coords[i],
		            _rows[Z_]._coords[i],
		            _rows[O_]._coords[i]};
	}
}

matrix::matrix()
{
	for (int i = X_; i <= O_; i++) {
		for (int j = X_; j <= O_; j++) {
			_mat[i][j] = ZERO;
		}
		prep_row(i);
	}

	prep_cols();
}

matrix::matrix(const matrix& m) :
	attrib(0, 0, 0, 0, 0, 0, 1),
	vector_3(0, 0, 0)
{
	for (int i = X_; i <= O_; i++) {
		for (int j = 0; j <= O_; j++) {
			_mat[i][j] = m._mat[i][j];
		}
		prep_row(i);
	}

	prep_cols();
}

matrix::matrix(unit u[dim][dim])
{
	for (int i = X_; i <= O_; i++) {
		for (int j = X_; j <= O_; j++) {
			_mat[i][j] = u[i][j];
		}
		prep_row(i);
	}

	prep_cols();
}

matrix matrix::operator*(const matrix& m)
{
	matrix temp;

	temp._mat[X_][X_] = _rows[X_] * m._cols[X_];
	temp._mat[X_][Y_] = _rows[X_] * m._cols[Y_];
	temp._mat[X_][Z_] = _rows[X_] * m._cols[Z_];
	temp._mat[X_][O_] = _rows[X_] * m._cols[O_];
	temp._mat[Y_][X_] = _rows[Y_] * m._cols[X_];
	temp._mat[Y_][Y_] = _rows[Y_] * m._cols[Y_];
	temp._mat[Y_][Z_] = _rows[Y_] * m._cols[Z_];
	temp._mat[Y_][O_] = _rows[Y_] * m._cols[O_];
	temp._mat[Z_][X_] = _rows[Z_] * m._cols[X_];
	temp._mat[Z_][Y_] = _rows[Z_] * m._cols[Y_];
	temp._mat[Z_][Z_] = _rows[Z_] * m._cols[Z_];
	temp._mat[Z_][O_] = _rows[Z_] * m._cols[O_];
	temp._mat[O_][X_] = _rows[O_] * m._cols[X_];
	temp._mat[O_][Y_] = _rows[O_] * m._cols[Y_];
	temp._mat[O_][Z_] = _rows[O_] * m._cols[Z_];
	temp._mat[O_][O_] = _rows[O_] * m._cols[O_];

	for (int i = X_; i <= O_; i++) {
		temp.prep_row(i);
	}
	temp.prep_cols();

	return temp;
}

matrix& matrix::operator=(const matrix& m)
{
	if (this != &m) {
		for (int i = X_; i <= O_; i++) {
			for (int j = X_; j <= O_; j++) {
				_mat[i][j] = m._mat[i][j];
			}
			prep_row(i);
		}
		prep_cols();
	}
	return *this;
}

matrix& matrix::operator*=(const matrix& m)
{
	*this = *this * m;
	return *this;
}

vector_3 matrix::operator*(const vector_3& v)
{
	vector_3 v1(_mat[X_][X_], _mat[X_][Y_], _mat[X_][Z_]);
	vector_3 v2(_mat[Y_][X_], _mat[Y_][Y_], _mat[Y_][Z_]);
	vector_3 v3(_mat[Z_][X_], _mat[Z_][Y_], _mat[Z_][Z_]);

	return vector_3((v * v1 + _mat[X_][O_]),
	                (v * v2 + _mat[Y_][O_]),
	                (v * v3 + _mat[Z_][O_]));
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

	_mat[X_][X_] = (zm * cdy * cdz);
	_mat[X_][Y_] = (zm * -cdy * sdz);
	_mat[X_][Z_] = (zm * -sdy);
	_mat[X_][O_] = ox;
	_mat[Y_][X_] = (zm * (cdx * sdz - sdx * sdy * cdz));
	_mat[Y_][Y_] = (zm * (cdx * cdz + sdx * sdy * sdz));
	_mat[Y_][Z_] = (zm * -sdx * cdy);
	_mat[Y_][O_] = oy;
	_mat[Z_][X_] = (zm * (sdx * sdz + cdx * sdy * cdz));
	_mat[Z_][Y_] = (zm * (sdx * cdz - cdx * sdy * sdz));
	_mat[Z_][Z_] = (zm * cdx * cdy);
	_mat[Z_][O_] = oz;
	_mat[O_][X_] = ZERO;
	_mat[O_][Y_] = ZERO;
	_mat[O_][Z_] = ZERO;
	_mat[O_][O_] = UNIT;

	for (int i = X_; i <= O_; i++) {
		prep_row(i);
	}
	prep_cols();
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

	_mat[X_][X_] = (cdy * cdy);
	_mat[X_][Y_] = (-cdy * sdz);
	_mat[X_][Z_] = (-sdy);
	_mat[X_][O_] = ZERO;
	_mat[Y_][X_] = (cdx * sdz - sdx * sdy * cdy);
	_mat[Y_][Y_] = (cdx * cdy + sdx * sdy * sdz);
	_mat[Y_][Z_] = (-sdx * cdy);
	_mat[Y_][O_] = ZERO;
	_mat[Z_][X_] = (sdx * sdz + cdx * sdy * cdy);
	_mat[Z_][Y_] = (sdx * cdy - cdx * sdy * sdz);
	_mat[Z_][Z_] = (cdx * cdy);
	_mat[Z_][O_] = ZERO;
	_mat[O_][X_] = ZERO;
	_mat[O_][Y_] = ZERO;
	_mat[O_][Z_] = ZERO;
	_mat[O_][O_] = UNIT;

	for (int i = X_; i <= O_; i++) {
		prep_row(i);
	}
	prep_cols();
}

matrix get_unit_mat()
{
	return matrix(unt);
}

} // namespace matrix_ns
