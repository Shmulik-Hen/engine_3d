#pragma once

#include <list>

#include "common.h"
#include "attrib.h"
#include "matrix.h"
#include "polygon.h"
#include "treenode.h"

namespace element_ns
{

using namespace attrib_ns;
using namespace matrix_ns;
using namespace polygon_ns;
using namespace treenode_ns;
// using unit_ns::unit;

class element : public treenode<element>
{
public:

	element();
	~element();

	bool read(const polygon::poly_list&, element**, std::ifstream&);
	void print() const;
	void print_all();

	element* find(element*, std::string&) const;

	void update(const attrib&);
	void update(const matrix&, const matrix&);
	void update();
	void update_all();

	const std::string* get_name() const { return _name; }

private:

	polygon::poly_list _polygons;
	std::string* _name {nullptr};
	std::string* _parrent_name {nullptr};
	element* _parrent {nullptr};
	matrix _gen_mat;
	matrix _rot_mat;
	attrib _att;
	bool _active {false};
	bool _dirty {false};
	static bool _mats_prepared;

	polygon* find(const polygon::poly_list&, const std::string&) const;
};

} // namespace element_ns
