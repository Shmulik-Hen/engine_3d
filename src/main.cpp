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

using std::ios;
using std::string;
using input_state = graphics_ns::graphics::input_state;

int main()
{
	try {
		char filename[] = "box.dat";
		ifstream f;
		LINE line;
		element* elem {nullptr};
		element* root {nullptr};
		polygon* poly {nullptr};
		polygon* ctrl_poly {nullptr};
		polygon::poly_list poly_lst;
		input_state in {};
		bool rc;

		graphics gfx("Software 3D Engine", 320, 200, 2);

		f.open(filename, ios::in);
		if (!f) {
			sys_error("file not found:", filename);
		}

		f.unsetf(ios::skipws);

		poly_lst.clear();

		while (!f.eof()) {
			rc = true;
			while ((!read_word(f, line)) && (!f.eof()));

			if (f.eof()) {
				break;
			}

			switch (line[1]) {
			// case '#':
			// 	read_remark(f);
			case 'p':
				// instantiate a new polygon
				poly = new polygon(gfx);
				rc = poly->read(f);
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
				rc = elem->read(poly_lst, &root, f);
				if (!rc) {
					delete elem;
					sys_error("read element failed");
				}
				break;
			}
		}

		f.close();

		if (!root) {
			sys_error("root is null");
		}

		if (poly_lst.empty()) {
			sys_error("no polygons");
		}

		// find the relevant elements to work with
		string* world_name = new string("world");
		string* box_name = new string("box");

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
		attrib initial_att(0, 0, 64, 0, 0, 0, 2048);
		box->update(initial_att);

		attrib keep_moving(0, 1, 0, 0, 0, 0, 1024);
		while (!in.quit) {
			gfx.poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);

			box->update(keep_moving);
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
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		return 0;
	}
	catch (const std::exception& e) {
		// In a commercial-style app, you’d log to stderr + maybe a file
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
