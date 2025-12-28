#ifndef __MY_VECTOR_H__
#define __MY_VECTOR_H__

#include <fstream>
#include <map>

#include "common.h"
#include "unit.h"

namespace my_vector_ns
{

using std::ifstream;
using std::istream;
using std::map;
using std::ostream;
using unit_ns::unit;

const int dim = 3;

class my_vector
{
public:

	enum class coord
	{
		__first_coord = 0, // dummy
		X = __first_coord,
		Y = 1,
		Z = 2,
		__last_coord // dummy
	};

	my_vector();
	my_vector(const unit, const unit, const unit);
	~my_vector() {}

	bool read(ifstream&);
	void print() const;
	unit get_coord(coord) const;
	my_vector project(const my_vector&, const my_vector&);
	void normalize(my_vector&);

	my_vector operator+(const my_vector&) const;
	my_vector operator-(const my_vector&) const;
	my_vector& operator+=(const my_vector&);
	my_vector& operator-=(const my_vector&);
	const unit operator*(const my_vector&) const;

	friend ostream& operator<<(ostream&, const my_vector&);
	friend istream& operator>>(istream&, my_vector&);

private:

	typedef map<coord, unit> vec_entry;
	vec_entry _coords;

	bool is_valid_coord(coord) const;
};

inline my_vector::coord& operator++(my_vector::coord& c)
{
	c = static_cast<my_vector::coord>(static_cast<int>(c) + 1);
	return c;
}

inline my_vector::coord operator++(my_vector::coord& c, int)
{
	my_vector::coord d = c;
	++c;
	return d;
}

} // namespace my_vector_ns

#endif //__MY_VECTOR_H__
