#include "my_vector.h"

namespace my_vector_ns
{

my_vector::my_vector()
{
	_coords.clear();
	_coords[coord::X] = unit_ns::ZERO;
	_coords[coord::Y] = unit_ns::ZERO;
	_coords[coord::Z] = unit_ns::ZERO;
}

my_vector::my_vector(const unit x, const unit y, const unit z)
{
	_coords.clear();
	_coords[coord::X] = x;
	_coords[coord::Y] = y;
	_coords[coord::Z] = z;
}

bool my_vector::read(ifstream& f)
{
	bool rc;

	rc = _coords[coord::X].read(f);
	if (!rc) {
		return false;
	}

	rc = _coords[coord::Y].read(f);
	if (!rc) {
		return false;
	}

	rc = _coords[coord::Z].read(f);
	if (!rc) {
		return false;
	}

	return true;
}

void my_vector::print() const
{
	DBG(STR("          vector:", 1));
	_coords.find(coord::X)->second.print();
	_coords.find(coord::Y)->second.print();
	_coords.find(coord::Z)->second.print();
}

unit my_vector::get_coord(coord c) const
{
	unit u = unit_ns::ZERO;

	if (is_valid_coord(c)) {
		u = _coords.find(c)->second;
	}

	return u;
}

my_vector my_vector::project(const my_vector& p, const my_vector& v)
{
	my_vector temp;

	unit px = p._coords.at(coord::X);
	unit py = p._coords.at(coord::Y);
	unit pz = p._coords.at(coord::Z);
	unit vx = v._coords.at(coord::X);
	unit vy = v._coords.at(coord::Y);
	unit vz = v._coords.at(coord::Z);

	if (abs(vz - pz) >= unit_ns::UNIT) {
		temp._coords.at(coord::X) = ((px - vx) / (vz - pz) * vz);
		temp._coords.at(coord::Y) = ((py - vy) / (vz - pz) * vz);
	}
	else {
		temp._coords.at(coord::X) = ((px - vx) * vz / (vz - pz + unit_ns::UNIT));
		temp._coords.at(coord::Y) = ((py - vy) * vz / (vz - pz + unit_ns::UNIT));
	}

	return temp;
}

void my_vector::normalize(my_vector& v)
{
	unit norm = sqrt(v * v);
	v._coords.at(coord::X) /= norm;
	v._coords.at(coord::Y) /= norm;
	v._coords.at(coord::Z) /= norm;
}

my_vector my_vector::operator+(const my_vector& v) const
{
	return my_vector(_coords.at(coord::X) + v._coords.at(coord::X),
	                 _coords.at(coord::Y) + v._coords.at(coord::Y),
	                 _coords.at(coord::Z) + v._coords.at(coord::Z));
}

my_vector my_vector::operator-(const my_vector& v) const
{
	return my_vector(_coords.at(coord::X) - v._coords.at(coord::X),
	                 _coords.at(coord::Y) - v._coords.at(coord::Y),
	                 _coords.at(coord::Z) - v._coords.at(coord::Z));
}

my_vector& my_vector::operator+=(const my_vector& v)
{
	_coords.at(coord::X) += v._coords.at(coord::X);
	_coords.at(coord::Y) += v._coords.at(coord::Y);
	_coords.at(coord::Z) += v._coords.at(coord::Z);
	return *this;
}

my_vector& my_vector::operator-=(const my_vector& v)
{
	_coords.at(coord::X) -= v._coords.at(coord::X);
	_coords.at(coord::Y) -= v._coords.at(coord::Y);
	_coords.at(coord::Z) -= v._coords.at(coord::Z);
	return *this;
}

const unit my_vector::operator*(const my_vector& v) const
{
	return unit(_coords.at(coord::X) * v._coords.at(coord::X) +
	            _coords.at(coord::Y) * v._coords.at(coord::Y) +
	            _coords.at(coord::Z) * v._coords.at(coord::Z));
}

ostream& operator<<(ostream& o, const my_vector& v)
{
	return o << '(' << v._coords.at(my_vector::coord::X) << ','
	         << v._coords.at(my_vector::coord::Y) << ','
	         << v._coords.at(my_vector::coord::Z) << ')';
}

istream& operator>>(istream& i, my_vector& v)
{
	i >> v._coords.at(my_vector::coord::X);
	i >> v._coords.at(my_vector::coord::Y);
	i >> v._coords.at(my_vector::coord::Z);
	return i;
}

bool my_vector::is_valid_coord(coord c) const
{
	return (c >= coord::__first_coord && c < coord::__last_coord);
}

} // namespace my_vector_ns
