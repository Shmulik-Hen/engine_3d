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
	frame_ctx.state->light.type = light_type::directional;
	frame_ctx.state->light.direction = {ZERO, ZERO, -UNIT};
	frame_ctx.state->light.position = {ZERO, ZERO, -ZLIMIT};
	frame_ctx.state->camera.position = {ZERO, ZERO, -ZLIMIT};

	frame_ctx.state->grfx.gfx = new graphics("Software 3D Engine");
	frame_ctx.state->grfx.clear_color = frame_ctx.state->grfx.gfx->get_color_val(graphics_ns::graphics::color_idx::black);
	frame_ctx.state->grfx.min_p = frame_ctx.state->grfx.gfx->get_min_position();
	frame_ctx.state->grfx.max_p = frame_ctx.state->grfx.gfx->get_max_position();
}

scene::~scene()
{
	clear();

	delete frame_ctx.state->grfx.gfx;
	frame_ctx.state->grfx.gfx = nullptr;

	delete frame_ctx.state;
	frame_ctx.state = nullptr;

	frame_ctx.draw_vec->clear();
	delete frame_ctx.draw_vec;
	frame_ctx.draw_vec = nullptr;
}

polygon* scene::add_polygon()
{
	polygons_owned.push_back(std::make_unique<polygon>(frame_ctx.state->grfx.min_p, frame_ctx.state->grfx.max_p));
	polygon* p = polygons_owned.back().get();
	poly_list.push_back(p);
	return p;
}

element* scene::add_element()
{
	elements_owned.push_back(std::make_unique<element>());
	return elements_owned.back().get();
}

polygon* scene::ensure_ctrl_polygon()
{
	if (!ctrl_poly) {
		polygons_owned.push_back(std::make_unique<polygon>(frame_ctx.state->grfx.min_p, frame_ctx.state->grfx.max_p));
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
