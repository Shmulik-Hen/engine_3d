#include <chrono>
#include <thread>

// #define DEBUG_PRINTS
#include "common.h"
#include "scene.h"
#include "utils.h"

using namespace attrib_ns;
using namespace element_ns;
using namespace graphics_ns;
using namespace scene_ns;
using namespace config_ns;
using attrib = attrib_ns::attrib;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using input_state = graphics_ns::graphics::input_state;

int main()
{
	try {
		scene scn; // executes parse() + build() - must be first
		input_state in {};
		graphics* gfx = scn.frame_ctx.state->grfx.gfx.get();

		if (!scn.root) {
			sys_error("root is null");
		}

		if (scn.poly_list.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		const std::string world_name = "world";
		const std::string box_name = "box";

		element* world = scn.root->find(scn.root, world_name);
		if (!world) {
			sys_error("find world failed");
		}

		element* box = scn.root->find(scn.root, box_name);
		if (!box) {
			sys_error("find box failed");
		}

#ifdef DEBUG_PRINTS
		DBG("initial:");
		DBG("print tree");
		scn.root->print_all();
		DBG("walk list");
		for (const auto poly : scn.poly_list) {
			poly->print();
		}
#endif // DEBUG_PRINTS

		// attrib(rotationX, rotationY, rotationZ, positionX, positionY, positionZ, zoom)
		attrib initial_att(45, 0, 45, 0, 0, 0, 1);
		attrib keep_moving(0, 1, 0, 0, 0, 0, 1);

		world->update(initial_att);

		while (!in.quit) {
			gfx->poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);

			scn.update();
			scn.render();

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			box->update(keep_moving);
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
