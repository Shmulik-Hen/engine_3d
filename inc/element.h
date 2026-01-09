#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <list>

#include "common.h"
#include "attrib.h"
#include "matrix.h"
#include "polygon.h"
#include "treenode.h"
#include "unit.h"

namespace element_ns
{

using attrib_ns::attrib;
using matrix_ns::matrix;
using polygon_ns::polygon;
using std::ifstream;
using std::list;
using std::string;
using unit_ns::unit;
using namespace treenode_ns;
using treenode_ns::treenode;

class element : public treenode<element>
{
public:

	element();
	~element();

	bool read(const polygon::poly_list&, element**, ifstream&);
	void print() const;
	void print_all();

	element* find(element*, string&) const;

	void update(const attrib&);
	void update(const matrix&, const matrix&);
	void update();
	void update_all();

	const string* get_name() const { return _name; }

private:

	polygon::poly_list _polygons;
	string* _name {nullptr};
	string* _parrent_name {nullptr};
	element* _parrent {nullptr};
	matrix _gen_mat;
	matrix _rot_mat;
	attrib _att;
	bool _active {false};
	bool _dirty {false};
	bool _mats_prepared {false};

	polygon* find(const polygon::poly_list&, const string&) const;
};

} // namespace element_ns
#endif //__ELEMENT_H__
