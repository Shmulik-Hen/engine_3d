#pragma once

#include <list>
#include <utility>

// #define DEBUG_POLYGON
#include "common.h"
#include "graphics.h"
#include "matrix.h"
#include "vector_3.h"

namespace polygon_ns
{

using namespace matrix_ns;
using namespace vector_3_ns;
using namespace graphics_ns;
using addr_t = graphics_ns::graphics::addr_t;
using point = graphics_ns::graphics::point;
using ARGB = graphics_ns::graphics::ARGB;
using frame_buffer = graphics_ns::graphics::frame_buffer;
// using coord_t = graphics::coord_t;
// using color_t = graphics::color_t;
// using val_t = graphics::val_t;
// using unit_ns::unit;

class polygon
{
	class drawing
	{
	public:

		struct span
		{
			val_t x_left {0};
			val_t x_right {0};
		};

		typedef std::vector<point> scratchpad;
		typedef std::vector<span> spans_t;

		scratchpad _scratch_pad;
		spans_t _spans;
		bool _invalid {false};
		// current points
		addr_t _xy_addr {nullptr};
		point _xy_pos;
		// screen min/max points
		point _min_pos;
		point _max_pos;
		// view port min/mid/max points
		// can be screen size or less
		point _vp_min_pos;
		point _vp_mid_pos;
		point _vp_max_pos;
		// shape's bounding box points
		// minimize filling calculations
		point _bbox_tl;
		point _bbox_br;
		ARGB _base_color {};
		ARGB _draw_color {};
		ARGB _clear_color {};

		graphics* _gfx {nullptr};
		frame_buffer _fb;
		matrix _gen_mat;
		matrix _rot_mat;
#ifdef DEBUG_POLYGON
		ARGB _debug_color {};
		ARGB _bbox_color {};
		ARGB _normal_color {};
		ARGB _fill_color {};
		vector_3 _debug_normal;
		vector_3 _debug_fill;
		point _normal_point;
		point _fill_point;
#endif

		drawing(graphics&);
		~drawing() {}

		addr_t offset(const point&) const;
		void set_min_max();
		void adjust_min(point&);
		void adjust_max(point&);
		void create_bbox();
		color_t get_color() const { return _base_color.c; }
		void putdirect(const ARGB& a) { *_xy_addr = a; }
		ARGB getdirect() const { return *_xy_addr; }
		void putpixel(const point&, ARGB&);
		ARGB getpixel(const point&);
		void moveto(const point&);
		void lineto(const point&);
		void bar(val_t, val_t);
		void line(const point&, const point&);
		void rect(const point&, const point&);
		frame_buffer clear();
		void set_color(int);
		void make_color(unit);
		void clear_scratch_pad();
		void plot();
		void fill();
		void project(const vector_3&);
		void draw();
		void present();
	}; // class polygon::drawing

	typedef std::vector<vector_3*> vector_t;

	std::string* _name {nullptr};
	vector_3 _fill;   // polygon middle point - average of all vectors
	vector_3 _normal; // polygon's direction
	vector_t _points;
	double _depth {ZERO}; // objects location on the z axis
	drawing* _gfx_ctx;
	int _force {0}; // display polygon regardless of direction

	static vector_3 _cam_position;    // camera position
	static vector_3 _cam_direction;   // camera direction (normalized)
	static vector_3 _light_position;  // light source position
	static vector_3 _light_direction; // light source direction (normalized)

	bool is_consec();
	bool is_degenerate();
	bool is_planar();
	bool verify();

	void gfx_draw();
	vector_3 find_fill();
	vector_3 find_normal();

public:

	typedef std::list<polygon*> poly_list;

	polygon(graphics&);
	~polygon();

	bool read(std::ifstream&);
	void print() const;
	void update(matrix&, matrix&);
	void sort();
	void show_all();

	const std::string* get_name() const { return _name; }
	double get_depth() const { return _depth; }

	void sort_polygons();
	void show_polygons();

private:

	static poly_list _draw_list;
};

} // namespace polygon_ns
