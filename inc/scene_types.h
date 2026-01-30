#pragma once

#include <vector>

#include "graphics.h"
#include "unit.h"
#include "vector_3.h"

// clang-format off
namespace polygon_ns { class polygon; }
namespace graphics_ns { class graphics; }
// clang-format on

namespace scene_ns
{

enum class light_type
{
	directional,
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

struct graphics_state
{
	graphics_ns::graphics* gfx {nullptr};
	graphics_ns::graphics::frame_buffer fb;
	graphics_ns::graphics::ARGB clear_color {};
	graphics_ns::graphics::point min_p;
	graphics_ns::graphics::point max_p;
};

struct scene_state
{
	light_state light;
	camera_state camera;
	graphics_state grfx;
};

using drawvec_t = std::vector<polygon_ns::polygon*>;

struct frame_context
{
	scene_state* state {nullptr};
	drawvec_t* draw_vec {nullptr};
};

} // namespace scene_ns
