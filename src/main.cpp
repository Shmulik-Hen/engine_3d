#include <ios>
#include <string>

#include "common.h"
#include "attrib.h"
#include "element.h"
#include "matrix.h"
#include "my_vector.h"
#include "polygon.h"
#include "treenode.h"
#include "utils.h"

// #include <graphics.h>
// #include "graphics.h"
// #include <conio.h>
// #include <stdlib.h>
// #include <dos.h>

using attrib_ns::attrib;
using element_ns::element;
using matrix_ns::matrix;
using my_vector_ns::my_vector;
using polygon_ns::polygon;
using std::ios;
using std::string;
using namespace matrix_ns;

// typedef char palette[3];
// enum rgb
// {
// 	R,
// 	G,
// 	B
// };

// palette pal_16_colors[16];
// palette pal_256_colors[256];

// void init_palette();
// void get_palette(palette buff[], int colors);
// void set_palette(palette buff[], int colors);
// int huge DetectVGA256();

int main()
{
	char filename[] = "box.dat";
	ifstream f;
	LINE line;
	polygon* poly {nullptr};
	element* elem {nullptr};
	element* root {nullptr};
	attrib a(1, 1, 1, 0, 0, 0, 1024);
	matrix UNIT_MAT = get_unit_mat();
	bool rc;
	int ret = 0;

	polygon::poly_list* poly_lst = new polygon::poly_list;
	if (!poly_lst) {
		ret = 1;
		error("poly_list allocation error");
	}

	poly_lst->clear();

	element::elem_list* elem_lst = new element::elem_list;
	if (!elem_lst) {
		ret = 1;
		error("elem_lst allocation error");
	}

	elem_lst->clear();

	f.open(filename, ios::in);
	if (!f) {
		ret = 1;
		error("file not found:", filename);
	}

	f.unsetf(ios::skipws);

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
					poly_lst->push_front(*poly);
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
				rc = elem->read(poly_lst, root, f);
				if (rc) {
					elem_lst->push_front(*elem);
				}
				else {
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

	if (poly_lst->empty()) {
		error("no polygons");
	}

	if (elem_lst->empty()) {
		error("no elements");
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

	attrib att1(-32, 0, 0, 0, 0, 0, 2048);
	attrib att2(0, 3, 7, 0, 0, 0, 1024);
	e2->update(att1);

	// while (!kbhit()) {
	int i = 3;
	while (i--) {
		e2->update(att2);
		printf("update tree\n");
		fflush(stdout);
		root->update_all();
		printf("print tree\n");
		fflush(stdout);
		root->print_all();
		printf("sort\n");
		fflush(stdout);
		poly_lst->sort();
		// clearviewport();
		printf("walk list\n");
		fflush(stdout);
		for (polygon::pol_it it = poly_lst->begin(); it != poly_lst->end(); ++it) {
			poly = &*it;
			if (poly) {
				poly->print();
			}
		}
		// delay(17);
	}
	// closegraph();
	return ret;
}

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
