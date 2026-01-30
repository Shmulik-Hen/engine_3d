#include <chrono>
#include <thread>

// #define DEBUG_PRINTS
#include "common.h"
#include "scene.h"
#include "utils.h"

using namespace attrib_ns;
using namespace element_ns;
using namespace graphics_ns;
using namespace polygon_ns;
using namespace scene_ns;
using attrib = attrib_ns::attrib;
using element = element_ns::element;
using graphics = graphics_ns::graphics;
using input_state = graphics_ns::graphics::input_state;
using polygon = polygon_ns::polygon;

int main()
{
	try {
		char filename[] = "box.dat";
		std::ifstream ifs;
		LINE line;
		bool rc;
		polygon* poly;
		element* elem;
		scene scn;

		input_state in {};
		graphics gfx("Software 3D Engine", 320, 200, 2);

		ifs.open(filename, std::ios::in);
		if (!ifs) {
			sys_error("file not found:", filename);
		}

		ifs.unsetf(std::ios::skipws);

		while (!ifs.eof()) {
			while ((!read_word(ifs, line)) && (!ifs.eof()));
			if (ifs.eof()) {
				break;
			}

			switch (line[1]) {
			// case '#':
			// 	read_remark(ifs);
			case 'p':
				poly = scn.add_polygon(gfx);
				rc = poly->read(ifs);
				if (!rc) {
					sys_error("read polygon failed");
				}
				scn.ensure_ctrl_polygon(gfx); // keep behavior
				break;
			case 'e':
				elem = scn.add_element();
				if (!scn.root) {
					scn.root = elem;
				}
				rc = elem->read(scn.poly_list, scn.root, ifs);
				if (!rc) {
					sys_error("read element failed");
				}
				break;
			}
		}

		ifs.close();

		if (!scn.root) {
			sys_error("root is null");
		}

		if (scn.poly_list.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		// No heap strings
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

		// int i = 3;
		// while (!in.quit && i--) {
		while (!in.quit) {
			gfx.poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);
			scn.frame_ctx.draw_vec->clear();

			DBG("update tree");
			scn.root->update_all(scn.frame_ctx);
#ifdef DEBUG_PRINTS
			DBG("print tree");
			scn.root->print_all();
#endif
			DBG("sort");
			scn.ctrl_poly->sort_polygons(scn.frame_ctx);
			DBG("show");
			scn.ctrl_poly->show_polygons(scn.frame_ctx);
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
			box->update(keep_moving);
		}

		return 0;
	}
	catch (const std::exception& e) {
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
