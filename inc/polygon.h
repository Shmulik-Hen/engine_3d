#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <list>

#include "common.h"
#include "graphics.h"
#include "matrix.h"
#include "vector_3.h"
#include "unit.h"

namespace polygon_ns
{

using graphics_ns::graphics;
using matrix_ns::matrix;
using std::ifstream;
using std::list;
using std::string;
using unit_ns::unit;
using vector_3_ns::vector_3;
using ARGB = graphics_ns::graphics::ARGB;
using frame_buffer = graphics_ns::graphics::frame_buffer;

class polygon : public matrix
{
public:

	typedef list<polygon*> poly_list;

	polygon(graphics&);
	~polygon();

	bool read(ifstream&);
	void print() const;
	const string* get_name() const { return (_name ? _name : nullptr); }
	unit get_depth() const { return _depth; }
	void update(matrix&, matrix&);

	void clear();
	void sort();
	void show(frame_buffer&);
	void show_all();

private:

	typedef list<vector_3*> vec_list;

	string* _name {nullptr};
	int _force {0};
	char _color {0};
	char _draw_color {0};
	unit _depth {unit_ns::ZERO};
	vector_3 _fill;
	vector_3 _normal;
	vec_list _points;
	matrix _draw_mat;
	graphics* _gfx {nullptr};
	ARGB _argb;
	ARGB _clear;

	static poly_list _draw_list;
	static vector_3 _view;
	static vector_3 _n_light;

	vector_3 find_fill();
	vector_3 find_normal();
	void make_color(unit);
};

} // namespace polygon_ns
#endif //__POLYGON_H__
