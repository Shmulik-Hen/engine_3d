#include <algorithm>
#include <chrono>
#include <thread>

// #define DEBUG_PRINTS
#include "utils.h"
#include "scene.h"
#include "parse_json.h"
#include "validate.h"

namespace scene_ns
{

using namespace element_ns;
using namespace graphics_ns;
using namespace polygon_ns;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using polygon = polygon_ns::polygon;

bool scene_ns::my_scene::_keep_going = true;

my_scene::my_scene()
{
	frame_ctx.draw_vec = std::make_unique<drawvec_t>();
	frame_ctx.state = std::make_unique<scene_state>();
	frame_ctx.state->grfx.gfx = std::make_unique<graphics>("Software 3D Engine");
	set_defaults();
}

polygon* my_scene::add_polygon()
{
	polygons_owned.push_back(std::make_unique<polygon>());
	polygon* p = polygons_owned.back().get();
	poly_list.push_back(p);
	return p;
}

element* my_scene::add_element()
{
	elements_owned.push_back(std::make_unique<element>());
	return elements_owned.back().get();
}

bool my_scene::parse(std::ifstream& ifs)
{
	return config_ns::parse_json(ifs, ast);
}

bool my_scene::validate()
{
	return config_ns::validate_ast(ast);
}

config_ns::scene_def* my_scene::find_scene(const std::string& name)
{
	for (auto& scn : ast.scene_defs) {
		if (scn.name == name) {
			return &scn;
		}
	}

	return nullptr;
}

void my_scene::build(const config_ns::scene_def& scene_def)
{
	DBG("build" << PFX << scene_def.name);
	// DBG("build: num polygon defs: " << (int)ast.library_defs.polygons.size());
	// for (const auto& def : ast.library_defs.polygons) {
	// 	polygon* poly = add_polygon();
	// 	poly->init_from_def(frame_ctx, def);
	// 	DBG("build: polygon:");
	// 	poly->print();
	// }

	// DBG("build: num element defs: " << (int)ast.library_defs.elements.size());
	// for (const auto& def : ast.library_defs.lib_elem_defs) {
	// 	element* elem = add_element();
	// 	if (!root) {
	// 		root = elem;
	// 	}
	// 	elem->init_from_def(poly_list, root, def);
	// 	DBG("build: element:");
	// 	elem->print();
	// }

	DBG("build: environment:");

	if (scene_def.env.has_value()) {
		config_ns::environ_def env = scene_def.env.value();

		if (env.resolution.has_value()) {
			DBG("build: override: resolution");

			config_ns::resolution_def res = env.resolution.value();
			bool res_changed = false;

			if ((res.width - 1) != max_pos.x) {
				frame_ctx.state->win.max_pos.x = res.width - 1;
				DBG(" build:override : max_pos.x : " << DEC(frame_ctx.state->win.max_pos.x, 4));
				res_changed = true;
			}

			if ((res.height - 1) != max_pos.y) {
				frame_ctx.state->win.max_pos.y = res.height - 1;
				DBG(" build:override : max_pos.y : " << DEC(frame_ctx.state->win.max_pos.y, 4));
				res_changed = true;
			}

			if (res_changed) {
				frame_ctx.state->win.mid_pos.x = frame_ctx.state->win.max_pos.x / 2;
				DBG("build: override: mid_pos.x: " << DEC(frame_ctx.state->win.mid_pos.x, 4));
				frame_ctx.state->win.mid_pos.y = frame_ctx.state->win.max_pos.y / 2;
				DBG("build: override: mid_pos.y: " << DEC(frame_ctx.state->win.mid_pos.y, 4));
			}
		}

		if (env.color_idx.has_value()) {
			DBG("build: override: color_idx");
			// clang-fotmat off
			frame_ctx.state->grfx.clear_color = frame_ctx.state->grfx.gfx->get_color_val(
				static_cast<graphics_ns::graphics::color_idx>(env.color_idx.value()));
			// clang=format on
		}

		if (env.light_type.has_value()) {
			DBG("build: override: light_type");
			// clang-fotmat off
			frame_ctx.state->light.type =
				env.light_type.value() ?
					config_ns::light_type::point :
					config_ns::light_type::directional;
			// clang=format on
		}

		if (env.light_direction.has_value()) {
			DBG("build: override: light_direction");
			frame_ctx.state->light.direction = env.light_direction.value();
			frame_ctx.state->light.direction.print();
		}

		if (env.light_position.has_value()) {
			DBG("build: override: light_position");
			frame_ctx.state->light.position = env.light_position.value();
			frame_ctx.state->light.position.print();
		}

		if (env.camera_position.has_value()) {
			DBG("build: override: camera_position");
			frame_ctx.state->camera.position = env.camera_position.value();
			frame_ctx.state->camera.position.print();
		}

		if (env.focal_len.has_value()) {
			DBG("build: override: focal_len");
			frame_ctx.state->proj.focal_len = env.focal_len.value();
		}

		if (env.near_eps.has_value()) {
			DBG("build: override: near_eps");
			frame_ctx.state->proj.near_eps = env.near_eps.value();
		}
	}

	if (scene_def.run.has_value()) {
		config_ns::runtime_def run = scene_def.run.value();
		if (run.loops.has_value()) {
			DBG("build: override: 1: loops: " << frame_ctx.state->run.loops);
			frame_ctx.state->run.loops = run.loops.value();
			DBG("build: override: 2: loops: " << frame_ctx.state->run.loops);
		}

		if (run.loop_delay.has_value()) {
			DBG("build: override: 1: loop_delay: " << frame_ctx.state->run.loop_delay);
			frame_ctx.state->run.loop_delay = run.loop_delay.value();
			DBG("build: override: 2: loop_delay: " << frame_ctx.state->run.loop_delay);
		}
	}
}

void my_scene::update()
{
	// draw list is per-frame (non-owning)
	frame_ctx.draw_vec->clear();

	// update tree + polygons; element/polygon code appends to draw_vec
	root->update_all(frame_ctx);

#ifdef DEBUG_PRINTS
	// DBG("print tree");
	// root->print_all();
#endif
}

void my_scene::sort()
{
	// clang-format off
	std::sort(frame_ctx.draw_vec->begin(), frame_ctx.draw_vec->end(),
		  [](polygon* a, polygon* b) {
			return a->get_depth() > b->get_depth();
		  });
	// clang-format on
}

void my_scene::draw()
{
	for (polygon* poly : *frame_ctx.draw_vec) {
		if (poly) {
			poly->draw(frame_ctx);
		}
	}
}

void my_scene::render()
{
	static int loops = frame_ctx.state->run.loops;

	graphics* gfx = frame_ctx.state->grfx.gfx.get();

	// Acquire backbuffer for this frame and clear it
	frame_ctx.state->grfx.fb = gfx->get_backbuffer();
	gfx->fill_buffer(frame_ctx.state->grfx.fb, frame_ctx.state->grfx.clear_color);

	if (!frame_ctx.draw_vec->empty()) {
		// DBG("sort");
		sort();
	}

	// DBG("draw");
	draw();

	// DBG("show");
	gfx->present();

	std::this_thread::sleep_for(std::chrono::milliseconds(frame_ctx.state->run.loop_delay));

	if (frame_ctx.state->run.loops > 0) {
		if (--loops < 1) {
			_keep_going = false;
		}
	}
}

bool my_scene::keep_going()
{
	return _keep_going;
}

void my_scene::set_defaults()
{
	DBG("set_default");

	// drawing vector
	frame_ctx.draw_vec->reserve(256);

	// graphics
	frame_ctx.state->grfx.clear_color = frame_ctx.state->grfx.gfx->get_color_val(graphics_ns::graphics::color_idx::black);
	DBG("clear color: " << HEX(frame_ctx.state->grfx.clear_color.c, 8));
	min_pos = frame_ctx.state->grfx.gfx->get_min_position();
	max_pos = frame_ctx.state->grfx.gfx->get_max_position();

	// light
	frame_ctx.state->light.type = config_ns::light_type::directional;
	frame_ctx.state->light.direction = {DEF_LIGHT_DIR_X, DEF_LIGHT_DIR_Y, DEF_LIGHT_DIR_Z};
	frame_ctx.state->light.position = {DEF_LIGHT_POS_X, DEF_LIGHT_POS_Y, DEF_LIGHT_POS_Z};
	DBG("light type: " << (int)frame_ctx.state->light.type);
	DBG("light direction:");
	frame_ctx.state->light.direction.print();
	DBG("light position:");
	frame_ctx.state->light.position.print();

	// camera
	frame_ctx.state->camera.position = {DEF_CAM_POS_X, DEF_CAM_POS_Y, DEF_CAM_POS_Z};
	DBG("camera position:");
	frame_ctx.state->camera.position.print();

	// projection
	frame_ctx.state->proj.focal_len = DEF_FOCAL_LEN;
	frame_ctx.state->proj.near_eps = DEF_NEAR_EPS;
	DBG("projection: focal len: " << FLT(frame_ctx.state->proj.focal_len, 3) << ", near_eps: " << FLT(frame_ctx.state->proj.near_eps, 3));

	// screen
	frame_ctx.state->win.min_pos = min_pos;
	frame_ctx.state->win.max_pos = max_pos;
	frame_ctx.state->win.mid_pos.x = max_pos.x / 2;
	frame_ctx.state->win.mid_pos.y = max_pos.y / 2;

	DBG("min_pos: {" << DEC(frame_ctx.state->win.min_pos.x, 4) << SEP << DEC(frame_ctx.state->win.min_pos.y, 4) << "}");
	DBG("max_pos: {" << DEC(frame_ctx.state->win.max_pos.x, 4) << SEP << DEC(frame_ctx.state->win.max_pos.y, 4) << "}");
	DBG("mid_pos: {" << DEC(frame_ctx.state->win.mid_pos.x, 4) << SEP << DEC(frame_ctx.state->win.mid_pos.y, 4) << "}");

	frame_ctx.state->run.loops = DEF_LOOP_NUM;
	frame_ctx.state->run.loop_delay = DEF_LOOP_DELAY;

	// limits for validations
	ast.valids.max_color_idx = frame_ctx.state->grfx.gfx->get_num_colors();
}

} // namespace scene_ns
