#include "vector_3.h"

namespace vector_3_ns
{

vector_3::vector_3()
{
	for (int i = X_; i <= Z_; i++) {
		_coords[i] = ZERO;
	}
}

vector_3::vector_3(const vector_3& v)
{
	for (int i = X_; i <= Z_; i++) {
		_coords[i] = v._coords[i];
	}
}

vector_3::vector_3(const unit v[dim])
{
	for (int i = X_; i <= Z_; i++) {
		_coords[i] = v[i];
	}
}

vector_3::vector_3(const unit x, const unit y, const unit z)
{
	_coords[X_] = x;
	_coords[Y_] = y;
	_coords[Z_] = z;
}

bool vector_3::read(ifstream& f)
{
	bool rc;

	for (int i = X_; i <= Z_; i++) {
		rc = _coords[i].read(f);
		if (!rc) {
			return false;
		}
	}

	return true;
}

void vector_3::print() const
{
	DBG(STR("          vector:", 1));
	for (int i = X_; i <= Z_; i++) {
		_coords[i].print();
	}
}

unit vector_3::get(int c) const
{
	if (c >= X_ && c <= Z_) {
		return _coords[c];
	}

	return ZERO;
}

vector_3 vector_3::project(const vector_3& p, const vector_3& v)
{
	vector_3 temp;

	if (abs(v._coords[Z_] - p._coords[Z_]) >= UNIT) {
		temp._coords[X_] = ((p._coords[X_] - v._coords[X_]) /
		                    (v._coords[Z_] - p._coords[Z_]) *
		                    v._coords[Z_]);
		temp._coords[Y_] = ((p._coords[Y_] - v._coords[Y_]) /
		                    (v._coords[Z_] - p._coords[Z_]) *
		                    v._coords[Z_]);
	}
	else {
		temp._coords[X_] = ((p._coords[X_] - v._coords[X_]) *
		                    v._coords[Z_] /
		                    (v._coords[Z_] - p._coords[Z_] + UNIT));
		temp._coords[Y_] = ((p._coords[Y_] - v._coords[Y_]) *
		                    v._coords[Z_] /
		                    (v._coords[Z_] - p._coords[Z_] + UNIT));
	}

	return temp;
}

void vector_3::normalize(vector_3& v)
{
	unit norm = sqrt(v * v);
	v._coords[X_] /= norm;
	v._coords[Y_] /= norm;
	v._coords[Z_] /= norm;
}

vector_3 vector_3::operator+(const vector_3& v) const
{
	return vector_3(_coords[X_] + v._coords[X_],
	                _coords[Y_] + v._coords[Y_],
	                _coords[Z_] + v._coords[Z_]);
}

vector_3 vector_3::operator-(const vector_3& v) const
{
	return vector_3(_coords[X_] - v._coords[X_],
	                _coords[Y_] - v._coords[Y_],
	                _coords[Z_] - v._coords[Z_]);
}

vector_3& vector_3::operator+=(const vector_3& v)
{
	_coords[X_] += v._coords[X_];
	_coords[Y_] += v._coords[Y_];
	_coords[Z_] += v._coords[Z_];

	return *this;
}

vector_3& vector_3::operator-=(const vector_3& v)
{
	_coords[X_] -= v._coords[X_];
	_coords[Y_] -= v._coords[Y_];
	_coords[Z_] -= v._coords[Z_];

	return *this;
}

const unit vector_3::operator*(const vector_3& v) const
{
	return unit(_coords[X_] * v._coords[X_] +
	            _coords[Y_] * v._coords[Y_] +
	            _coords[Z_] * v._coords[Z_]);
}

ostream& operator<<(ostream& o, const vector_3& v)
{
	return o << '(' << v._coords[X_] << ','
	         << v._coords[Y_] << ','
	         << v._coords[Z_] << ')';
}

istream& operator>>(istream& i, vector_3& v)
{
	i >> v._coords[X_];
	i >> v._coords[Y_];
	i >> v._coords[Z_];
	return i;
}

} // namespace vector_3_ns
