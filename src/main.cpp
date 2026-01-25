#include <chrono>
#include <thread>

// #define DEBUG_PRINTS
#include "common.h"
#include "attrib.h"
#include "element.h"
#include "graphics.h"
#include "matrix.h"
#include "polygon.h"
#include "treenode.h"
#include "utils.h"

using namespace attrib_ns;
using namespace element_ns;
using namespace graphics_ns;
using namespace matrix_ns;
using namespace polygon_ns;
using input_state = graphics_ns::graphics::input_state;

int main()
{
	try {
		char filename[] = "box.dat";
		std::ifstream ifs;
		LINE line;
		element* elem {nullptr};
		element* root {nullptr};
		polygon* poly {nullptr};
		polygon* ctrl_poly {nullptr};
		polygon::poly_list poly_lst;
		input_state in {};
		bool rc;

		graphics gfx("Software 3D Engine", 320, 200, 2);

		ifs.open(filename, std::ios::in);
		if (!ifs) {
			sys_error("file not found:", filename);
		}

		ifs.unsetf(std::ios::skipws);

		poly_lst.clear();

		while (!ifs.eof()) {
			rc = true;
			while ((!read_word(ifs, line)) && (!ifs.eof()));

			if (ifs.eof()) {
				break;
			}

			switch (line[1]) {
			// case '#':
			// 	read_remark(ifs);
			case 'p':
				// instantiate a new polygon
				poly = new polygon(gfx);
				rc = poly->read(ifs);
				if (!rc) {
					delete poly;
					sys_error("read polygon failed");
				}

				poly_lst.push_back(poly);

				if (!ctrl_poly) {
					ctrl_poly = new polygon(gfx);
				}
				break;
			case 'e':
				// instantiate a new element
				elem = new element;
				rc = elem->read(poly_lst, &root, ifs);
				if (!rc) {
					delete elem;
					sys_error("read element failed");
				}
				break;
			}
		}

		ifs.close();

		if (!root) {
			sys_error("root is null");
		}

		if (poly_lst.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		std::string* world_name = new std::string("world");
		std::string* box_name = new std::string("box");

		element* world = root->find(root, *world_name);
		if (!world) {
			sys_error("find world failed");
		}

		element* box = root->find(root, *box_name);
		if (!box) {
			sys_error("find box failed");
		}

#ifdef DEBUG_PRINTS
		DBG("initial:");
		DBG("print tree");
		root->print_all();
		DBG("walk list");
		for (const auto poly : poly_lst) {
			poly->print();
		}
#endif // DEBUG_PRINTS

		// attrib(rotationX, rotationY, rotationZ, positionX, positionY, positionZ, zoom)
		attrib initial_att(0, 0, 0, 0, 0, 0, 1);
		attrib keep_moving(1, 2, 3, 0, 0, 0, 1);

		box->update(initial_att);

		// int i = 3;
		// while (!in.quit && i--) {
		while (!in.quit) {
			gfx.poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);

			DBG("update tree");
			root->update_all();
#ifdef DEBUG_PRINTS
			DBG("print tree");
			root->print_all();
#endif
			DBG("sort");
			ctrl_poly->sort_polygons();
			DBG("show");
			ctrl_poly->show_polygons();
			std::this_thread::sleep_for(std::chrono::milliseconds(15));
			box->update(keep_moving);
		}

		return 0;
	}
	catch (const std::exception& e) {
		// In a commercial-style app, you’d log to stderr + maybe a file
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
