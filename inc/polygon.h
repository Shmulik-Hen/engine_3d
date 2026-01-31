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
		graphics_ns::graphics::point _bbox_tl;
		graphics_ns::graphics::point _bbox_br;
		graphics_ns::graphics::ARGB _base_color {};
		graphics_ns::graphics::ARGB _draw_color {};
		matrix_ns::matrix _trans_mat;
		matrix_ns::matrix _rot_mat;

#ifdef DEBUG_POLYGON
		vector_3_ns::vector_3 _debug_normal;
		vector_3_ns::vector_3 _debug_fill;
		graphics_ns::graphics::color_idx _debug_color {graphics_ns::graphics::white};
		graphics_ns::graphics::color_idx _bbox_color {graphics_ns::graphics::green};
		graphics_ns::graphics::color_idx _normal_color {graphics_ns::graphics::lime};
		graphics_ns::graphics::color_idx _fill_color {graphics_ns::graphics::red};
		graphics_ns::graphics::point _normal_point;
		graphics_ns::graphics::point _fill_point;
#endif

		drawing() = default;
		~drawing() = default;

		graphics_ns::graphics::addr_t offset(scene_ns::frame_context&, const graphics_ns::graphics::point&) const;
		graphics_ns::color_t get_color() const { return _base_color.c; }
		graphics_ns::graphics::ARGB getdirect() const { return *_xy_addr; }
		graphics_ns::graphics::ARGB getpixel(scene_ns::frame_context&, const graphics_ns::graphics::point&);
		graphics_ns::graphics::frame_buffer clear();
		void adjust_min(scene_ns::frame_context&, graphics_ns::graphics::point&);
		void adjust_max(scene_ns::frame_context&, graphics_ns::graphics::point&);
		void create_bbox(scene_ns::frame_context&);
		void putdirect(const graphics_ns::graphics::ARGB& a) { *_xy_addr = a; }
		void putpixel(scene_ns::frame_context&, const graphics_ns::graphics::point&, graphics_ns::graphics::ARGB&);
		void moveto(scene_ns::frame_context&, const graphics_ns::graphics::point&);
		void lineto(scene_ns::frame_context&, const graphics_ns::graphics::point&);
		void stroke_to(scene_ns::frame_context&, const graphics_ns::graphics::point&);
		void bar(graphics_ns::val_t, graphics_ns::val_t);
		void line(scene_ns::frame_context&, const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void rect(scene_ns::frame_context&, const graphics_ns::graphics::point&, const graphics_ns::graphics::point&);
		void set_color(const graphics_ns::graphics*, int);
		void make_color(unit);
		void clear_scratch_pad();
		void plot(scene_ns::frame_context&);
		void fill(scene_ns::frame_context&);
		void project(const vector_3_ns::vector_3&, scene_ns::frame_context&);
		void draw(scene_ns::frame_context&);
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

	polygon();
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
