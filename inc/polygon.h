#pragma once

#include <list>
#include <utility>
#include <vector>

// #define DEBUG_POLYGON
#include "common.h"
#include "graphics.h"
#include "matrix.h"
#include "unit.h"
#include "vector_3.h"
#include "scene_types.h"

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

		drawing(const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		~drawing() {}

		graphics_ns::graphics::addr_t offset(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&) const;
		graphics_ns::color_t get_color() const { return _base_color.c; }
		graphics_ns::graphics::ARGB getdirect() const { return *_xy_addr; }
		graphics_ns::graphics::ARGB getpixel(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&);
		graphics_ns::graphics::frame_buffer clear();
		void set_min_max(const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void adjust_min(graphics_ns::graphics::point&);
		void adjust_max(graphics_ns::graphics::point&);
		void create_bbox();
		void putdirect(const graphics_ns::graphics::ARGB& a) { *_xy_addr = a; }
		void putpixel(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&, graphics_ns::graphics::ARGB&);
		void moveto(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&);
		void lineto(const graphics_ns::graphics::point&);
		void bar(graphics_ns::val_t, graphics_ns::val_t);
		void line(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void rect(graphics_ns::graphics::frame_buffer&, const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void set_color(const graphics_ns::graphics*, int);
		void make_color(unit);
		void clear_scratch_pad();
		void plot(graphics_ns::graphics::frame_buffer&);
		void fill(graphics_ns::graphics::frame_buffer&);
		void project(const vector_3_ns::vector_3&, const vector_3_ns::vector_3&);
		void draw(graphics_ns::graphics::frame_buffer&);
	}; // class polygon::drawing

	typedef std::vector<vector_3_ns::vector_3*> vector_t;

	std::string _name;
	vector_3_ns::vector_3 _fill;   // polygon middle point - average of all vectors
	vector_3_ns::vector_3 _normal; // polygon's direction
	vector_t _points;              // owning
	double _depth {ZERO};          // objects location on the z axis
	drawing* _draw_ctx {nullptr};  // raw pointer, non owning
	int _force {0};                // display polygon regardless of direction

	bool is_consec();
	bool is_degenerate();
	bool is_planar();
	bool verify();

	void draw(scene_ns::frame_context&);
	vector_3_ns::vector_3 find_fill();
	vector_3_ns::vector_3 find_normal();

public:

	typedef std::list<polygon*> polylist_t;
	using frame_context = scene_ns::frame_context;

	polygon(const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
	~polygon();

	bool read(const graphics_ns::graphics*, std::ifstream&);
	void print() const;
	void update(matrix_ns::matrix&, matrix_ns::matrix&, scene_ns::frame_context&);
	void sort(scene_ns::frame_context&);
	void draw_all(scene_ns::frame_context&);

	const std::string get_name() const { return _name; }
	double get_depth() const { return _depth; }

	void sort_polygons(scene_ns::frame_context&);
	void draw_polygons(scene_ns::frame_context&);
};

} // namespace polygon_ns
