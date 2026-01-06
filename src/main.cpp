#if 1
#include "graphics_sdl2.h"

using graphics_ns::graphics;
using input_state = graphics_ns::graphics::input_state;
using frame_buffer = graphics_ns::graphics::frame_buffer;
using std::string;

int main()
{
	try {
		graphics platform("Software 3D Engine", 320, 200, 2);

		// Engine engine(...);

		platform.run(
			/* update */
			[&](double dt [[maybe_unused]], const input_state& in) {
				(void)in;
				// engine.update(dt);
			},

			/* render */
			[&](frame_buffer& fb) {
				// engine.render(fb.pixels, fb.width, fb.height, fb.pitchBytes);
			        // OR: engine.render(fb);
			        // For now, a simple test pattern:
				std::fill(fb.pixels, fb.pixels + fb.width * fb.height, 0xFFa0a0a0u);
			});

		return 0;
	}
	catch (const std::exception& e) {
		// In a commercial-style app, you’d log to stderr + maybe a file
		std::fprintf(stderr, "Fatal: %s\n", e.what());
		return 1;
	}
}
#else
#include "common.h"
#include "attrib.h"
#include "element.h"
#include "matrix.h"
#include "polygon.h"
#include "treenode.h"
#include "utils.h"

#if 0
#include <graphics.h>
#include "graphics.h"
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#endif // 0

using attrib_ns::attrib;
using element_ns::element;
using matrix_ns::matrix;
using polygon_ns::polygon;
using std::ios;
using std::string;
using namespace matrix_ns;

#if 0
typedef char palette[3];
enum rgb
{
	R,
	G,
	B
};

palette pal_16_colors[16];
palette pal_256_colors[256];

void init_palette();
void get_palette(palette buff[], int colors);
void set_palette(palette buff[], int colors);
int huge DetectVGA256();
#endif // 0

polygon ctrl_poly;

int main()
{
	char filename[] = "box.dat";
	ifstream f;
	LINE line;
	polygon* poly {nullptr};
	element* elem {nullptr};
	element* root {nullptr};
	polygon::poly_list poly_lst;
	bool rc;
	int ret = 0;

	f.open(filename, ios::in);
	if (!f) {
		ret = 1;
		error("file not found:", filename);
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
		case '#':
			read_remark(f);
		case 'p':
			poly = new polygon;
			if (poly) {
				rc = poly->read(f);
				if (rc) {
					poly_lst.push_back(poly);
				}
				else {
					ret = 1;
					error("read polygon failed");
				}
			}
			else {
				ret = 1;
				error("new polygon failed");
			}
			break;
		case 'e':
			elem = new element;
			if (elem) {
				rc = elem->read(poly_lst, &root, f);
				if (!rc) {
					ret = 1;
					error("read element failed");
				}
			}
			else {
				ret = 1;
				error("new element failed");
			}
			break;
		}

		if (!rc) {
			ret = 1;
			error("parsing error");
		}

		if (ret) {
			return ret;
		}
	}

	f.close();
#if 0
	int Gd = DETECT, Gm;
	installuserdriver("SVGA256", DetectVGA256);
	initgraph(&Gd, &Gm, "");
	init_palette();
	setviewport(1, 1, 319, 199, 1);
#endif // 0

	if (!root) {
		error("root is null");
	}

	if (poly_lst.empty()) {
		error("no polygons");
	}

	string* s1 = new string("world");
	if (!s1) {
		error("string allocation error");
	}

	string* s2 = new string("box");
	if (!s2) {
		error("string allocation error");
	}

	element* e1 = root->find(root, *s1);
	if (!e1) {
		error("find world failed");
	}

	element* e2 = root->find(root, *s2);
	if (!e2) {
		error("find box failed");
	}

#ifdef DEBUG_GRFX
	DBG("initial:");
	DBG("print tree");
	root->print_all();
	DBG("walk list");
	for (const auto poly : poly_lst) {
		poly->print();
	}
#endif // DEBUG_GRFX

	attrib att1(-32, 0, 0, 0, 0, 0, 2048);
	attrib att2(10, 10, 10, 0, 0, 0, 1024);
	e2->update(att1);

	// while (!kbhit()) {
	int i = 20;
	do {
		DBG(DEC(i, 4));
		e2->update(att2);
		DBG("update tree");
		root->update_all();
		DBG("print tree");
		root->print_all();
		DBG("sort");
		ctrl_poly.sort();
		ctrl_poly.show_all();
		// clearviewport();
	} while (--i);
	// closegraph();
	return ret;
}
#endif // 1

#if 0
void init_palette()
{
	int i, j;

	get_palette(pal_16_colors, 16);
	for (i = 0; i < 16; i++)
		for (j = 0; j < 16; j++) {
			pal_256_colors[j * 16 + i][R] = (char)(((int)(pal_16_colors[i][R]) * j) / 15);
			pal_256_colors[j * 16 + i][G] = (char)(((int)(pal_16_colors[i][G]) * j) / 15);
			pal_256_colors[j * 16 + i][B] = (char)(((int)(pal_16_colors[i][B]) * j) / 15);
		}
	set_palette(pal_256_colors, 256);
}

void get_palette(palette buff[], int colors)
{
	struct REGPACK reg;

	reg.r_ax = 0x1017;
	reg.r_bx = 0;
	reg.r_cx = colors;
	reg.r_dx = FP_OFF(buff);
	reg.r_es = FP_SEG(buff);
	intr(0x10, &reg);
}

void set_palette(palette buff[], int colors)
{
	struct REGPACK reg;

	reg.r_ax = 0x1012;
	reg.r_bx = 0;
	reg.r_cx = colors;
	reg.r_dx = FP_OFF(buff);
	reg.r_es = FP_SEG(buff);
	intr(0x10, &reg);
}

int huge DetectVGA256()
{
	return 0;
	/*
		 int Vid;

		 printf("Which video mode would you like to use? \n");
		 printf("  0) 320x200x256\n");
		 printf("  1) 640x400x256\n");
		 printf("  2) 640x480x256\n");
		 printf("  3) 800x600x256\n");
		 printf("  4) 1024x768x256\n\n>");
		 scanf("%d",&Vid);
		 return Vid;
	*/
}
#endif // 0
