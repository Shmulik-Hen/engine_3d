#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <list>
#include <utility>

#include "common.h"
#include "graphics.h"
#include "matrix.h"
#include "unit.h"
#include "vector_3.h"

namespace polygon_ns
{

using graphics_ns::graphics;
using matrix_ns::matrix;
using std::ifstream;
using std::list;
using std::pair;
using std::string;
using unit_ns::unit;
using vector_3_ns::vector_3;
using ARGB = graphics_ns::graphics::ARGB;
using frame_buffer = graphics_ns::graphics::frame_buffer;

class polygon : public matrix
{
private:

	class drawing : public matrix
	{
	public:

		drawing(graphics&);
		~drawing() {}

		typedef list<graphics::point> scratchpad;

		frame_buffer clear();
		graphics::color_t get_color() const { return _argb.c; }
		void set_color(int);
		void make_color(unit, matrix&);
		void clear_scratch_pad();
		void prepare(const vector_3&);
		void draw(frame_buffer&);
		void present();

	private:

		graphics* _gfx {nullptr};
		graphics::addr_t _xy_addr {nullptr};
		graphics::point _xy_pos {};
		graphics::point _min_pos {};
		graphics::point _max_pos {};
		graphics::point _vp_min_pos {};
		graphics::point _vp_mid_pos {};
		graphics::point _vp_max_pos {};
		scratchpad _scratch_pad;
		matrix _draw_mat;
		ARGB _argb {};
		ARGB _draw_argb {};
		ARGB _clear {};
		ARGB _edge {};

		graphics::addr_t offset(const graphics::point&, const frame_buffer&) const;
		bool is_in_bounds(const graphics::point&, const graphics::point&) const;
		void set_min_max();
		void adjust_min(graphics::point&) const;
		void adjust_max(graphics::point&) const;
		void moveto(const graphics::point&, const frame_buffer&);
		void lineto(const graphics::point&, const ARGB&);
		void line(const graphics::point&, const graphics::point&, const ARGB&, frame_buffer&);
		void rect(const graphics::point&, const graphics::point&, const ARGB&, frame_buffer&);
		void putdirect(const ARGB& a) { *_xy_addr = a; }
		void putpixel(const graphics::point&, ARGB&, frame_buffer&);
		ARGB getpixel(const graphics::point&, const frame_buffer&) const;
	};

	typedef list<vector_3*> vec_list;

	string* _name {nullptr};
	vector_3 _fill;
	vector_3 _normal;
	vec_list _points;
	unit _depth {unit_ns::ZERO};
	drawing* _gfx_ctx;
	int _force {0};

	static vector_3 _view;
	static vector_3 _n_light;

	frame_buffer gfx_clear();
	void gfx_draw(frame_buffer&);
	void gfx_present();
	vector_3 find_fill();
	vector_3 find_normal();

public:

	typedef list<polygon*> poly_list;

	polygon(graphics&);
	~polygon();

	bool read(ifstream&);
	void print() const;
	void update(matrix&, matrix&);
	void sort();
	void show_all();

	const string* get_name() const { return _name; }
	unit get_depth() const { return _depth; }

	static void sort_polygons();
	static void show_polygons();

private:

	static poly_list _draw_list;
};

} // namespace polygon_ns
#endif //__POLYGON_H__
