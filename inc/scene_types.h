#pragma once

#include <vector>
#include <memory>

#include "graphics.h"
#include "unit.h"
#include "vector_3.h"

// clang-format off
namespace polygon_ns { class polygon; }
namespace graphics_ns { class graphics; }
// clang-format on

namespace scene_ns
{

struct graphics_state
{
	std::unique_ptr<graphics_ns::graphics> gfx {nullptr};
	graphics_ns::graphics::frame_buffer fb;
	graphics_ns::graphics::ARGB clear_color {};
};

enum class light_type
{
	directional = 0,
	point
};

struct light_state
{
	light_type type;

	// Convention: direction is "surface -> light", should be normalized
	vector_3_ns::vector_3 direction;

	// Used only if type == point
	vector_3_ns::vector_3 position;
};

struct camera_state
{
	vector_3_ns::vector_3 position;
};

struct projection_state
{
	// distance from camera to projection plane (in world units)
	unit focal_len; // default preserves old look

	// optional safety clamps (can be ignored initially)
	unit near_eps; // avoid divide by ~0
};

struct viewport_state
{
	// screen min/max points
	graphics_ns::graphics::point min_pos;
	graphics_ns::graphics::point max_pos;
	// view port min/mid/max points - can be screen size or less
	graphics_ns::graphics::point vp_min_pos;
	graphics_ns::graphics::point vp_mid_pos;
	graphics_ns::graphics::point vp_max_pos;
};

struct scene_state
{
	graphics_state grfx;
	light_state light;
	camera_state camera;
	projection_state proj;
	viewport_state vp;
};

using drawvec_t = std::vector<polygon_ns::polygon*>;

struct frame_context
{
	std::unique_ptr<drawvec_t> draw_vec {nullptr};
	std::unique_ptr<scene_state> state {nullptr};
};

} // namespace scene_ns
