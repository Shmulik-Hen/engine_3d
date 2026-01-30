#include "scene.h"

namespace scene_ns
{

using namespace element_ns;
using namespace graphics_ns;
using namespace polygon_ns;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using polygon = polygon_ns::polygon;

scene::scene()
{
	frame_ctx.draw_vec = new drawvec_t;
	frame_ctx.draw_vec->reserve(256);

	frame_ctx.state = new scene_state;
}

scene::~scene()
{
	delete frame_ctx.state;
	frame_ctx.state = nullptr;
	frame_ctx.draw_vec->clear();
	delete frame_ctx.draw_vec;
	frame_ctx.draw_vec = nullptr;
}

polygon* scene::add_polygon(graphics& gfx)
{
	polygons_owned.push_back(std::make_unique<polygon>(gfx));
	polygon* p = polygons_owned.back().get();
	poly_list.push_back(p); // keep your existing list-of-raw-pointers
	return p;
}

element* scene::add_element()
{
	elements_owned.push_back(std::make_unique<element>());
	return elements_owned.back().get();
}

polygon* scene::ensure_ctrl_polygon(graphics& gfx)
{
	if (!ctrl_poly) {
		polygons_owned.push_back(std::make_unique<polygon>(gfx));
		ctrl_poly = polygons_owned.back().get();
	}
	return ctrl_poly;
}

void scene::clear()
{
	ctrl_poly = nullptr;
	root = nullptr;
	poly_list.clear();
	polygons_owned.clear();
	elements_owned.clear();
}

} // namespace scene_ns
