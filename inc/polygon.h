#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <fstream>
#include <list>
#include <string>

#include "common.h"
#include "my_vector.h"
#include "unit.h"

namespace polygon_ns
{

using my_vector_ns::my_vector;
using std::ifstream;
using std::list;
using std::string;
using unit_ns::unit;

class polygon : public my_vector
{
public:

	typedef list<polygon> poly_list;
	typedef poly_list::iterator pol_it;

	polygon();
	~polygon();

	bool read(ifstream&);
	void print() const;

	const int get_force() const { return _force; }
	const char get_color() const { return _color; }
	const my_vector get_fill() const { return _fill; }
	const my_vector get_normal() const { return _normal; }
	const string* get_name() const { return (_name ? _name : nullptr); }

	void set_fill(my_vector f) { _fill = f; }
	void set_normal(my_vector n) { _normal = n; }
	void set_color(char c) { _color = c; }
	void set_depth(unit d) { _depth = d; }

	friend int operator<(const polygon&, const polygon&);

private:

	typedef list<my_vector> vec_list;
	typedef vec_list::const_iterator vec_it;

	string* _name {nullptr};
	int _force {0};
	char _color {0};
	unit _depth {unit_ns::ZERO};
	my_vector _fill;
	my_vector _normal;
	vec_list* _points {nullptr};

	my_vector find_fill();
	my_vector find_normal();
};

} // namespace polygon_ns
#endif //__POLYGON_H__
