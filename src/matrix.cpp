// #define DEBUG_PRINTS
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

matrix::vector_4& matrix::vector_4::operator=(const vector_4& v)
{
	if (this != &v) {
		for (int i = X_; i <= O_; i++) {
			_coords[i] = v._coords[i];
		}
	}
	return *this;
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

unit matrix::vector_4::operator*(const vector_4& v) const
{
	return unit(_coords[X_] * v._coords[X_] +
	            _coords[Y_] * v._coords[Y_] +
	            _coords[Z_] * v._coords[Z_] +
	            _coords[O_] * v._coords[O_]);
}

// ---------------------- end of class matrix::vector_4 -------------------------

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

matrix::matrix(const matrix& m)
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

void matrix::print() const
{
#ifdef DEBUG_PRINTS
	DBG("matrix:");
	for (int i = X_; i <= O_; i++) {
		for (int j = X_; j <= O_; j++) {
			DBG("    [" << i << "][" << j << "] : " << _mat[i][j]);
		}
	}
	DBG("");
#endif
}

matrix matrix::operator*(const matrix& m) const
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

vector_3 matrix::operator*(const vector_3& v) const
{
	vector_3 v1(_mat[X_][X_], _mat[X_][Y_], _mat[X_][Z_]);
	vector_3 v2(_mat[Y_][X_], _mat[Y_][Y_], _mat[Y_][Z_]);
	vector_3 v3(_mat[Z_][X_], _mat[Z_][Y_], _mat[Z_][Z_]);

	return vector_3(vector_3::dot(v, v1) + _mat[X_][O_],
	                vector_3::dot(v, v2) + _mat[Y_][O_],
	                vector_3::dot(v, v3) + _mat[Z_][O_]);
}

void matrix::prep_gen_mat(const attrib& a)
{
	unit dx = a.get_deg_x() * DEG_TO_RAD;
	unit dy = a.get_deg_y() * DEG_TO_RAD;
	unit dz = a.get_deg_z() * DEG_TO_RAD;
	unit ox = a.get_off_x();
	unit oy = a.get_off_y();
	unit oz = a.get_off_z();
	unit zm = a.get_zoom();

	DBG("prep_gen_mat:");
	DBG("      dx: " << dx << " dy: " << dy << " dz: " << dz);
	DBG("      ox: " << ox << " oy: " << oy << " oz: " << oz);
	DBG("    zoom: " << zm);

	unit sdx = std::sin(dx);
	unit sdy = std::sin(dy);
	unit sdz = std::sin(dz);
	unit cdx = std::cos(dx);
	unit cdy = std::cos(dy);
	unit cdz = std::cos(dz);

	DBG("     sdx: " << sdx << " sdy: " << sdy << " sdz: " << sdz);
	DBG("     cdx: " << cdx << " cdy: " << cdy << " cdz: " << cdz << ENDL);

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

#ifdef DEBUG_PRINTS
	print();
#endif
}

void matrix::prep_rot_mat(const attrib& a)
{
	unit dx = a.get_deg_x() * DEG_TO_RAD;
	unit dy = a.get_deg_y() * DEG_TO_RAD;
	unit dz = a.get_deg_z() * DEG_TO_RAD;

	DBG("prep_rot_mat:");
	DBG("      dx: " << dx << " dy: " << dy << " dz: " << dz);

	unit sdx = std::sin(dx);
	unit sdy = std::sin(dy);
	unit sdz = std::sin(dz);
	unit cdx = std::cos(dx);
	unit cdy = std::cos(dy);
	unit cdz = std::cos(dz);

	DBG("     sdx: " << sdx << " sdy: " << sdy << " sdz: " << sdz);
	DBG("     cdx: " << cdx << " cdy: " << cdy << " cdz: " << cdz << ENDL);

	_mat[X_][X_] = (cdy * cdz);
	_mat[X_][Y_] = (-cdy * sdz);
	_mat[X_][Z_] = (-sdy);
	_mat[X_][O_] = ZERO;

	_mat[Y_][X_] = (cdx * sdz - sdx * sdy * cdz);
	_mat[Y_][Y_] = (cdx * cdz + sdx * sdy * sdz);
	_mat[Y_][Z_] = (-sdx * cdy);
	_mat[Y_][O_] = ZERO;

	_mat[Z_][X_] = (sdx * sdz + cdx * sdy * cdz);
	_mat[Z_][Y_] = (sdx * cdz - cdx * sdy * sdz);
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

#ifdef DEBUG_PRINTS
	print();
#endif
}

matrix get_unit_mat()
{
	return unt;
}

} // namespace matrix_ns
