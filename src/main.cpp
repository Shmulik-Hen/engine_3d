#include <chrono>
#include <thread>

#define DEBUG_PRINTS
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
using namespace matrix_ns;
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
		polygon* ctrl_poly {nullptr};
		polygon* poly {nullptr};
		polygon::poly_list poly_lst;
		input_state in {};
		bool rc;

		graphics gfx("Software 3D Engine", 320, 200, 2);
		ctrl_poly = new polygon(gfx);
		if (!ctrl_poly) {
			sys_error("ctrl_poly is null");
		}

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
				poly = new polygon(gfx);
				if (poly) {
					rc = poly->read(f);
					if (rc) {
						poly_lst.push_back(poly);
					}
					else {
						sys_error("read polygon failed");
					}
				}
				else {
					sys_error("new polygon failed");
				}
				break;
			case 'e':
				elem = new element;
				if (elem) {
					rc = elem->read(poly_lst, &root, f);
					if (!rc) {
						sys_error("read element failed");
					}
				}
				else {
					sys_error("new element failed");
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

		string* s1 = new string("world");
		if (!s1) {
			sys_error("string allocation error");
		}

		string* s2 = new string("box");
		if (!s2) {
			sys_error("string allocation error");
		}

		element* e1 = root->find(root, *s1);
		if (!e1) {
			sys_error("find world failed");
		}

		element* e2 = root->find(root, *s2);
		if (!e2) {
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

		// attrib att1(-32, 0, 0, 0, 0, 0, 2048);
		attrib att2(0, 0, 0, 0, 0, 0, 1024);
		// e2->update(att1);

		while (!in.quit) {
			gfx.poll_events(in);
			DBG("in.quit: " << in.quit << " in.esc: " << in.key_escape);

			e2->update(att2);
			DBG("update tree");
			root->update_all();
			// DBG("print tree");
			// root->print_all();
			// DBG("sort");
			// ctrl_poly->sort();
			ctrl_poly->show_all();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return 0;
		}

		return 0;
	}
	catch (const std::exception& e) {
		// In a commercial-style app, you’d log to stderr + maybe a file
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
