#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <fstream>
#include <list>
#include <string>

#include "common.h"
#include "matrix.h"
#include "vector_3.h"
#include "unit.h"

namespace polygon_ns
{

using matrix_ns::matrix;
using std::ifstream;
using std::list;
using std::string;
using unit_ns::unit;
using vector_3_ns::vector_3;

class polygon : public matrix
{
public:

	typedef list<polygon> poly_list;
	typedef poly_list::iterator pol_it;
	typedef poly_list::const_iterator con_pol_it;

	polygon();
	~polygon();

	bool read(ifstream&);
	void print() const;
	const string* get_name() const { return (_name ? _name : nullptr); }
	void update(matrix&, matrix&);

	friend int operator<(const polygon&, const polygon&);

private:

	typedef list<vector_3> vec_list;
	typedef vec_list::const_iterator vec_it;

	string* _name {nullptr};
	int _force {0};
	char _color {0};
	unit _depth {unit_ns::ZERO};
	vector_3 _fill;
	vector_3 _normal;
	vec_list _points;

	vector_3 find_fill();
	vector_3 find_normal();
};

} // namespace polygon_ns
#endif //__POLYGON_H__
