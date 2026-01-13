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

// #define POLYGON_DEBUG

using matrix_ns::matrix;
using std::ifstream;
using std::list;
using std::pair;
using std::string;
using unit_ns::unit;
using vector_3_ns::vector_3;
using graphics = graphics_ns::graphics;
using ARGB = graphics_ns::graphics::ARGB;
using frame_buffer = graphics_ns::graphics::frame_buffer;
using coord_t = graphics::coord_t;
using color_t = graphics::color_t;
using val_t = graphics::val_t;

class polygon : public matrix
{
private:

	class drawing : public matrix
	{
	public:

		drawing(graphics&);
		~drawing() {}

		enum vec_type
		{
			VEC_PLAIN = 0, // regular vector
#ifdef POLYGON_DEBUG
			VEC_NORMAL // the normal of the polygon
#endif
		};

		// clang-format off
		struct polypoint
		{
			coord_t x {0};
			coord_t y {0};
#ifdef POLYGON_DEBUG
			vec_type type {VEC_PLAIN};
			polypoint() = default;
			polypoint(coord_t x_, coord_t y_, vec_type type_ = VEC_PLAIN) : x{x_}, y{y_}, type{type_} {}
#else
			polypoint() = default;
			polypoint(coord_t x_, coord_t y_) : x{x_}, y{y_} {}
#endif
		};
		// clang-format on

		typedef list<polypoint> scratchpad;

		frame_buffer _fb;

		frame_buffer clear();
		color_t get_color() const { return _argb.c; }
		void set_color(int);
		void make_color(unit, matrix&);
		void clear_scratch_pad();
		void project(const vector_3&, vec_type t = VEC_PLAIN);
		void draw();
		void present();

	private:

		graphics* _gfx {nullptr};
		graphics::addr_t _xy_addr {nullptr};
		polypoint _xy_pos;
		polypoint _min_pos;
		polypoint _max_pos;
		polypoint _vp_min_pos;
		polypoint _vp_mid_pos;
		polypoint _vp_max_pos;
		scratchpad _scratch_pad;
		matrix _draw_mat;
		ARGB _argb {};
		ARGB _draw_argb {};
		ARGB _clear {};
		ARGB _edge {};
#ifdef POLYGON_DEBUG
		ARGB _debug_argb {};
#endif

		graphics::addr_t offset(const polypoint&) const;
		bool is_in_bounds(const polypoint&, const polypoint&) const;
		void set_min_max();
		void adjust_min(polypoint&) const;
		void adjust_max(polypoint&) const;
		void moveto(const polypoint&);
		void lineto(const polypoint&, const ARGB&);
		void line(const polypoint&, const polypoint&, const ARGB&);
		void rect(const polypoint&, const polypoint&, const ARGB&);
		void putdirect(const ARGB& a) { *_xy_addr = a; }
		ARGB getdirect() const { return *_xy_addr; }
		void putpixel(const polypoint&, ARGB&);
		ARGB getpixel(const polypoint&);
	};

	typedef list<vector_3*> vec_list;

	string* _name {nullptr};
	vector_3 _fill;   // polygon middle point - average of all vectors
	vector_3 _normal; // polygon's direction
	vec_list _points;
	unit _depth {unit_ns::ZERO}; // objects location on the z axis
	drawing* _gfx_ctx;
	int _force {0}; // display polygon regardless of direction

	static vector_3 _view;    // camera position
	static vector_3 _n_light; // normalized light source position

#ifdef POLYGON_DEBUG
	vector_3 _draw_first;
	vector_3 _draw_normal;
#endif

	void gfx_draw();
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

	void sort_polygons();
	void show_polygons();

private:

	static poly_list _draw_list;
};

} // namespace polygon_ns
#endif //__POLYGON_H__
