#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <fstream>
#include <list>
#include <string>

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

class element : public matrix_ns::matrix,
		public polygon_ns::polygon,
		public treenode<element>
{
public:

	typedef list<element> elem_list;

	element();
	~element();

	bool read(poly_list*, element*, ifstream&);
	void print_all();
	void print() const;

	const polygon* find(poly_list*, const string&) const;
	element* find(elem_list*, string&) const;
	element* find(element*, string&) const;

	void update(const attrib&);
	void update(const attrib&, matrix&, matrix&);
	void update();
	void update_all();

	string* get_name() const { return _name; }
	matrix get_gen_mat() const { return _gen_mat; }
	matrix get_rot_mat() const { return _rot_mat; }
	attrib get_attrib() const { return _att; }

private:

	typedef elem_list::iterator elem_it;

	poly_list* _polygons;
	string* _name {nullptr};
	string* _parrent_name {nullptr};
	element* _parrent {nullptr};
	matrix _gen_mat;
	matrix _rot_mat;
	attrib _att;
	int _active {0};
	int _dirty {0};
};

} // namespace element_ns
#endif //__ELEMENT_H__
