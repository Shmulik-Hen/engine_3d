// #define DEBUG_PRINTS
#include "common.h"
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
	frame_ctx.state = new scene_state;
	frame_ctx.state->grfx.gfx = new graphics("Software 3D Engine");

	/* All static for NOW - later read from config or dynamic */

	// drawing vector
	frame_ctx.draw_vec->reserve(256);

	// graphics
	frame_ctx.state->grfx.clear_color = frame_ctx.state->grfx.gfx->get_color_val(graphics_ns::graphics::color_idx::black);
	DBG("clear color: " << HEX(frame_ctx.state->grfx.clear_color.c, 8));

	// light
	frame_ctx.state->light.type = light_type::directional;
	frame_ctx.state->light.direction = {ZERO, ZERO, -UNIT};
	frame_ctx.state->light.position = {320.0f, 320.0f, -500.0f};
	DBG("light type: " << (int)frame_ctx.state->light.type);
	DBG("light direction:");
	frame_ctx.state->light.direction.print();
	DBG("light position:");
	frame_ctx.state->light.position.print();

	// camera
	frame_ctx.state->camera.position = {ZERO, ZERO, -ZLIMIT};
	DBG("camera position:");
	frame_ctx.state->camera.position.print();

	// projection
	frame_ctx.state->proj.focal_len = ZLIMIT * 1.5f;
	frame_ctx.state->proj.near_eps = UNIT;
	DBG("projection: focal len: " << FLT(frame_ctx.state->proj.focal_len, 3) << ", near_eps: " << FLT(frame_ctx.state->proj.near_eps, 3));

	// viewport
	frame_ctx.state->vp.min_pos = frame_ctx.state->grfx.gfx->get_min_position();
	frame_ctx.state->vp.max_pos = frame_ctx.state->grfx.gfx->get_max_position();
	frame_ctx.state->vp.vp_min_pos = frame_ctx.state->vp.min_pos;
	frame_ctx.state->vp.vp_max_pos = frame_ctx.state->vp.max_pos;
	frame_ctx.state->vp.vp_mid_pos.x = frame_ctx.state->vp.vp_min_pos.x + (frame_ctx.state->vp.vp_max_pos.x - frame_ctx.state->vp.vp_min_pos.x) / 2;
	frame_ctx.state->vp.vp_mid_pos.y = frame_ctx.state->vp.vp_min_pos.y + (frame_ctx.state->vp.vp_max_pos.y - frame_ctx.state->vp.vp_min_pos.y) / 2;

	DBG("min_pos: {" << DEC(frame_ctx.state->vp.min_pos.x, 4) << SEP << DEC(frame_ctx.state->vp.min_pos.y, 4) << "}");
	DBG("max_pos: {" << DEC(frame_ctx.state->vp.max_pos.x, 4) << SEP << DEC(frame_ctx.state->vp.max_pos.y, 4) << "}");
	DBG("vp_min_pos: {" << DEC(frame_ctx.state->vp.vp_min_pos.x, 4) << SEP << DEC(frame_ctx.state->vp.vp_min_pos.y, 4) << "}");
	DBG("vp_mid_pos: {" << DEC(frame_ctx.state->vp.vp_mid_pos.x, 4) << SEP << DEC(frame_ctx.state->vp.vp_mid_pos.y, 4) << "}");
	DBG("vp_max_pos: {" << DEC(frame_ctx.state->vp.vp_max_pos.x, 4) << SEP << DEC(frame_ctx.state->vp.vp_max_pos.y, 4) << "}");
}

scene::~scene()
{
	ctrl_poly = nullptr;
	root = nullptr;
	poly_list.clear();
	polygons_owned.clear();
	elements_owned.clear();

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
	polygons_owned.push_back(std::make_unique<polygon>());
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
		polygons_owned.push_back(std::make_unique<polygon>());
		ctrl_poly = polygons_owned.back().get();
	}

	return ctrl_poly;
}

} // namespace scene_ns
