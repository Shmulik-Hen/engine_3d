// #define DEBUG_PRINTS
#include "vector_3.h"
#include "utils.h"

namespace vector_3_ns
{

vector_3::vector_3()
{
	_coords[X_] = ZERO;
	_coords[Y_] = ZERO;
	_coords[Z_] = ZERO;
}

vector_3::vector_3(const vector_3& v)
{
	_coords[X_] = v._coords[X_];
	_coords[Y_] = v._coords[Y_];
	_coords[Z_] = v._coords[Z_];
}

vector_3::vector_3(const unit v[dim])
{
	_coords[X_] = v[X_];
	_coords[Y_] = v[Y_];
	_coords[Z_] = v[Z_];
}

vector_3::vector_3(const unit x, const unit y, const unit z)
{
	_coords[X_] = x;
	_coords[Y_] = y;
	_coords[Z_] = z;
}

bool vector_3::read(std::ifstream& ifs)
{
	LINE line;

	for (int i = X_; i <= Z_; i++) {
		while (!read_word(ifs, line));
		_coords[i] = strtof(line, NULL);
	}

	return true;
}

void vector_3::print() const
{
#ifdef DEBUG_PRINTS
	DBG(STR("  vector:", 1));
	for (int i = X_; i <= Z_; i++) {
		DBG("    i: " << i << ": " << _coords[i]);
	}
#endif // DEBUG_PRINTS
}

unit vector_3::get(int i) const
{
	if (is_valid_coord(i)) {
		return _coords[i];
	}

	return ZERO;
}

void vector_3::set(const int i, const unit& u)
{
	if (is_valid_coord(i)) {
		_coords[i] = u;
	}
}

unit vector_3::dot(const vector_3& v1, const vector_3& v2)
{
	return (v1._coords[X_] * v2._coords[X_] +
	        v1._coords[Y_] * v2._coords[Y_] +
	        v1._coords[Z_] * v2._coords[Z_]);
}

vector_3 vector_3::cross(const vector_3& v1, const vector_3& v2)
{
	return {v1._coords[Y_] * v2._coords[Z_] - v1._coords[Z_] * v2._coords[Y_],
	        v1._coords[Z_] * v2._coords[X_] - v1._coords[X_] * v2._coords[Z_],
	        v1._coords[X_] * v2._coords[Y_] - v1._coords[Y_] * v2._coords[X_]};
}

vector_3 vector_3::project(const vector_3& t, const vector_3& v)
{
	vector_3 temp;

	if (std::abs(v._coords[Z_] - t._coords[Z_]) >= UNIT) {
		temp._coords[X_] = ((t._coords[X_] - v._coords[X_]) /
		                    (v._coords[Z_] - t._coords[Z_]) *
		                    v._coords[Z_]);
		temp._coords[Y_] = ((t._coords[Y_] - v._coords[Y_]) /
		                    (v._coords[Z_] - t._coords[Z_]) *
		                    v._coords[Z_]);
	}
	else {
		temp._coords[X_] = ((t._coords[X_] - v._coords[X_]) *
		                    v._coords[Z_] /
		                    (v._coords[Z_] - t._coords[Z_] + UNIT));
		temp._coords[Y_] = ((t._coords[Y_] - v._coords[Y_]) *
		                    v._coords[Z_] /
		                    (v._coords[Z_] - t._coords[Z_] + UNIT));
	}

	return temp;
}

vector_3 vector_3::normalize(const vector_3& v)
{
	unit sqr_len = dot(v, v);

	if (sqr_len <= EPSILON * EPSILON) {
		return {ZERO, ZERO, ZERO};
	}

	unit inv_len = UNIT / std::sqrt(sqr_len);
	return v * inv_len;
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

vector_3 vector_3::operator*(const unit u) const
{
	return vector_3(_coords[X_] * u,
	                _coords[Y_] * u,
	                _coords[Z_] * u);
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

std::ostream& operator<<(std::ostream& os, const vector_3& v)
{
	return os << '(' << v._coords[X_] << ','
	          << v._coords[Y_] << ','
	          << v._coords[Z_] << ')';
}

std::istream& operator>>(std::istream& is, vector_3& v)
{
	is >> v._coords[X_];
	is >> v._coords[Y_];
	is >> v._coords[Z_];
	return is;
}

} // namespace vector_3_ns
