#pragma once

#include <list>

#include "common.h"
#include "attrib.h"
#include "matrix.h"
#include "polygon.h"
#include "treenode.h"

// clang-format off
namespace attrib_ns { class attrib; }
namespace graphics_ns { class graphics; }
namespace matrix_ns { class matrix; }
namespace vector_3_ns { class vector_3; }
// clang-format on

namespace element_ns
{

class element : public treenode_ns::treenode<element>
{
public:

	element() = default;
	~element() = default;

	bool read(const polygon_ns::polygon::polylist_t&, element*, std::ifstream&);
	void print() const;
	void print_all();

	element* find(element*, const std::string&) const;

	void update(const attrib_ns::attrib&);
	void update(const matrix_ns::matrix&, const matrix_ns::matrix&);
	void update();
	void update_all();

	const std::string get_name() const { return _name; }

private:

	polygon_ns::polygon::polylist_t _polygons;
	std::string _name;
	std::string _parrent_name;
	element* _parrent {nullptr}; // raw pointer, non-owning
	matrix_ns::matrix _trans_mat;
	matrix_ns::matrix _rot_mat;
	attrib_ns::attrib _att;
	bool _active {false};
	bool _dirty {false};
	static bool _mats_prepared;

	polygon_ns::polygon* find(const polygon_ns::polygon::polylist_t&, const std::string&) const;
};

} // namespace element_ns
