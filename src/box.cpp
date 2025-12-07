#include <ios>
#include "element.h"
#include "polyelem.h"
#include "polygon.h"
#include "utils.h"
#include "attrib.h"
#include "list.h"

// #include <graphics.h>
// #include "graphics.h"
// #include <conio.h>
// #include <stdlib.h>
// #include <dos.h>

using std::ios;

// extern list<polygon> poly_list;
extern matrix UNIT_MAT;
vector view(0, 0, -1000000);
vector n_light(0, 0, -1024);

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
// int huge DetectVGA256();

int main()
{
	char filename[] = "box.dat";
	ifstream f;
	LINE line;
	polygon *poly;
	element *root = NULL, *elem1, *elem2;
	polyelem *pe = new polyelem;
	attrib a(1, 1, 1, 0, 0, 0, 1024);

	f.open(filename, ios::in);
	if (!f)
		error("file not found:", filename);

	f.unsetf(ios::skipws);

	printf("start parsing\n");
	while (!f.eof()) {
		printf("box: \n");
		while ((!read_word(f, line)) && (!f.eof()))
			;
		switch (line[1]) {
		case '#':
			printf("box: #: \n");
			read_remark(f);
		case 'p':
			printf("box: p: \n");
			poly = new polygon;
			poly->read(f);
			polygon::poly_list.insert(poly);
			break;
		case 'e':
			printf("box: e: \n");
			elem1 = new element;
			elem1->read(f);
			if (!root)
				root = elem1;
			else {
				elem2 = root->find_elem(elem1->parrent);
				if (elem2)
					elem1->addnode(elem2);
				else
					error("element not found in tree::addnode()");
			}
			break;
		}
	}
	f.close();
	printf("parsing complete\n");
#if 0
	int Gd = DETECT, Gm;
	installuserdriver("SVGA256", DetectVGA256);
	initgraph(&Gd, &Gm, "");
	init_palette();
	setviewport(1, 1, 319, 199, 1);
#endif // 0

	printf("find world\n");
	elem1 = root->find_elem("world");
	printf("find box\n");
	elem2 = root->find_elem("box");
	printf("update 1\n");
	elem2->update(attrib(-32, 0, 0, 0, 0, 0, 2048));
	// while (!kbhit()) {
	printf("update 2\n");
	elem2->update(attrib(0, 3, 7, 0, 0, 0, 1024));
	printf("update tree\n");
	update_tree(root, UNIT_MAT, UNIT_MAT);
	printf("print tree\n");
	printall(root);
	printf("merge sort\n");
	pe->merge_sort();
	// clearviewport();
	printf("walk list\n");
	while (pe) {
		printf("  0\n");
		polyelem *tmp = pe;
		printf("  1\n");
		pe = pe->next;
		printf("  2\n");
		printf("  print tmp\n");
		tmp->print();
		printf("  3\n");
		delete tmp;
		printf("  4\n");
	}
	// delay(17);
	// }
	// closegraph();
	printf("done\n");
	return 0;
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
