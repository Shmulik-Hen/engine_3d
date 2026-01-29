#pragma once

#include <vector>

#include "unit.h"
#include "vector_3.h"

// clang-format off
namespace polygon_ns { class polygon; }
// clang-format on
using polygon = polygon_ns::polygon;

namespace scene_ns
{

typedef std::vector<polygon*> drawvec_t;

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

struct frame_context
{
	drawvec_t draw_vec;
	light_state light_st;
	camera_state camera_st;
};

} // namespace scene_ns
