#pragma once

#include <list>
#include <memory>

#include "element.h"
#include "graphics.h"
#include "polygon.h"
#include "scene_types.h"
#include "config_document.h"
#include "config_legacy.h"

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

	frame_context frame_ctx;

	config_ns::document doc;

	scene();
	scene(const scene&) = delete;
	scene& operator=(const scene&) = delete;
	scene(scene&&) = delete;
	scene& operator=(scene&&) = delete;
	~scene();

	polygon_ns::polygon* add_polygon();
	element_ns::element* add_element();
	void parse();
	void build();
	void update();
	void render();

private:

	void sort();
	void draw();
};

} // namespace scene_ns
