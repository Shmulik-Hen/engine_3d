#include <algorithm>

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
	frame_ctx.draw_vec = std::make_unique<drawvec_t>();
	frame_ctx.state = std::make_unique<scene_state>();
	frame_ctx.state->grfx.gfx = std::make_unique<graphics>("Software 3D Engine");

	/* All static for NOW - later read from config or dynamic */

	// drawing vector
	frame_ctx.draw_vec->reserve(256);

	// graphics
	frame_ctx.state->grfx.clear_color = frame_ctx.state->grfx.gfx->get_color_val(graphics_ns::graphics::color_idx::black);
	DBG("clear color: " << HEX(frame_ctx.state->grfx.clear_color.c, 8));

	// light
	frame_ctx.state->light.type = light_type::point;
	frame_ctx.state->light.direction = {ZERO, ZERO, -UNIT};
	frame_ctx.state->light.position = {ZERO, LIGHT_ELEV, LIGHT_DIST};
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
	root = nullptr;
	poly_list.clear();
	polygons_owned.clear();
	elements_owned.clear();
	frame_ctx.draw_vec->clear();
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

void scene::update()
{
	if (!root || !frame_ctx.draw_vec) {
		return;
	}

	// draw list is per-frame (non-owning)
	frame_ctx.draw_vec->clear();

	// update tree + polygons; element/polygon code appends to draw_vec
	root->update_all(frame_ctx);

#ifdef DEBUG_PRINTS
	DBG("print tree");
	root->print_all();
#endif
}

void scene::render()
{
	if (!frame_ctx.state ||
	    !frame_ctx.state->grfx.gfx ||
	    !frame_ctx.draw_vec ||
	    frame_ctx.draw_vec->empty()) {
		return;
	}

	graphics* gfx = frame_ctx.state->grfx.gfx.get();

	// Acquire backbuffer for this frame and clear it
	frame_ctx.state->grfx.fb = gfx->get_backbuffer();
	gfx->fill_buffer(frame_ctx.state->grfx.fb, frame_ctx.state->grfx.clear_color);

	DBG("sort");
	sort();
	DBG("draw");
	draw();
	DBG("show");
	gfx->present();
}

void scene::sort()
{
	if (frame_ctx.draw_vec->size() > 1) {
		// clang-format off
		std::sort(frame_ctx.draw_vec->begin(), frame_ctx.draw_vec->end(),
			  [](polygon* a, polygon* b) {
				return a->get_depth() < b->get_depth();
			  });
		// clang-format on
	}
}

void scene::draw()
{
	if (frame_ctx.draw_vec->empty()) {
		DBG("draw: empty");
		return;
	}

	for (polygon* poly : *frame_ctx.draw_vec) {
		if (poly) {
			poly->draw(frame_ctx);
		}
	}
}

} // namespace scene_ns
