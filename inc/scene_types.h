#pragma once

#include <vector>

#include "unit.h"
#include "vector_3.h"

// clang-format off
namespace polygon_ns { class polygon; }
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
	light_type type {light_type::directional};

	// Convention: direction is "surface -> light", should be normalized
	vector_3_ns::vector_3 direction {ZERO, ZERO, -UNIT};

	// Used only if type == point
	vector_3_ns::vector_3 position {ZERO, ZERO, -ZLIMIT};
};

struct camera_state
{
	vector_3_ns::vector_3 position {ZERO, ZERO, -ZLIMIT};
};

struct scene_state
{
	light_state light;
	camera_state camera;
};

using drawvec_t = std::vector<polygon_ns::polygon*>;

struct frame_context
{
	scene_state const* state {nullptr};
	drawvec_t* draw_vec {nullptr};
};

} // namespace scene_ns
