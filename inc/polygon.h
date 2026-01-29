#pragma once

#include <list>
#include <utility>

// #define DEBUG_POLYGON
#include "common.h"
#include "graphics.h"
#include "matrix.h"
#include "unit.h"
#include "vector_3.h"

// clang-format off
namespace graphics_ns { class graphics; }
namespace matrix_ns { class matrix; }
namespace vector_3_ns { class vector_3; }
// clang-format on

namespace polygon_ns
{

class polygon
{
	class drawing
	{
	public:

		struct span
		{
			graphics_ns::val_t x_left {0};
			graphics_ns::val_t x_right {0};
		};

		typedef std::vector<graphics_ns::graphics::point> scratchpad_t;
		typedef std::vector<span> spans_t;

		scratchpad_t _scratch_pad;
		spans_t _spans;
		bool _invalid {false};
		// current point
		graphics_ns::graphics::addr_t _xy_addr {nullptr};
		graphics_ns::graphics::point _xy_pos;
		// screen min/max points
		graphics_ns::graphics::point _min_pos;
		graphics_ns::graphics::point _max_pos;
		// view port min/mid/max points - can be screen size or less
		graphics_ns::graphics::point _vp_min_pos;
		graphics_ns::graphics::point _vp_mid_pos;
		graphics_ns::graphics::point _vp_max_pos;
		// shape's bounding box points - minimize filling area calculations
		graphics_ns::graphics::point _bbox_tl;
		graphics_ns::graphics::point _bbox_br;
		graphics_ns::graphics::ARGB _base_color {};
		graphics_ns::graphics::ARGB _draw_color {};
		graphics_ns::graphics::ARGB _clear_color {};

		graphics_ns::graphics* _gfx {nullptr};
		graphics_ns::graphics::frame_buffer _fb;
		matrix_ns::matrix _trans_mat;
		matrix_ns::matrix _rot_mat;
#ifdef DEBUG_POLYGON
		vector_3_ns::vector_3 _debug_normal;
		vector_3_ns::vector_3 _debug_fill;
		graphics_ns::graphics::ARGB _debug_color {};
		graphics_ns::graphics::ARGB _bbox_color {};
		graphics_ns::graphics::ARGB _normal_color {};
		graphics_ns::graphics::ARGB _fill_color {};
		graphics_ns::graphics::point _normal_point;
		graphics_ns::graphics::point _fill_point;
#endif

		drawing(graphics_ns::graphics&);
		~drawing() {}

		graphics_ns::graphics::addr_t offset(const graphics_ns::graphics::point&) const;
		graphics_ns::color_t get_color() const { return _base_color.c; }
		graphics_ns::graphics::ARGB getdirect() const { return *_xy_addr; }
		graphics_ns::graphics::ARGB getpixel(const graphics_ns::graphics::point&);
		graphics_ns::graphics::frame_buffer clear();
		void set_min_max();
		void adjust_min(graphics_ns::graphics::point&);
		void adjust_max(graphics_ns::graphics::point&);
		void create_bbox();
		void putdirect(const graphics_ns::graphics::ARGB& a) { *_xy_addr = a; }
		void putpixel(const graphics_ns::graphics::point&, graphics_ns::graphics::ARGB&);
		void moveto(const graphics_ns::graphics::point&);
		void lineto(const graphics_ns::graphics::point&);
		void bar(graphics_ns::val_t, graphics_ns::val_t);
		void line(const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void rect(const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void set_color(int);
		void make_color(unit);
		void clear_scratch_pad();
		void plot();
		void fill();
		void project(const vector_3_ns::vector_3&);
		void draw();
		void present();
	}; // class polygon::drawing

	typedef std::vector<vector_3_ns::vector_3*> vector_t;

	std::string _name;
	vector_3_ns::vector_3 _fill;   // polygon middle point - average of all vectors
	vector_3_ns::vector_3 _normal; // polygon's direction
	vector_t _points;              // owning
	double _depth {ZERO};          // objects location on the z axis
	drawing* _gfx_ctx {nullptr};   // raw pointer, non owning
	int _force {0};                // display polygon regardless of direction

	static vector_3_ns::vector_3 _cam_position;    // camera position
	static vector_3_ns::vector_3 _cam_direction;   // camera direction (normalized)
	static vector_3_ns::vector_3 _light_position;  // light source position
	static vector_3_ns::vector_3 _light_direction; // light source direction (normalized)

	bool is_consec();
	bool is_degenerate();
	bool is_planar();
	bool verify();

	void gfx_draw();
	vector_3_ns::vector_3 find_fill();
	vector_3_ns::vector_3 find_normal();

public:

	typedef std::list<polygon*> polylist_t;

	polygon(graphics_ns::graphics&);
	~polygon();

	bool read(std::ifstream&);
	void print() const;
	void update(matrix_ns::matrix&, matrix_ns::matrix&);
	void sort();
	void show_all();

	const std::string get_name() const { return _name; }
	double get_depth() const { return _depth; }

	void sort_polygons();
	void show_polygons();

private:

	static polylist_t _draw_list;
};

} // namespace polygon_ns
