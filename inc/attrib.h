#ifndef __ATTRIB_H__
#define __ATTRIB_H__

#include <iostream>
#include "unit.h"

using std::ifstream;
using std::istream;
using std::ostream;

class matrix;
class attrib
{
	unit deg_x{0};
	unit deg_y{0};
	unit deg_z{0};
	unit off_x{0};
	unit off_y{0};
	unit off_z{0};
	unit zoom{0};

public:

	attrib();
	~attrib();
	attrib(unit, unit, unit, unit, unit, unit, unit);
	friend attrib operator+(const attrib &, const attrib &);
	attrib &operator+=(const attrib &);
	friend void prep_gen_mat(matrix &, const attrib &);
	friend void prep_rot_mat(matrix &, const attrib &);
	friend ostream &operator<<(ostream &, const attrib &);
	friend istream &operator>>(istream &, attrib &);
	void read(ifstream &);
	void print();
};

#endif //__ATTRIB_H__
