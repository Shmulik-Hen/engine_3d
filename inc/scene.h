#pragma once

#include <list>
#include <memory>

#include "element.h"
#include "graphics.h"
#include "polygon.h"

// clang-format off
namespace element_ns { class element; }
namespace graphics_ns { class graphics; }
namespace polygon_ns { class polygon; }
// clang-format on

namespace scene_ns
{

class scene
{
public:

	// Own everything allocated during parsing/runtime
	std::list<std::unique_ptr<polygon_ns::polygon>> polygons_owned;
	std::list<std::unique_ptr<element_ns::element>> elements_owned;

	// Non-owning views
	polygon_ns::polygon::polylist_t poly_list;

	// Root of element tree (non-owning)
	element_ns::element* root {nullptr};

	// "control polygon" (owned, but referenced as raw pointer)
	polygon_ns::polygon* ctrl_poly {nullptr};

	scene() = default;
	scene(const scene&) = delete;
	scene& operator=(const scene&) = delete;
	scene(scene&&) = delete;
	scene& operator=(scene&&) = delete;

	polygon_ns::polygon* add_polygon(graphics_ns::graphics& gfx);
	element_ns::element* add_element();
	polygon_ns::polygon* ensure_ctrl_polygon(graphics_ns::graphics& gfx);

	void clear(); // optional convenience
};

} // namespace scene_ns
